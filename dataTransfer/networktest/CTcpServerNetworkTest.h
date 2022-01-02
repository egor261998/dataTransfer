#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** TCP сервер для тестирования сети */
	class CNetworkTest::CTcpServerNetworkTest : public wname::network::CTcpServer
	{
		friend class CNetworkTest;
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** класс подключенного клиента к TCP серверу */
		class CTcpConnectedClientNetworkTest;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор TCP сервера для тестирования сети.
		* @param pNetworkTest - родительный класс.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		DATATRANSFER CTcpServerNetworkTest(
			CNetworkTest* const pNetworkTest,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* обработчик события отключения сервера.
		* @param ec - код ошибки.
		*/
		DATATRANSFER void serverConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события отключения сервера.
		* @param ec - код ошибки.
		*/
		DATATRANSFER void serverDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* деструктор TCP сервера для тестирования.
		*/
		DATATRANSFER ~CTcpServerNetworkTest();
	//==========================================================================
		CTcpServerNetworkTest(const CTcpServerNetworkTest&) = delete;
		CTcpServerNetworkTest(CTcpServerNetworkTest&&) = delete;
		CTcpServerNetworkTest& operator=(const CTcpServerNetworkTest&) = delete;
		CTcpServerNetworkTest& operator=(CTcpServerNetworkTest&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* обработчик создания клиента.
		* @return - клиент.
		*/
		DATATRANSFER std::unique_ptr<CTcpConnectedClient> createClient() override;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** родительный класс */
		CNetworkTest* const _pNetworkTest;

		/** количество переподключений */
		std::atomic_uint _dwRecconect = 0;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END