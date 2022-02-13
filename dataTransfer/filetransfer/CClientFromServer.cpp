#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferTcpServer::CClientFromServer::CClientFromServer(
	wname::network::CTcpServer* const pParent) :
	CTcpConnectedClient(pParent),
	ITransferTcpCommunication(this)
{

}
//==============================================================================
void CTransferControl::CTransferTcpServer::CClientFromServer::workerRequest() noexcept
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return;

	auto pTcpServer = dynamic_cast<CTcpServerFromMember*>(_pParent);
	assert(pTcpServer != nullptr);
	std::wstring requestPath;

	try
	{
		ERequest eRequest = ERequest::eNone;

		auto recvEc = recvRequestPath(
			eRequest, requestPath);
		if (recvEc)
		{
			return;
		}

		{
			wname::cs::CCriticalSectionScoped lock(pTcpServer->_pTransferTcpServer->_csCounter);
			auto& transferFree = pTcpServer->_pTransferTcpServer->_transferFree;
			if (transferFree.find(requestPath) == transferFree.end())
			{
				transferFree[requestPath] = std::list<CClientFromServer*>();
				transferFree.at(requestPath).push_back(this);
			}

			transferFree.at(requestPath).push_back(this);
		}

		recvEc = pTcpServer->_pTransferTcpServer->_pTransferControl->request(
			eRequest, requestPath, pTcpServer->_pTransferTcpServer);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
	}

	try
	{
		wname::cs::CCriticalSectionScoped lock(pTcpServer->_pTransferTcpServer->_csCounter);
		auto& transferFree = pTcpServer->_pTransferTcpServer->_transferFree;
		if (auto kv = transferFree.find(requestPath); kv != transferFree.end())
		{
			for (auto it = kv->second.begin(); it != kv->second.end(); it++)
			{
				if (*it == this)
				{
					kv->second.erase(it);
					break;
				}
			}

			if (kv->second.empty())
			{
				transferFree.erase(requestPath);
			}
		}
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
	}

	disconnect();
}
//==============================================================================
void CTransferControl::CTransferTcpServer::CClientFromServer::clientConnected(
	const std::error_code ec) noexcept
{
	if (ec)
	{
		return;
	}

	try
	{
		auto th = std::thread(&CClientFromServer::workerRequest, this);
		th.detach();
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		disconnect();
	}	
}
//==============================================================================
void CTransferControl::CTransferTcpServer::CClientFromServer::clientAsyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	ITransferTcpCommunication::asyncRecvComplitionHandler(
		bufferRecv, dwReturnSize, ec);
}
//==============================================================================
void CTransferControl::CTransferTcpServer::CClientFromServer::clientAsyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	ITransferTcpCommunication::asyncSendComplitionHandler(
		bufferSend, dwReturnSize, ec);
}
//==============================================================================
void CTransferControl::CTransferTcpServer::CClientFromServer::release(
	const bool bIsWait) noexcept
{
	/** ждем завершения всего */
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferTcpServer::CClientFromServer::~CClientFromServer()
{
	/** завершение всего */
	release(true);
}
//==============================================================================