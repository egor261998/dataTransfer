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

	/** ��������� ������ */
	counter.release();

	/** �������� ������ ����� */
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

	/** �������� ������ �������� */
	_pTransferRequest->_pTransferControl->progressTransferFileHandler(
		EProgressTransferFileHandler::eStartTransfer,
		*_pTransferRequest,
		*this);

	try
	{
		/** �������� ������ � ����������� �������� */
		if (const auto ec = _pTransferRequest->_pMemberIn->openFile(this); ec)
		{
			return ec;
		}
		if (const auto ec = _pTransferRequest->_pMemberOut->createFile(this); ec)
		{
			/** ��������� ���� ������� ��� ��� ������ */
			_pTransferRequest->_pMemberIn->closeFile(this);
			return ec;
		}

		/** ��������� ������� */
		_bIsTransfer = true;
		fileIo();

		/** ������� ���������� �������� */
		_evEndTransfer.wait();

		/** �������� ��������� �������� */
		_pTransferRequest->_pTransferControl->progressTransferFileHandler(
			EProgressTransferFileHandler::eEndTransfer,
			*_pTransferRequest,
			*this);

		/** ��������� ���� ��������	� ��������� ������ */
		const auto ecCloseIn = _pTransferRequest->_pMemberIn->closeFile(this);
		const auto ecCloseOut = _pTransferRequest->_pMemberOut->closeFile(this);
		if (_ec)
		{
			/** ���������� ������ */
			return _ec;
		}

		/** ������ �������� ������ */
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
		/** ���������� �������� */
		if (!_bIsTransfer)
		{
			endTransfer(std::error_code(ERROR_OPERATION_ABORTED, std::system_category()));
			return;
		}

		/** ������� ����� */
		auto ec = asyncWrite();
		if (ec)
		{
			/** ����������� */
			endTransfer(ec);
			return;
		}

		/** ��������� ����� */
		ec = asyncRead();
		if (ec)
		{
			/** ����������� */
			endTransfer(ec);
			return;
		}

		/** �������� �������� */
		_pTransferRequest->_pTransferControl->progressTransferFileHandler(
			EProgressTransferFileHandler::eProgress,
			*_pTransferRequest,
			*this);

		if (_uWriteSize == _fileInfo.uSize && _uReadSize == _fileInfo.uSize)
		{
			/** ���� ��������� ������� */
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
		/** ����������� */
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
			/** ���������� �������� */
			endTransfer(std::error_code(ERROR_OPERATION_ABORTED, std::system_category()));
			endOperation();
			return;
		}

		auto pBuffer = std::move(_listBufferReadBusy.at(pFileBufferRead));
		_listBufferReadBusy.erase(pFileBufferRead);

		/** ���������� � ������ �� ������ */
		_listBufferWrite[pFileBufferRead] = std::move(pBuffer);

		/** ��������� ������ ������ � ����� ����� ������������ */
		_uReadSize += pFileBufferRead->_buffer.dwSize;

		/** ��������� �������� �����/������ */
		fileIo();
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		endTransfer(std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));
	}

	/** ����������� �������� ���������� */
	endOperation();
}
//==============================================================================
void CTransferControl::CTransferFile::asyncWriteComplitionHandler(
	CTransferFileBuffer* const pFileBufferWrite,
	const std::error_code ec) noexcept
{
	if (ec)
	{
		/** ����������� */
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
			/** ���������� �������� */
			endTransfer(std::error_code(ERROR_OPERATION_ABORTED, std::system_category()));
			endOperation();
			return;
		}

		auto pBuffer = std::move(_listBufferWriteBusy.at(pFileBufferWrite));
		_listBufferWriteBusy.erase(pFileBufferWrite);

		/** ���������� � ������ �� ������ */
		_listBufferRead[pFileBufferWrite] = std::move(pBuffer);

		/** ��������� ������ ������ � ����� ����� ����������� */
		_uWriteSize += pFileBufferWrite->_buffer.dwSize;

		/** ��������� �������� �����/������ */
		fileIo();
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		endTransfer(std::error_code(ERROR_INVALID_FUNCTION, std::system_category()));
	}

	/** ����������� �������� ���������� */
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
			/** ���� ���� ��������, ������ �� ��������� */
			_listBufferRead.clear();
			return std::error_code();
		}
		else if (_uReadOffset > _fileInfo.uSize)
		{
			/** ���������� �������� �������� */
			return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
		}

		if ((_listBufferReadBusy.size() +
			_listBufferWriteBusy.size() +
			_listBufferWrite.size()) >= _dwCountBuffer)
		{
			/** ���� ������ �����������, ������ �� ��������� */
			return std::error_code();
		}

		/** ����� ������ ����� ����� */
		std::unique_ptr<CTransferFileBuffer> pFileBufferRead;
		if (_listBufferRead.empty())
		{
			/** ��������� ������� ���, ������� */
			pFileBufferRead = std::make_unique<CTransferFileBuffer>(this);
		}
		else
		{
			/** �������� ������ ������� �� ������ */
			pFileBufferRead = std::move(_listBufferRead.begin()->second);
			_listBufferRead.erase(pFileBufferRead.get());
		}

		/** �������� ��������� �� ����� */
		auto pBuffer = pFileBufferRead.get();

		/** ��������� ������ �� ������ */
		#pragma warning (disable: 26493)
		const auto uSize = _fileInfo.uSize - _uReadOffset;
		if (uSize > sizeof(pBuffer->_buffer))
			pBuffer->_buffer.dwSize = sizeof(pBuffer->_buffer.data);
		else
			pBuffer->_buffer.dwSize = (DWORD)uSize;
		pBuffer->_buffer.offset = _uReadOffset;

		/** ��������� � ��������� */
		_uReadOffset += pBuffer->_buffer.dwSize;
		_listBufferReadBusy[pBuffer] = std::move(pFileBufferRead);

		const auto ec = _pTransferRequest->_pMemberIn->startAsyncRead(pBuffer);
		if (ec)
		{
			/** ������ ������ */
			return ec;
		}

		/** ����������� �������� ����������� */
		counter.release();
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		/** ������ ������ */
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
			/** ���� ���� �������, ������ �� ��������� */
			return std::error_code();
		}
		else if (_uWriteOffset > _fileInfo.uSize)
		{
			/** ���������� �������� �������� */
			return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
		}

		if (_listBufferWrite.empty())
		{
			/** ������� �� ������ ��� */
			return std::error_code();
		}

		/** �������� ������� � ���������� ��������� */
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
			/** ������ �� ������ ��� �� ������*/
			return std::error_code();
		}

		auto pBufferWrite = std::move(_listBufferWrite.at(pBuffer));
		_listBufferWrite.erase(pBuffer);

		/** ��������� � ��������� */
		_uWriteOffset += pBuffer->_buffer.dwSize;
		_listBufferWriteBusy[pBuffer] = std::move(pBufferWrite);

		const auto ec = _pTransferRequest->_pMemberOut->startAsyncWrite(pBuffer);
		if (ec)
		{
			/** ������ ������ */
			return ec;
		}

		/** ����������� �������� ����������� */
		counter.release();
	}
	catch (const std::exception& ex)
	{
		_pTransferRequest->_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		/** ������ ������ */
		throw;
	}

	return std::error_code();
}
//==============================================================================
void CTransferControl::CTransferFile::endTransfer(
	const std::error_code ec) noexcept
{
	wname::cs::CCriticalSectionScoped lock(_csCounter);

	/** ���������� �������� */
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
	/** ���������� ����� */
	release(true);

	/** ������� ������ � ������� */
	_pTransferRequest->endOperation();

	/** �������� ����� ����� */
	_pTransferRequest->_pTransferControl->progressTransferFileHandler(
		EProgressTransferFileHandler::eDelete,
		*_pTransferRequest,
		*this);
}
//==============================================================================