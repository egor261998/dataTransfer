#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** TCP сервер дл€ тестировани€ сети */
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
		* конструктор TCP сервера дл€ тестировани€ сети.
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
		* обработчик событи€ отключени€ сервера.
		* @param ec - код ошибки.
		*/
		DATATRANSFER void serverConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик событи€ отключени€ сервера.
		* @param ec - код ошибки.
		*/
		DATATRANSFER void serverDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* деструктор TCP сервера дл€ тестировани€.
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
		* обработчик создани€ клиента.
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
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END