#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferFile::CTransferFile(
	CTransferRequest* const pTransferRequest,
	const CTransferRequestInfo::SFileInfo& fileInfo) : 
	_pTransferRequest(pTransferRequest),
	_fileInfo(fileInfo)
{
	if (_pTransferRequest == nullptr)
	{
		throw std::invalid_argument("_pTransferRequest == nullptr");
	}

	wname::misc::CCounterScoped counter(*pTransferRequest);
	if (!counter.isStartOperation())
	{
		throw std::logic_error("Is not start");
	}

	/** оставляем ссылки */
	counter.release();

	/** прогресс нового файла */
	_pTransferRequest->_pTransferControl->progressTransferFileHandler(
		EProgressTransferFileHandler::eCreate,
		*_pTransferRequest,
		*this);
}
//==============================================================================
std::error_code CTransferControl::CTransferFile::request()
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	/** прогресс начала передачи */
	_pTransferRequest->_pTransferControl->progressTransferFileHandler(
		EProgressTransferFileHandler::eStartTransfer,
		*_pTransferRequest,
		*this);

	try
	{
		/** начинаем работу с участниками передачи */
		if (const auto ec = _pTransferRequest->_pMemberIn->openFile(this); ec)
		{
			return ec;
		}
		if (const auto ec = _pTransferRequest->_pMemberOut->createFile(this); ec)
		{
			/** закрываем файл который уже был открыт */
			_pTransferRequest->_pMemberIn->closeFile(this);
			return ec;
		}

		/** запускаем запросы */
		_bIsTransfer = true;
		fileIo();

		/** ожидаем завершения передачи */
		_evEndTransfer.wait();

		/** прогресс окончания передачи */
		_pTransferRequest->_pTransferControl->progressTransferFileHandler(
			EProgressTransferFileHandler::eEndTransfer,
			*_pTransferRequest,
			*this);

		/** закрываем файл передачи	и фиксируем ошибку */
		const auto ecCloseIn = _pTransferRequest->_pMemberIn->closeFile(this);
		const auto ecCloseOut = _pTransferRequest->_pMemberOut->closeFile(this);
		if (_ec)
		{
			/** внутренняя ошибка */
			return _ec;
		}

		/** ошибка закрытия файлов */
		return ecCloseIn ? ecCloseIn : ecCloseOut;
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical, ex);

		_pTransferRequest->_pMemberIn->closeFile(this);
		_pTransferRequest->_pMemberOut->closeFile(this);
		throw;
	}
}
//==============================================================================
void CTransferControl::CTransferFile::fileIo()
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
	{
		endTransfer(std::error_code(ERROR_OPERATION_ABORTED, std::system_category()));
		return;
	}

	try
	{	
		wname::cs::CCriticalSectionScoped lock(_csCounter);
		/** завершение передачи */
		if (!_bIsTransfer)
		{
			endTransfer(std::error_code(ERROR_OPERATION_ABORTED, std::system_category()));
			return;
		}

		/** запишем буфер */
		auto ec = asyncWrite();
		if (ec)
		{
			/** заканчиваем */
			endTransfer(ec);
			return;
		}

		/** прочитаем буфер */
		ec = asyncRead();
		if (ec)
		{
			/** заканчиваем */
			endTransfer(ec);
			return;
		}

		/** прогресс передачи */
		_pTransferRequest->_pTransferControl->progressTransferFileHandler(
			EProgressTransferFileHandler::eProgress,
			*_pTransferRequest,
			*this);

		if (_uWriteSize == _fileInfo.uSize && _uReadSize == _fileInfo.uSize)
		{
			/** файл полностью передан */
			endTransfer(std::error_code());
			return;
		}
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		endTransfer(std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));
		throw;
	}
}
//==============================================================================
void CTransferControl::CTransferFile::asyncReadComplitionHandler(
	CTransferFileBuffer* const pFileBufferRead,
	const std::error_code ec) noexcept
{
	if (ec)
	{
		/** заканчиваем */
		endTransfer(ec);
		endOperation();
		return;
	}

	try
	{
		assert(pFileBufferRead != nullptr);

		wname::cs::CCriticalSectionScoped lock(_csCounter);
		if (!_bIsTransfer)
		{
			/** завершение передачи */
			endTransfer(std::error_code(ERROR_OPERATION_ABORTED, std::system_category()));
			endOperation();
			return;
		}

		auto pBuffer = std::move(_listBufferReadBusy.at(pFileBufferRead));
		_listBufferReadBusy.erase(pFileBufferRead);

		/** закидываем в список на запись */
		_listBufferWrite[pFileBufferRead] = std::move(pBuffer);

		/** добавляем размер буфера к общей части прочитанного */
		_uReadSize += pFileBufferRead->_buffer.dwSize;

		/** запускаем операцию ввода/вывода */
		fileIo();
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		endTransfer(std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));
	}

	/** асинхронная операция отработала */
	endOperation();
}
//==============================================================================
void CTransferControl::CTransferFile::asyncWriteComplitionHandler(
	CTransferFileBuffer* const pFileBufferWrite,
	const std::error_code ec) noexcept
{
	if (ec)
	{
		/** заканчиваем */
		endTransfer(ec);
		endOperation();
		return;
	}

	try
	{
		assert(pFileBufferWrite != nullptr);

		wname::cs::CCriticalSectionScoped lock(_csCounter);
		if (!_bIsTransfer)
		{
			/** завершение передачи */
			endTransfer(std::error_code(ERROR_OPERATION_ABORTED, std::system_category()));
			endOperation();
			return;
		}

		auto pBuffer = std::move(_listBufferWriteBusy.at(pFileBufferWrite));
		_listBufferWriteBusy.erase(pFileBufferWrite);

		/** закидываем в список на чтение */
		_listBufferRead[pFileBufferWrite] = std::move(pBuffer);

		/** добавляем размер буфера к общей части записанного */
		_uWriteSize += pFileBufferWrite->_buffer.dwSize;

		/** запускаем операцию ввода/вывода */
		fileIo();
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		endTransfer(std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));
	}

	/** асинхронная операция отработала */
	endOperation();
}
//==============================================================================
std::error_code CTransferControl::CTransferFile::asyncRead()
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
	{
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
	}

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		if (_uReadOffset == _fileInfo.uSize)
		{
			/** весь файл прочитан, больше не требуется */
			_listBufferRead.clear();
			return std::error_code();
		}
		else if (_uReadOffset > _fileInfo.uSize)
		{
			/** разъехался протокол передачи */
			return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
		}

		if ((_listBufferReadBusy.size() +
			_listBufferWriteBusy.size() +
			_listBufferWrite.size()) >= _dwCountBuffer)
		{
			/** пока буферы переполнены, чтение не требуется */
			return std::error_code();
		}

		/** можно читать новый буфер */
		std::unique_ptr<CTransferFileBuffer> pFileBufferRead;
		if (_listBufferRead.empty())
		{
			/** свободных буферов нет, создаем */
			pFileBufferRead = std::make_unique<CTransferFileBuffer>(this);
		}
		else
		{
			/** забираем первый элемент из списка */
			pFileBufferRead = std::move(_listBufferRead.begin()->second);
			_listBufferRead.erase(pFileBufferRead.get());
		}

		/** получаем указатель на буфер */
		auto pBuffer = pFileBufferRead.get();

		/** формируем данные на чтение */
		#pragma warning (disable: 26493)
		const auto uSize = _fileInfo.uSize - _uReadOffset;
		if (uSize > sizeof(pBuffer->_buffer))
			pBuffer->_buffer.dwSize = sizeof(pBuffer->_buffer.data);
		else
			pBuffer->_buffer.dwSize = (DWORD)uSize;
		pBuffer->_buffer.offset = _uReadOffset;

		/** вставляем в обработку */
		_uReadOffset += pBuffer->_buffer.dwSize;
		_listBufferReadBusy[pBuffer] = std::move(pFileBufferRead);

		const auto ec = _pTransferRequest->_pMemberIn->startAsyncRead(pBuffer);
		if (ec)
		{
			/** ошибка чтения */
			return ec;
		}

		/** асинхронная операция выполняется */
		counter.release();
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		/** падаем дальше */
		throw;
	}

	return std::error_code();
}
//==============================================================================
std::error_code CTransferControl::CTransferFile::asyncWrite()
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
	{
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
	}

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		if (_uWriteOffset == _fileInfo.uSize)
		{
			/** весь файл записан, больше не требуется */
			return std::error_code();
		}
		else if (_uWriteOffset > _fileInfo.uSize)
		{
			/** разъехался протокол передачи */
			return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
		}

		if (_listBufferWrite.empty())
		{
			/** буферов на запись нет */
			return std::error_code();
		}

		/** забираем элемент с наименьшим смещением */
		CTransferFileBuffer* pBuffer = nullptr;
		for (const auto& [k, v] : _listBufferWrite)
		{
			if (k->_buffer.offset == _uWriteOffset)
			{
				pBuffer = k;
				break;
			}
		}

		if (pBuffer == nullptr)
		{
			/** буферы на запись еще не готовы*/
			return std::error_code();
		}

		auto pBufferWrite = std::move(_listBufferWrite.at(pBuffer));
		_listBufferWrite.erase(pBuffer);

		/** вставляем в обработку */
		_uWriteOffset += pBuffer->_buffer.dwSize;
		_listBufferWriteBusy[pBuffer] = std::move(pBufferWrite);

		const auto ec = _pTransferRequest->_pMemberOut->startAsyncWrite(pBuffer);
		if (ec)
		{
			/** ошибка записи */
			return ec;
		}

		/** асинхронная операция выполняется */
		counter.release();
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		/** падаем дальше */
		throw;
	}

	return std::error_code();
}
//==============================================================================
void CTransferControl::CTransferFile::endTransfer(
	const std::error_code ec) noexcept
{
	wname::cs::CCriticalSectionScoped lock(_csCounter);

	/** завершение передачи */
	if (!_bIsTransfer)
		return;

	_bIsTransfer = false;
	_ec = ec;
	_evEndTransfer.notify();
}
//==============================================================================
void CTransferControl::CTransferFile::release(
	const bool bIsWait) noexcept
{
	endTransfer(std::error_code(ERROR_OPERATION_ABORTED, std::system_category()));
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferFile::~CTransferFile()
{
	/** завершение всего */
	release(true);

	/** снимаем ссылки с объекта */
	_pTransferRequest->endOperation();

	/** прогресс конца файла */
	_pTransferRequest->_pTransferControl->progressTransferFileHandler(
		EProgressTransferFileHandler::eDelete,
		*_pTransferRequest,
		*this);
}
//==============================================================================