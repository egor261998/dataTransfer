#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferTcpClient::CTransferTcpClient(
	CTransferControl* const pTransferControl,
	const SAccessFlag& sAccessFlag,
	const std::string strIp,
	const WORD wPort) :
	ITransferMember(pTransferControl, sAccessFlag),
	_strIp(strIp),
	_wPort(wPort)
{
	
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpClient::openMember(
	CTransferRequest* const pTransferRequest)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		if (_transferActive.find(pTransferRequest) != _transferActive.end())
		{
			/** такой запрос уже есть */
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());
		}

		/** создаем новую TCP сессию */
		auto pClient = std::make_shared<CClient>(
			this, _strIp, _wPort, _pTransferControl->_pIocp);
		auto ec = pClient->connect();
		if (ec)
		{
			/** не удалось соединиться */
			return ec;
		}

		/** отправляем путь запроса */
		ec = pClient->sendRequestPath(
			pTransferRequest->_eRequest, pTransferRequest->_requestPath);
		if (ec)
		{
			/** не удалось отправить путь */
			return ec;
		}

		_transferActive[pTransferRequest] = pClient;
		return ec;
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpClient::getRequestInfo(
	CTransferRequestInfo& requestInfo)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::shared_ptr<CClient> pClient;

		{
			wname::cs::CCriticalSectionScoped lock(_csCounter);
			pClient = _transferActive.at(requestInfo._pTransferRequest);
		}

		return pClient->getRequestInfo(requestInfo);
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpClient::putRequestInfo(
	const CTransferRequestInfo& requestInfo)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::shared_ptr<CClient> pClient;

		{
			wname::cs::CCriticalSectionScoped lock(_csCounter);
			pClient = _transferActive.at(requestInfo._pTransferRequest);
		}

		return pClient->putRequestInfo(requestInfo);
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpClient::openFile(
	CTransferFile* const pTransferFile)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		return _transferActive.at(pTransferFile->_pTransferRequest)->operationFile(pTransferFile);
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpClient::createFile(
	CTransferFile* const pTransferFile)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		return _transferActive.at(pTransferFile->_pTransferRequest)->operationFile(pTransferFile);
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpClient::startAsyncRead(
	CTransferFileBuffer* const pFileBufferRead)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		return _transferActive.at(pFileBufferRead->_pTransferFile->_pTransferRequest)
			->ITransferTcpCommunication::startAsyncRead(pFileBufferRead);
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpClient::startAsyncWrite(
	CTransferFileBuffer* const pFileBufferWrite)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		return _transferActive.at(pFileBufferWrite->_pTransferFile->_pTransferRequest)
			->ITransferTcpCommunication::startAsyncWrite(pFileBufferWrite);
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpClient::closeFile(
	CTransferFile* const pTransferFile) noexcept
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		return _transferActive.at(pTransferFile->_pTransferRequest)->operationFile(pTransferFile);
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpClient::closeMember(
	CTransferRequest* const pTransferRequest) noexcept
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	wname::cs::CCriticalSectionScoped lock(_csCounter);

	if (_transferActive.erase(pTransferRequest) == 0)
		return std::error_code(ERROR_NOT_FOUND, std::system_category());

	return std::error_code();
}
//==============================================================================
void CTransferControl::CTransferTcpClient::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferTcpClient::~CTransferTcpClient()
{
	release(true);
}
//==============================================================================