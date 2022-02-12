#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferRequest::CTransferRequest(
	CTransferControl* const pTransferControl,
	const ERequest eRequest,
	const std::filesystem::path requestPath,
	ITransferMember* const pMemberIn,
	ITransferMember* const pMemberOut):
	_eRequest(eRequest), 
	_requestPath(requestPath),
	_pTransferControl(pTransferControl),
	_pMemberIn(pMemberIn),
	_pMemberOut(pMemberOut)
{
	if (_pTransferControl == nullptr ||
		_pMemberIn == nullptr || 
		_pMemberOut == nullptr)
	{
		throw std::invalid_argument("_pTransferControl == nullptr || _pMemberIn == nullptr || _pMemberOut == nullptr");
	}

	wname::misc::CCounterScoped counterMemberIn(*_pMemberIn);
	wname::misc::CCounterScoped counterMemberOut(*_pMemberOut);
	if (!counterMemberIn.isStartOperation() ||
		!counterMemberOut.isStartOperation())
	{
		throw std::logic_error("Is not start");
	}	

	/** оставляем ссылки */
	counterMemberIn.release();
	counterMemberOut.release();
}
//==============================================================================
std::error_code CTransferControl::CTransferRequest::request()
{
	/**
	* запрос у участника передачи и отправка информации участнику передачи.
	* @param getInfo - информация о запросе для получения.
	* @param putInfo - информация о запросе для заполнения.
	* @return - статус завершения запроса.
	*/
	auto requestInfo = [](
		CTransferRequestInfo& getInfo,
		CTransferRequestInfo& putInfo)
	{
		try
		{
			auto ec = getInfo._pMember->getRequestInfo(getInfo);
			if (ec)
				return ec;

			ec = putInfo._pMember->putRequestInfo(getInfo);
			if (ec)
				return ec;
		}
		catch (const std::exception& ex)
		{
			getInfo._pMember->_pTransferControl->_pIocp->log(
				wname::logger::EMessageType::critical,
				ex);
			throw;
		}

		/** успешный запрос */
		return std::error_code();
	};

	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
	{
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
	}

	if (!_pMemberIn->_sAccessFlag.bIsCopy || !_pMemberOut->_sAccessFlag.bIsPaste)
	{
		/** нет доступа для данного запроса */
		return std::error_code(ERROR_ACCESS_DENIED, std::system_category());
	}

	/** создаем запросы */
	CTransferRequestInfo requestInfoIn(this, _pMemberIn);
	CTransferRequestInfo requestInfoOut(this, _pMemberOut);

	try
	{
		/** начинаем работу с участниками передачи */
		if (const auto ec = _pMemberIn->openMember(this); ec)
		{
			return ec;
		}
		if (const auto ec = _pMemberOut->openMember(this); ec)
		{
			/** закрываем файл который уже был открыт */
			_pMemberIn->closeMember(this);
			return ec;
		}

		/** запускаем запросы */
		auto resultIn = std::async(std::launch::async, std::bind(
			requestInfo,
			std::ref(requestInfoIn),
			std::ref(requestInfoOut)));
		auto resultOut = std::async(std::launch::async, std::bind(
			requestInfo,
			std::ref(requestInfoOut),
			std::ref(requestInfoIn)));

		/** ожидание запросов */
		resultIn.wait();
		resultOut.wait();

		/** проверка успеха запросов */
		if (const auto ec = resultIn.get(); ec)
			return ec;
		if (const auto ec = resultOut.get(); ec)
			return ec;

		for (const auto& itIn : requestInfoIn._files)
		{
			for (const auto& itOut : requestInfoOut._files)
			{
				if (_wcsicmp(&itIn.wszFilePath[0], &itOut.wszFilePath[0]) == 0 &&
					!_pMemberOut->_sAccessFlag.bIsOverrite)
				{
					/** нет доступа для данного запроса */
					return std::error_code(ERROR_ACCESS_DENIED, std::system_category());
				}
			}

			/** передача файла */
			const auto ec = transferFile(itIn);
			if (ec)
				return ec;
		}

		/** закрываем участников передачи для запроса
			и фиксируем ошибку */
		const auto ecCloseIn = _pMemberIn->closeMember(this);
		const auto ecCloseOut = _pMemberOut->closeMember(this);
		return ecCloseIn ? ecCloseIn : ecCloseOut;
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		_pMemberIn->closeMember(this);
		_pMemberOut->closeMember(this);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferRequest::transferFile(
	const CTransferRequestInfo::SFileInfo& fileInfo)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
	{
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
	}

	try
	{
		/** передача файла */
		CTransferFile transferFile(this, fileInfo);
		return transferFile.request();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);
		throw;
	}
}
//==============================================================================
void CTransferControl::CTransferRequest::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferRequest::~CTransferRequest()
{
	/** завершение всего */
	release(true);

	/** снимаем ссылки с объектов */
	_pMemberIn->endOperation();
	_pMemberOut->endOperation();
}
//==============================================================================