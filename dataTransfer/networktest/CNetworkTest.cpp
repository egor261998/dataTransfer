#include "../stdafx.h"

using CNetworkTestPrefix = datatransfer::networktest::CNetworkTest;

//==============================================================================
CNetworkTestPrefix::CNetworkTest(
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

		if (sInfoClient.dwSizeBuffer == 0)
			throw std::invalid_argument("sInfoClient.dwSizeBuffer == 0");

		if (dwCountClient == 0)
			throw std::invalid_argument("dwCountClient == 0");

		memcpy(&_sInfoClient, &sInfoClient, sizeof(SInfoClient));

		for (DWORD i = 0; i < dwCountClient; i++)
		{
			_pTcpClientNetworkTest.push_back(std::make_unique<CTcpClientNetworkTest>(
				this, strIp, wPort, pIocp));
		}

		initialize();
		_evThreadUpdate.initialize();
		_threadUpdate = std::async(std::launch::async, &CNetworkTest::threadUpdate, this);
	}
	catch (const std::exception& ex)
	{
		pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
CNetworkTestPrefix::CNetworkTest(
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp)
{
	if (pIocp == nullptr)
		throw std::invalid_argument("pIocp == nullptr");

	try
	{
		_pIocp = pIocp;

		_pTcpServerNetworkTest = std::make_unique<CTcpServerNetworkTest>(
			this, strIp, wPort, pIocp);

		initialize();
		_evThreadUpdate.initialize();
		_threadUpdate = std::async(std::launch::async, &CNetworkTest::threadUpdate, this);
	}
	catch (const std::exception& ex)
	{
		pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CNetworkTestPrefix::connectedClient(
	INetworkTestStatistic* const pClient) noexcept
{
	wname::cs::CCriticalSectionScoped lock(_csCounter);

	try
	{
		if (_statisticClients.find(pClient) == _statisticClients.end())
		{
			_statisticClients[pClient] = SStatisticClient() = { 0 };
		}
		else
		{
			throw std::logic_error("pClient already added");
		}

		connectedClientHandler(pClient);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::warning, ex);

		/** удаляем его на всякий случай */
		_statisticClients.erase(pClient);
	}
}
//==============================================================================
void CNetworkTestPrefix::disconnectedClient(
	INetworkTestStatistic* const pClient) noexcept
{
	wname::cs::CCriticalSectionScoped lock(_csCounter);

	_statisticClients.erase(pClient);
	disconnectedClientHandler(pClient);
}
//==============================================================================
void CNetworkTestPrefix::connectedClientHandler(
	INetworkTestStatistic* const pClient) noexcept
{
	UNREFERENCED_PARAMETER(pClient);
}
//==============================================================================
void CNetworkTestPrefix::disconnectedClientHandler(
	INetworkTestStatistic* const pClient) noexcept
{
	UNREFERENCED_PARAMETER(pClient);
}
//==============================================================================
void CNetworkTestPrefix::threadUpdate() noexcept
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
				/**  */
				k->getStatistic(v, dwDifTime);
			}
		}
		catch (const std::exception& ex)
		{
			_pIocp->log(wname::logger::EMessageType::warning, ex);
		}	
	}
}
//==============================================================================
CNetworkTestPrefix::~CNetworkTest()
{
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

	/** ждем всего */
	release();
}
//==============================================================================