#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferTcpServer::CTransferTcpServer(
	CTransferControl* const pTransferControl,
	const SAccessFlag& sAccessFlag,
	const std::string strIp,
	const WORD wPort) :
	ITransferMember(pTransferControl, sAccessFlag)
{
	try
	{
		_pTcpServerFromMember = std::make_unique<CTcpServerFromMember>(
			this, strIp, wPort, pTransferControl->_pIocp);
		_pTcpServerFromMember->connectServer();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpServer::openMember(
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

		/** получаем клиента */
		auto& list = _transferFree.at(pTransferRequest->_requestPath);
		auto pClient = list.begin();

		_transferActive[pTransferRequest] = *pClient;
		list.erase(pClient);
		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpServer::getRequestInfo(
	CTransferRequestInfo& requestInfo)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		return _transferActive.at(requestInfo._pTransferRequest)->getRequestInfo(requestInfo);
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpServer::putRequestInfo(
	const CTransferRequestInfo& requestInfo)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		return _transferActive.at(requestInfo._pTransferRequest)->putRequestInfo(requestInfo);
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferTcpServer::openFile(
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
std::error_code CTransferControl::CTransferTcpServer::createFile(
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
std::error_code CTransferControl::CTransferTcpServer::startAsyncRead(
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
std::error_code CTransferControl::CTransferTcpServer::startAsyncWrite(
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
std::error_code CTransferControl::CTransferTcpServer::closeFile(
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
std::error_code CTransferControl::CTransferTcpServer::closeMember(
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
void CTransferControl::CTransferTcpServer::release(
	const bool bIsWait) noexcept
{
	__super::release(false);
	_pTcpServerFromMember.reset();
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferTcpServer::~CTransferTcpServer()
{
	release(true);
}
//==============================================================================