#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** клиент TCP сервера для теста сети */
	class DATATRANSFER CNetworkTest::CTcpServerNetworkTest::CTcpConnectedClientNetworkTest final :
		public wname::network::CTcpConnectedClient, public INetworkTestStatistic
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор подключаемого клиента.
		* @param pParent - родительный класс TCP сервера.
		*/
		CTcpConnectedClientNetworkTest(
			CTcpServer* const pParent);
	//==========================================================================
		/**
		* получение статистики.
		* @param sStatisticClient - статистика.
		* @param dwDifTime - промежуток опроса.
		*/
		void getStatistic(
			SStatisticClient& sStatisticClient,
			const DWORD dwDifTime) noexcept override;
	//==========================================================================
		/**
		* получение адреса.
		* @return - адрес подключения.
		*/
		wname::network::socket::CSocketAddress getAddress() noexcept override;
	//==========================================================================
		/**
		* закончить работу.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		~CTcpConnectedClientNetworkTest();
	//==========================================================================

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* обработчик события завершения асинхронного чтения.
		* @param bufferRecv - буфер данных.
		* @param dwReturnSize - количество полученных байт.
		* @param ec - код завершения.
		*/
		void clientAsyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи.
		* @param bufferSend - буфер данных.
		* @param dwReturnSize - количество переданных байт.
		* @param ec - код завершения.
		*/
		void clientAsyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события подключения клиента.
		* @param ec - код ошибки.
		*/
		void clientConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события отключения клиента.
		* @param ec - код ошибки.
		*/
		void clientDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		CTcpConnectedClientNetworkTest(const CTcpConnectedClientNetworkTest&) = delete;
		CTcpConnectedClientNetworkTest(CTcpConnectedClientNetworkTest&&) = delete;
		CTcpConnectedClientNetworkTest& operator=(const CTcpConnectedClientNetworkTest&) = delete;
		CTcpConnectedClientNetworkTest& operator=(CTcpConnectedClientNetworkTest&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** информация о сети */
		SInfoClient _sInfoClient {0};

		/** буфер приема данных */
		std::list<std::vector<BYTE>> _listBufferRecv;
		/** буфер отправки данных */
		std::list<std::vector<BYTE>> _listBufferSend;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END