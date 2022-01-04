#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** TCP клиент для тестирования сети */
	class CNetworkTest::CTcpClientNetworkTest : 
		public wname::network::CTcpClient, public INetworkTestStatistic
	{
	#pragma region Public_Method
	public:
		//==========================================================================
		/**
		* конструктор TCP клиента.
		* @param pNetworkTest - родительный класс.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		DATATRANSFER CTcpClientNetworkTest(
			CNetworkTest* const pNetworkTest,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
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
		* получение адреса.
		* @return - адрес подключения.
		*/
		DATATRANSFER wname::network::socket::CSocketAddress getAddress() noexcept override;
	//==========================================================================
		/**
		* закончить работу.
		*/
		WNAME void release() noexcept override;
	//==========================================================================
		/**
		* деструктор.
		*/
		DATATRANSFER ~CTcpClientNetworkTest();
	//==========================================================================
		CTcpClientNetworkTest(const CTcpClientNetworkTest&) = delete;
		CTcpClientNetworkTest(CTcpClientNetworkTest&&) = delete;
		CTcpClientNetworkTest& operator=(const CTcpClientNetworkTest&) = delete;
		CTcpClientNetworkTest& operator=(CTcpClientNetworkTest&&) = delete;
	//==========================================================================
	#pragma endregion

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
		* виртуальный обработчик события отключения клиента.
		* @param ec - код ошибки.
		*/
		DATATRANSFER void clientDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** родительный класс */
		CNetworkTest* const _pNetworkTest;

		/** буфер приема данных */
		std::list<std::vector<BYTE>> _listBufferRecv;
		/** буфер отправки данных */
		std::list<std::vector<BYTE>> _listBufferSend;

		/** количество переподключений */
		std::atomic_uint _dwReconnect = 0;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END