#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** тестирование сети */
	class CNetworkTest : protected wname::misc::CCounter
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** информация о сети клиента */
		#pragma pack(1)
		typedef struct _SInfoClient
		{
			/** размер буфера */
			DWORD dwSizeBuffer;

			/** пропускная способность */
			DWORD dwBandwidth;
			/** признак приема данных */
			bool bIsRecv;
			/** признак отправки данных */
			bool bIsSend;
		}SInfoClient, * PSInfoClient;
		#pragma pack()
	//==========================================================================
		/** статистика клиента */
		#pragma pack(1)
		typedef struct _SStatisticClient
		{	
			/** начальная точка отсчета */
			UINT64 tickCount;
			/** статистика переданных данных */
			UINT64 nRecvData;
			UINT64 nSendData;
			UINT64 nAvgRecvData;
			UINT64 nAvgSendData;
			UINT64 nAllRecvData;
			UINT64 nAllSendData;

			/** информация о сети клиента */
			SInfoClient info;
		}SStatisticClient, * PSStatisticClient;
		#pragma pack()
	//==========================================================================
	#pragma endregion

	#pragma region Private_Inner
	private:
	//==========================================================================
		/** класс тестирования сети*/
		class INetworkTestStatistic;
		class CTcpServerNetworkTest;
		class CTcpClientNetworkTest;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор тестирования сети как клиент.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param dwCountClient - количество клиентов.
		* @param sInfoClient - параметры тестирования сети.
		* @param pIocp - механизм Iocp.
		*/
		DATATRANSFER CNetworkTest(
			const std::string strIp,
			const WORD wPort,
			const DWORD dwCountClient,
			const SInfoClient& sInfoClient,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* конструктор тестирования сети как сервер.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		DATATRANSFER CNetworkTest(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* деструктор тестирования сети.
		*/
		DATATRANSFER ~CNetworkTest();
	//==========================================================================
		CNetworkTest(const CNetworkTest&) = delete;
		CNetworkTest(CNetworkTest&&) = delete;
		CNetworkTest& operator=(const CNetworkTest&) = delete;
		CNetworkTest& operator=(CNetworkTest&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* виртуальное уведомление о подключение клиента.
		* @param pClient - клиент.
		*/
		DATATRANSFER virtual void connectedClientHandler(
			INetworkTestStatistic* const pClient) noexcept;
	//==========================================================================
		/**
		* виртуальное уведомление об отключении клиента.
		* @param pClient - клиент.
		*/
		DATATRANSFER virtual void disconnectedClientHandler(
			INetworkTestStatistic* const pClient) noexcept;
	//==========================================================================
		/**
		* подключение клиента.
		* @param pClient - клиент.
		*/
		DATATRANSFER void connectedClient(
			INetworkTestStatistic* const pClient) noexcept;
	//==========================================================================
		/**
		* отключение клиента.
		* @param pClient - клиент.
		*/
		DATATRANSFER void disconnectedClient(
			INetworkTestStatistic* const pClient) noexcept;
	//==========================================================================
		/**
		* обновление статистики.
		*/
		DATATRANSFER void threadUpdate() noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** механизм ввода/вывода */
		std::shared_ptr<wname::io::iocp::CIocp> _pIocp;

		/** информация о клиенте */
		SInfoClient _sInfoClient {0};
		/** клиенты */
		std::list<std::unique_ptr<CTcpClientNetworkTest>> _pTcpClientNetworkTest;

		/** сервер */
		std::unique_ptr<CTcpServerNetworkTest> _pTcpServerNetworkTest;

		/** сбор статистики */
		std::future<void> _threadUpdate;
		wname::handle::CEvent _evThreadUpdate;
		std::unordered_map<INetworkTestStatistic*, SStatisticClient> _statisticClients;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END