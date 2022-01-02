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
std::unique_ptr<CTcpServerNetworkTestPrefix::CTcpConnectedClient> CTcpServerNetworkTestPrefix::createClient()
{
	try
	{
		/** �������� ������� ��-��������� */
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
	if (ec)
	{
		_pIocp->log(wname::logger::EMessageType::warning, L"serverConnected failed", ec);
	}	

	/** ������� ������������ ����� */
	while (ec)
	{
		wname::misc::CCounterScoped counter(*this);
		if (!counter.isStartOperation())
		{
			/** ���������� */
			return;
		}

		/** ��������������� */
		_dwRecconect++;
		const auto ecConnect = connectServer();
		if (!ecConnect)
		{
			/** ������������ */
			break;
		}
	}
}
//==============================================================================
void CTcpServerNetworkTestPrefix::serverDisconnected(
	const std::error_code ec) noexcept
{
	if (ec)
	{
		_pIocp->log(wname::logger::EMessageType::warning, L"serverDisconnected failed", ec);
	}
}
//==============================================================================
CTcpServerNetworkTestPrefix::~CTcpServerNetworkTest()
{
	/** ��������� */
	disconnectServer();

	/** ���� ���������� ����� */
	release();

	_pNetworkTest->endOperation();
}
//==============================================================================