#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** клиент TCP сервера для теста сети */
	class CNetworkTest::CTcpServerNetworkTest::CTcpConnectedClientNetworkTest : 
		public wname::network::CTcpConnectedClient, public INetworkTestStatistic
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор подключаемого клиента.
		* @param pParent - родительный класс TCP сервера.
		*/
		DATATRANSFER CTcpConnectedClientNetworkTest(
			CTcpServer* const pParent);
	//==========================================================================
		/**
		* получение статистики.
		* @param sStatisticClient - статистика.
		* @param dwDifTime - промежуток опроса.
		*/
		DATATRANSFER void getStatistic(
			SStatisticClient& sStatisticClient,
			const DWORD dwDifTime) noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		DATATRANSFER ~CTcpConnectedClientNetworkTest();
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
		DATATRANSFER void clientAsyncRecvComplitionHandler(
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
		DATATRANSFER void clientAsyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события подключения клиента.
		* @param ec - код ошибки.
		*/
		DATATRANSFER void clientConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события отключения клиента.
		* @param ec - код ошибки.
		*/
		DATATRANSFER void clientDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		CTcpConnectedClientNetworkTest(const CTcpConnectedClientNetworkTest&) = delete;
		CTcpConnectedClientNetworkTest(CTcpConnectedClientNetworkTest&&) = delete;
		CTcpConnectedClientNetworkTest& operator=(const CTcpConnectedClientNetworkTest&) = delete;
		CTcpConnectedClientNetworkTest& operator=(CTcpConnectedClientNetworkTest&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** информация о сети */
		SInfoClient _sInfoClient {0};

		/** буфер приема данных */
		std::vector<BYTE> _bufferRecv;
		/** буфер отправки данных */
		std::vector<BYTE> _bufferSend;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END