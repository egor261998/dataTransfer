#include "../stdafx.h"

#define CTcpConnectedClientNetworkTestPrefix datatransfer::networktest::CNetworkTest::CTcpServerNetworkTest::CTcpConnectedClientNetworkTest

//==============================================================================
CTcpConnectedClientNetworkTestPrefix::CTcpConnectedClientNetworkTest(
	CTcpServer* const pParent)
	: CTcpConnectedClient(pParent)
{

}
//==============================================================================
void CTcpConnectedClientNetworkTestPrefix::getStatistic(
	SStatisticClient& sStatisticClient,
	const DWORD dwDifTime) noexcept
{
	if (sStatisticClient.tickCount == 0)
	{
		sStatisticClient.tickCount = GetTickCount64();
	}

	UINT64 tickCountDif = GetTickCount64() - sStatisticClient.tickCount;

	/** �������� ������� */
	if (tickCountDif < dwDifTime)
		return;

	tickCountDif/= dwDifTime;

	/** �������� ���������� */
	memcpy(&sStatisticClient.info, &_sInfoClient, 
		sizeof(_sInfoClient));

	/** ��������� ���������� �� �������� */
	sStatisticClient.nSendData = _nCountWriteByte - sStatisticClient.nAllSendData;
	sStatisticClient.nAllSendData += sStatisticClient.nSendData;
	sStatisticClient.nAvgSendData = sStatisticClient.nAllSendData / tickCountDif;

	/** ��������� ���������� �� ����� */
	sStatisticClient.nRecvData = _nCountReadByte - sStatisticClient.nAllRecvData;
	sStatisticClient.nAllRecvData += sStatisticClient.nRecvData;
	sStatisticClient.nAvgRecvData = sStatisticClient.nAllRecvData / tickCountDif;
}
//==============================================================================
const wname::network::socket::CSocketAddress& CTcpConnectedClientNetworkTestPrefix::getAddress() noexcept
{
	return _remotelAddress;
}
//==============================================================================
void CTcpConnectedClientNetworkTestPrefix::clientAsyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	#pragma warning(disable: 26493)
	UNREFERENCED_PARAMETER(bufferRecv);
	UNREFERENCED_PARAMETER(dwReturnSize);

	if (ec)
	{
		/** ������ ������ */
		return;
	}

	try
	{
		if (_sInfoClient.bIsRecv)
		{
			const auto ecRecv = startAsyncRecv(
				bufferRecv, dwReturnSize, MSG_WAITALL);
			if (ecRecv)
			{
				/** ������ ������ ������ */
				return;
			}
		}
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
	}
}
//==============================================================================
void CTcpConnectedClientNetworkTestPrefix::clientAsyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	#pragma warning(disable: 26493)
	UNREFERENCED_PARAMETER(bufferSend);
	UNREFERENCED_PARAMETER(dwReturnSize);

	if (ec)
	{
		/** ������ �������� */
		return;
	}

	try
	{
		if (_sInfoClient.bIsSend)
		{
			const auto ecSend = startAsyncSend(
				bufferSend, dwReturnSize);
			if (ecSend)
			{
				/** ������ ������ �������� */
				return;
			}
		}
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
	}
}
//==============================================================================
void CTcpConnectedClientNetworkTestPrefix::clientConnected(
	const std::error_code ec) noexcept
{
	#pragma warning(disable: 26493)

	if (ec)
	{
		/** ����� */
		dynamic_cast<CTcpServerNetworkTest*>(_pParent)->_pNetworkTest->connectedClient(this, ec);
		return;
	}

	try
	{
		if (!_socket.setKeepAlive(true))
		{
			/** ������ ��������� �������� */
			dynamic_cast<CTcpServerNetworkTest*>(_pParent)->_pNetworkTest->connectedClient(this, 
				std::error_code(WSAGetLastError(), std::system_category()));
			return;
		}

		/** �������� ���������� � ���� */
		DWORD dwReturnedByte = 0;
		SInfoClient sInfoClient;
		auto ecRecv = startRecv(
			(PBYTE)&sInfoClient, sizeof(sInfoClient), &dwReturnedByte, MSG_WAITALL);
		if (ecRecv)
		{
			/** ������ ������ ������ */
			dynamic_cast<CTcpServerNetworkTest*>(_pParent)->_pNetworkTest->connectedClient(this, ecRecv);
			return;
		}

		memcpy(&_sInfoClient, &sInfoClient, sizeof(sInfoClient));

		/** ����� ������� */
		_sInfoClient.bIsRecv = sInfoClient.bIsSend;
		_sInfoClient.bIsSend = sInfoClient.bIsRecv;

		/** �������� ������ ��� �������� */
		if (_sInfoClient.bIsRecv)
		{
			for (WORD i = 0; i < _sInfoClient.wBufferCount; i++)
			{
				_listBufferRecv.push_back(
					std::vector<BYTE>(_sInfoClient.dwSizeBuffer));
			}
		}

		if (_sInfoClient.bIsSend)
		{
			for (WORD i = 0; i < _sInfoClient.wBufferCount; i++)
			{
				_listBufferSend.push_back(
					std::vector<BYTE>(_sInfoClient.dwSizeBuffer));
			}
		}

		if (_sInfoClient.bIsRecv)
		{
			for (auto& it : _listBufferRecv)
			{
				ecRecv = startAsyncRecv(
					it.data(), (DWORD)it.size(), MSG_WAITALL);
				if (ecRecv)
				{
					/** ������ ������ ������ */
					dynamic_cast<CTcpServerNetworkTest*>(_pParent)->_pNetworkTest->connectedClient(this, ecRecv);
					return;
				}
			}
		}

		if (_sInfoClient.bIsSend)
		{
			for (auto& it : _listBufferSend)
			{
				const auto ecSend = startAsyncSend(
					it.data(), (DWORD)it.size());
				if (ecSend)
				{
					/** ������ ������ �������� */
					dynamic_cast<CTcpServerNetworkTest*>(_pParent)->_pNetworkTest->connectedClient(this, ecSend);
					return;
				}
			}
		}

		/** ������������ � ������������ ������� */
		dynamic_cast<CTcpServerNetworkTest*>(_pParent)->_pNetworkTest->connectedClient(this);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		dynamic_cast<CTcpServerNetworkTest*>(_pParent)->_pNetworkTest->connectedClient(this,
			std::error_code(ERROR_FUNCTION_FAILED, std::system_category()));
	}
}
//==============================================================================
void CTcpConnectedClientNetworkTestPrefix::clientDisconnected(
	const std::error_code ec) noexcept
{
	/** ����������� �� ������������� ������� */
	dynamic_cast<CTcpServerNetworkTest*>(_pParent)->_pNetworkTest->disconnectedClient(this, ec);
}
//==============================================================================
CTcpConnectedClientNetworkTestPrefix::~CTcpConnectedClientNetworkTest()
{
	/** ���� ���������� ����� */
	release();
}
//==============================================================================