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
			DWORD dwBandWidth;
			/** количество буфером передачи */
			WORD wBufferCount;

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

			/** переподключение */
			DWORD dwRecconect;

			/** информация о сети клиента */
			SInfoClient info;
		}SStatisticClient, * PSStatisticClient;
		#pragma pack()
	//==========================================================================
		/** статистика общая */
		#pragma pack(1)
		typedef struct _SStatistic
		{
			/** статистика переданных данных */
			UINT64 nAvgRecvData;
			UINT64 nAvgSendData;
			UINT64 nAllRecvData;
			UINT64 nAllSendData;
		}SStatistic, * PSStatistic;
		#pragma pack()
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Inner
	protected:
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
		* начать тестирование сети.
		* @param statistic - статистика.
		* @param dwTime - время тестирования в милисекундах.
		* @return - результата работы.
		*/
		DATATRANSFER std::error_code start(
			SStatistic& statistic,
			const DWORD dwTime = INFINITE);
	//==========================================================================
		/**
		* закончить работу и дождаться всех операций.
		*/
		DATATRANSFER void release() noexcept override;
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

	#pragma region Protected_Method
	private:
	//==========================================================================
		/**
		* виртуальное уведомление о подключение клиента.
		* @param pClient - клиент.
		* @param dwReconnect - количество переподключений.
		* @param ec - ошибка подключения.
		*/
		DATATRANSFER virtual void connectedClientHandler(
			INetworkTestStatistic* const pClient,
			const DWORD dwReconnect,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* виртуальное уведомление об тике сбора статистики.
		* @param statisticClients - статистика.
		*/
		DATATRANSFER virtual void statisticHandler(
			const std::unordered_map<INetworkTestStatistic*, SStatisticClient>& statisticClients) noexcept;
	//==========================================================================
		/**
		* виртуальное уведомление об отключении клиента.
		* @param pClient - клиент.
		* @param dwReconnect - количество переподключений.
		* @param ec - ошибка отключения.
		*/
		DATATRANSFER virtual void disconnectedClientHandler(
			INetworkTestStatistic* const pClient,
			const DWORD dwReconnect,
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* подключение клиента.
		* @param pClient - клиент.
		* @param dwReconnect - количество переподключений.
		* @param ec - ошибка подключения.
		*/
		DATATRANSFER void connectedClient(
			INetworkTestStatistic* const pClient,
			const DWORD dwReconnect,
			const std::error_code ec = std::error_code()) noexcept;
	//==========================================================================
		/**
		* отключение клиента.
		* @param pClient - клиент.
		* @param dwReconnect - количество переподключений.
		* @param ec - ошибка отключения.
		*/
		DATATRANSFER void disconnectedClient(
			INetworkTestStatistic* const pClient,
			const DWORD dwReconnect,
			const std::error_code ec = std::error_code()) noexcept;
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
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
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
		SStatistic _statisticAll;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END