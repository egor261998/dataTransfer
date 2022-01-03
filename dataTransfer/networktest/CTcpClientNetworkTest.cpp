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

		/** выдел€ем буферы под операции */
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

	/** рановато считать */
	if (tickCountDif < dwDifTime)
		return;

	tickCountDif /= dwDifTime;

	/** копируем информацию */
	memcpy(&sStatisticClient.info, &_pNetworkTest->_sInfoClient, 
		sizeof(_pNetworkTest->_sInfoClient));

	/** заполн€ем статистику на отправку */
	sStatisticClient.nSendData = _nCountWriteByte - sStatisticClient.nAllSendData;
	sStatisticClient.nAllSendData += sStatisticClient.nSendData;
	sStatisticClient.nAvgSendData = sStatisticClient.nAllSendData / tickCountDif;

	/** заполн€ем статистику на прием */
	sStatisticClient.nRecvData = _nCountReadByte - sStatisticClient.nAllRecvData;
	sStatisticClient.nAllRecvData += sStatisticClient.nRecvData;
	sStatisticClient.nAvgRecvData = sStatisticClient.nAllRecvData / tickCountDif;

	/** были ли переподключени€ */
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
		/** ошибка приема */
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
				/** ошибка старта приема */
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
		/** ошибка отправки */
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
				/** ошибка старта отправки */
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
		/** валим */ 
		_pNetworkTest->connectedClient(this, ec);
		return;
	}

	try
	{
		const auto ecKeepAlive = setKeepAlive(true);
		if(ecKeepAlive)
		{
			/** ошибка установки опции */
			_pNetworkTest->connectedClient(this, ecKeepAlive);
			disconnect(ecKeepAlive);
			return;
		}

		/** отправл€ем информацию о сети */
		DWORD dwReturnedByte = 0;
		auto ecSend = startSend(
			(PBYTE)&_pNetworkTest->_sInfoClient, 
			sizeof(_pNetworkTest->_sInfoClient),
			&dwReturnedByte);
		if (ecSend)
		{
			/** ошибка старта отправки */
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
					/** ошибка старта приема */
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
					/** ошибка старта отправки */
					_pNetworkTest->connectedClient(this, ec);
					return;
				}
			}	
		}

		/** подключаемс€ к управл€ющему объекту */
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
	/** отключаемс€ от управл€ющиего объекта */
	_pNetworkTest->disconnectedClient(this, ec);

	/** пробуем подключитьс€ снова */
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
		const auto ecConnect = connect();
		if (!ecConnect)
		{
			/** подключились */
			break;
		}
	}
}
//==============================================================================
CTcpClientNetworkTestPrefix::~CTcpClientNetworkTest()
{
	/** отключаем */
	disconnect();

	/** ждем завершени€ всего */
	release();

	_pNetworkTest->endOperation();
}
//==============================================================================