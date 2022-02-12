#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferTcpServer::CTcpServerFromMember::CTcpServerFromMember(
	CTransferTcpServer* const pTransferTcpServer,
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp) : 
	CTcpServer(strIp, wPort, pIocp),
	_pTransferTcpServer(pTransferTcpServer)
{
	try
	{
		if (_pTransferTcpServer == nullptr)
			throw std::invalid_argument("_pTransferTcpServer == nullptr");
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}

	if (!_pTransferTcpServer->startOperation())
	{
		throw std::logic_error("_pTransferTcpServer is not start");
	}
}
//==============================================================================
void CTransferControl::CTransferTcpServer::CTcpServerFromMember::serverConnected(
	const std::error_code ec) noexcept
{
	if (ec)
	{
		_pIocp->log(
			wname::logger::EMessageType::warning,
			L"serverConnected failed", ec);
	}

	/** пробуем подключиться снова */
	while (ec)
	{
		wname::misc::CCounterScoped counter(*this);
		if (!counter.isStartOperation())
		{
			/** выключение */
			return;
		}

		try
		{
			const auto ecConnect = connectServer();
			if (!ecConnect)
			{
				/** подключились */
				break;
			}
		}
		catch (const std::exception& ex)
		{
			_pIocp->log(wname::logger::EMessageType::warning, ex);
		}
	}
}
//==============================================================================
void CTransferControl::CTransferTcpServer::CTcpServerFromMember::serverDisconnected(
	const std::error_code ec) noexcept
{
	if (ec)
	{
		_pIocp->log(
			wname::logger::EMessageType::warning,
			L"serverDisconnected failed", ec);
	}
}
//==============================================================================
std::unique_ptr<CTransferControl::CTransferTcpServer::CTcpServerFromMember::CTcpConnectedClient>
CTransferControl::CTransferTcpServer::CTcpServerFromMember::createClient()
{
	try
	{
		/** создание клиента по-умолчанию */
		return std::make_unique<CClientFromServer>((CTcpServer*)this);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTransferControl::CTransferTcpServer::CTcpServerFromMember::release(
	const bool bIsWait) noexcept
{
	__super::release(false);
	/** отключаем */
	disconnectServer();

	/** ждем завершения всего */
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferTcpServer::CTcpServerFromMember::~CTcpServerFromMember()
{
	/** завершение всего */
	release(true);

	_pTransferTcpServer->endOperation();
}
//==============================================================================