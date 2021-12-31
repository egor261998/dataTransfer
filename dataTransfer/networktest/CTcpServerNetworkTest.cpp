#include "../stdafx.h"

#define CTcpServerNetworkTestPrefix datatransfer::networktest::CNetworkTest::CTcpServerNetworkTest

//==============================================================================
CTcpServerNetworkTestPrefix::CTcpServerNetworkTest(
	CNetworkTest* const pNetworkTest,
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp)
	: CTcpServer(strIp, wPort, pIocp),
	_pNetworkTest(pNetworkTest)
{
	connectServer();
}
//==============================================================================
std::unique_ptr<CTcpServerNetworkTestPrefix::CTcpConnectedClient> CTcpServerNetworkTestPrefix::createClient()
{
	try
	{
		/** создание клиента по-умолчанию */
		return std::make_unique<CTcpConnectedClientNetworkTest>((CTcpServer*)this);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTcpServerNetworkTestPrefix::serverConnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CTcpServerNetworkTestPrefix::serverDisconnected(
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
CTcpServerNetworkTestPrefix::~CTcpServerNetworkTest()
{
	/** отключаем */
	disconnectServer();

	/** ждем завершения всего */
	release();
}
//==============================================================================