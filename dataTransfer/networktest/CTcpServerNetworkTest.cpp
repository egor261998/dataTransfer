#include "../stdafx.h"

using datatransfer::networktest::CNetworkTest;

//==============================================================================
CNetworkTest::CTcpServerNetworkTest::CTcpServerNetworkTest(
	CNetworkTest* const pNetworkTest,
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp) :
	CTcpServer(strIp, wPort, pIocp),
	_pNetworkTest(pNetworkTest)
{
	try
	{
		if (_pNetworkTest == nullptr)
			throw std::invalid_argument("_pNetworkTest == nullptr");
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
	
	if (!_pNetworkTest->startOperation())
	{
		throw std::logic_error("pNetworkTest is not start");
	}
}
//==============================================================================
std::unique_ptr<CNetworkTest::CTcpServerNetworkTest::CTcpConnectedClient> 
CNetworkTest::CTcpServerNetworkTest::createClient()
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
void CNetworkTest::CTcpServerNetworkTest::serverConnected(
	const std::error_code ec) noexcept
{
	if (ec)
	{
		_pIocp->log(wname::logger::EMessageType::warning, L"serverConnected failed", ec);
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

		/** переподключение */
		_dwRecconect++;
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
void CNetworkTest::CTcpServerNetworkTest::serverDisconnected(
	const std::error_code ec) noexcept
{
	if (ec)
	{
		_pIocp->log(wname::logger::EMessageType::warning, L"serverDisconnected failed", ec);
	}
}
//==============================================================================
void CNetworkTest::CTcpServerNetworkTest::release(
	const bool bIsWait) noexcept
{
	__super::release(false);
	/** отключаем */
	disconnectServer();

	/** ждем завершения всего */
	__super::release(bIsWait);
}
//==============================================================================
CNetworkTest::CTcpServerNetworkTest::~CTcpServerNetworkTest()
{
	/** завершение всего */
	release(true);

	_pNetworkTest->endOperation();
}
//==============================================================================