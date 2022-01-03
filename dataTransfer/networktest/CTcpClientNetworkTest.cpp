#include "../stdafx.h"

#define CTcpClientNetworkTestPrefix datatransfer::networktest::CNetworkTest::CTcpClientNetworkTest

//==============================================================================
CTcpClientNetworkTestPrefix::CTcpClientNetworkTest(
	CNetworkTest* const pNetworkTest,
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp)
	: CTcpClient(strIp, wPort, pIocp),
	_pNetworkTest(pNetworkTest)
{
	try
	{
		if (_pNetworkTest == nullptr)
			throw std::invalid_argument("_pNetworkTest == nullptr");

		/** �������� ������ ��� �������� */
		if (_pNetworkTest->_sInfoClient.bIsRecv)
		{
			for (WORD i = 0; i < _pNetworkTest->_sInfoClient.wBufferCount; i++)
			{
				_listBufferRecv.push_back(
					std::vector<BYTE>(_pNetworkTest->_sInfoClient.dwSizeBuffer));
			}
		}
			
		if (_pNetworkTest->_sInfoClient.bIsSend)
		{
			for (WORD i = 0; i < _pNetworkTest->_sInfoClient.wBufferCount; i++)
			{
				_listBufferSend.push_back(
					std::vector<BYTE>(_pNetworkTest->_sInfoClient.dwSizeBuffer));
			}
		}

		if (!_pNetworkTest->startOperation())
		{
			throw std::logic_error("pNetworkTest is not start");
		}
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTcpClientNetworkTestPrefix::getStatistic(
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

	tickCountDif /= dwDifTime;

	/** �������� ���������� */
	memcpy(&sStatisticClient.info, &_pNetworkTest->_sInfoClient, 
		sizeof(_pNetworkTest->_sInfoClient));

	/** ��������� ���������� �� �������� */
	sStatisticClient.nSendData = _nCountWriteByte - sStatisticClient.nAllSendData;
	sStatisticClient.nAllSendData += sStatisticClient.nSendData;
	sStatisticClient.nAvgSendData = sStatisticClient.nAllSendData / tickCountDif;

	/** ��������� ���������� �� ����� */
	sStatisticClient.nRecvData = _nCountReadByte - sStatisticClient.nAllRecvData;
	sStatisticClient.nAllRecvData += sStatisticClient.nRecvData;
	sStatisticClient.nAvgRecvData = sStatisticClient.nAllRecvData / tickCountDif;

	/** ���� �� ��������������� */
	sStatisticClient.dwRecconect = _dwRecconect;
}
//==============================================================================
const wname::network::socket::CSocketAddress& CTcpClientNetworkTestPrefix::getAddress() noexcept
{
	return CTcpClient::getAddress();
}
//==============================================================================
void CTcpClientNetworkTestPrefix::clientAsyncRecvComplitionHandler(
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
		if (_pNetworkTest->_sInfoClient.bIsRecv)
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
void CTcpClientNetworkTestPrefix::clientAsyncSendComplitionHandler(
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
		if (_pNetworkTest->_sInfoClient.bIsSend)
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
void CTcpClientNetworkTestPrefix::clientConnected(
	const std::error_code ec) noexcept
{
	#pragma warning(disable: 26493)

	if (ec)
	{
		/** ����� */ 
		_pNetworkTest->connectedClient(this, ec);
		return;
	}

	try
	{
		const auto ecKeepAlive = setKeepAlive(true);
		if(ecKeepAlive)
		{
			/** ������ ��������� ����� */
			_pNetworkTest->connectedClient(this, ecKeepAlive);
			disconnect(ecKeepAlive);
			return;
		}

		/** ���������� ���������� � ���� */
		DWORD dwReturnedByte = 0;
		auto ecSend = startSend(
			(PBYTE)&_pNetworkTest->_sInfoClient, 
			sizeof(_pNetworkTest->_sInfoClient),
			&dwReturnedByte);
		if (ecSend)
		{
			/** ������ ������ �������� */
			_pNetworkTest->connectedClient(this, ecSend);
			return;
		}

		if (_pNetworkTest->_sInfoClient.bIsRecv)
		{
			for (auto &it : _listBufferRecv)
			{
				const auto ecRecv = startAsyncRecv(
					it.data(), (DWORD)it.size(), MSG_WAITALL);
				if (ecRecv)
				{
					/** ������ ������ ������ */
					_pNetworkTest->connectedClient(this, ecRecv);
					return;
				}
			}		
		}

		if (_pNetworkTest->_sInfoClient.bIsSend)
		{
			for (auto& it : _listBufferSend)
			{
				ecSend = startAsyncSend(
					it.data(), (DWORD)it.size());
				if (ecSend)
				{
					/** ������ ������ �������� */
					_pNetworkTest->connectedClient(this, ec);
					return;
				}
			}	
		}

		/** ������������ � ������������ ������� */
		_pNetworkTest->connectedClient(this);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		_pNetworkTest->connectedClient(this, 
			std::error_code(ERROR_FUNCTION_FAILED, std::system_category()));
	}
}
//==============================================================================
void CTcpClientNetworkTestPrefix::clientDisconnected(
	const std::error_code ec) noexcept
{
	/** ����������� �� ������������� ������� */
	_pNetworkTest->disconnectedClient(this, ec);

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
		const auto ecConnect = connect();
		if (!ecConnect)
		{
			/** ������������ */
			break;
		}
	}
}
//==============================================================================
CTcpClientNetworkTestPrefix::~CTcpClientNetworkTest()
{
	/** ��������� */
	disconnect();

	/** ���� ���������� ����� */
	release();

	_pNetworkTest->endOperation();
}
//==============================================================================