#include "../stdafx.h"

using datatransfer::networktest::CNetworkTest;

//==============================================================================
CNetworkTest::CNetworkTest(
	const std::string strIp,
	const WORD wPort,
	const DWORD dwCountClient,
	const SInfoClient& sInfoClient,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp)
{
	if (pIocp == nullptr)
		throw std::invalid_argument("pIocp == nullptr");

	try
	{
		_pIocp = pIocp;

		if (!sInfoClient.bIsRecv && !sInfoClient.bIsSend)
			throw std::invalid_argument("!sInfoClient.bIsRecv && !sInfoClient.bIsSend");

		if (sInfoClient.dwBandWidth == 0)
			throw std::invalid_argument("sInfoClient.dwBandwidth == 0");

		if (sInfoClient.dwSizeBuffer == 0)
			throw std::invalid_argument("sInfoClient.dwSizeBuffer == 0");

		if (sInfoClient.wBufferCount == 0)
			throw std::invalid_argument("sInfoClient.wBufferCount == 0");

		if (dwCountClient == 0)
			throw std::invalid_argument("dwCountClient == 0");

		memcpy(&_sInfoClient, &sInfoClient, sizeof(SInfoClient));
		_threadUpdate = std::async(std::launch::async, &CNetworkTest::threadUpdate, this);

		for (DWORD i = 0; i < dwCountClient; i++)
		{
			_pTcpClientNetworkTest.push_back(std::make_unique<CTcpClientNetworkTest>(
				this, strIp, wPort, pIocp));
		}		
	}
	catch (const std::exception& ex)
	{
		pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
CNetworkTest::CNetworkTest(
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp)
{
	if (pIocp == nullptr)
		throw std::invalid_argument("pIocp == nullptr");

	try
	{
		_pIocp = pIocp;

		_threadUpdate = std::async(std::launch::async, &CNetworkTest::threadUpdate, this);

		_pTcpServerNetworkTest = std::make_unique<CTcpServerNetworkTest>(
			this, strIp, wPort, pIocp);		
	}
	catch (const std::exception& ex)
	{
		pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CNetworkTest::start(
	CNetworkTest::SStatistic& statistic,
	const DWORD dwTime)
{
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		if (_pTcpServerNetworkTest != nullptr)
		{
			const auto ec = _pTcpServerNetworkTest->connectServer();
			if (ec)
			{
				_pTcpServerNetworkTest->disconnectServer(ec);
				return ec;
			}		
		}

		if (!_pTcpClientNetworkTest.empty())
		{
			for (const auto& it : _pTcpClientNetworkTest)
			{
				const auto ec = it->connect();
				if (ec)
				{
					/** ошибка подключения */
					for (const auto& itDisconnect : _pTcpClientNetworkTest)
					{
						itDisconnect->disconnect(ec);
					}
					return ec;
				}
			}
		}
	}
	
	Sleep(dwTime);	
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		if (_pTcpServerNetworkTest != nullptr)
		{
			_pTcpServerNetworkTest->disconnectServer();
		}

		if (!_pTcpClientNetworkTest.empty())
		{
			for (const auto& itDisconnect : _pTcpClientNetworkTest)
			{
				itDisconnect->disconnect();
			}
		}

		memcpy(&statistic, &_statisticAll, sizeof(statistic));
	}

	return std::error_code();
}
//==============================================================================
void CNetworkTest::release(
	const bool bIsWait) noexcept
{
	__super::release(false);
	/** клиенты */
	std::list<std::unique_ptr<CTcpClientNetworkTest>> pTcpClientNetworkTest;
	/** сервер */
	std::unique_ptr<CTcpServerNetworkTest> pTcpServerNetworkTest;

	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		/** перемещаем все в другие контейнеры чтобы потом высвободить
			без участия критической секции */
		pTcpClientNetworkTest = std::move(_pTcpClientNetworkTest);
		pTcpServerNetworkTest = std::move(_pTcpServerNetworkTest);
	}

	pTcpClientNetworkTest.clear();
	pTcpServerNetworkTest.reset();

	_evThreadUpdate.notify();

	__super::release(bIsWait);
}
//==============================================================================
void CNetworkTest::connectedClient(
	INetworkTestStatistic* const pClient,
	const DWORD dwReconnect,
	const std::error_code ec) noexcept
{
	wname::cs::CCriticalSectionScoped lock(_csCounter);

	try
	{
		if (!ec)
		{
			if (_statisticClients.find(pClient) == _statisticClients.end())
			{
				_statisticClients[pClient] = SStatisticClient() = { 0 };
			}
			else
			{
				throw std::logic_error("pClient already added");
			}
		}

		connectedClientHandler(pClient, dwReconnect, ec);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::warning, ex);

		/** удаляем его на всякий случай */
		_statisticClients.erase(pClient);
	}
}
//==============================================================================
void CNetworkTest::disconnectedClient(
	INetworkTestStatistic* const pClient,
	const DWORD dwReconnect,
	const std::error_code ec) noexcept
{
	wname::cs::CCriticalSectionScoped lock(_csCounter);

	_statisticClients.erase(pClient);
	disconnectedClientHandler(pClient, dwReconnect, ec);
}
//==============================================================================
void CNetworkTest::connectedClientHandler(
	INetworkTestStatistic* const pClient,
	const DWORD dwReconnect,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(pClient);
	UNREFERENCED_PARAMETER(dwReconnect);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CNetworkTest::statisticHandler(
	const std::unordered_map<INetworkTestStatistic*, SStatisticClient>& statisticClients) noexcept
{
	UNREFERENCED_PARAMETER(statisticClients);
}
//==============================================================================
void CNetworkTest::disconnectedClientHandler(
	INetworkTestStatistic* const pClient,
	const DWORD dwReconnect,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(pClient);
	UNREFERENCED_PARAMETER(dwReconnect);
	UNREFERENCED_PARAMETER(ec);
}
//==============================================================================
void CNetworkTest::threadUpdate() noexcept
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
	{
		return;
	}

	/** промежуток опроса в 1с */
	constexpr auto dwDifTime = 1000;

	while (isInitialize() && (WAIT_TIMEOUT == _evThreadUpdate.wait(dwDifTime).value()))
	{
		try
		{
			/** обновляем статистику */
			wname::cs::CCriticalSectionScoped lock(_csCounter);
		
			for (auto& [k, v] : _statisticClients)
			{
				assert(k != nullptr);

				/** получение статистики */
				k->getStatistic(v, dwDifTime);
				_statisticAll.nAvgRecvData += v.nAvgRecvData;
				_statisticAll.nAvgSendData += v.nAvgSendData;
				_statisticAll.nAllRecvData += v.nAllRecvData;
				_statisticAll.nAllSendData += v.nAllSendData;
			}

			if (!_statisticClients.empty())
			{
				statisticHandler(_statisticClients);
			}		
		}
		catch (const std::exception& ex)
		{
			_pIocp->log(wname::logger::EMessageType::warning, ex);
		}	
	}
}
//==============================================================================
CNetworkTest::~CNetworkTest()
{
	/** ждем всего */
	release(true);
}
//==============================================================================