#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** TCP сервер для тестирования сети */
	class DATATRANSFER CTransferControl::CTransferTcpServer::CTcpServerFromMember final :
		public wname::network::CTcpServer
	{
		friend class CClientFromServer;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор TCP сервера для.
		* @param pTransferTcpServer - родительный класс.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		CTcpServerFromMember(
			CTransferTcpServer* const pTransferTcpServer,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* обработчик события отключения сервера.
		* @param ec - код ошибки.
		*/
		void serverConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события отключения сервера.
		* @param ec - код ошибки.
		*/
		void serverDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* закончить работу.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* деструктор TCP сервера.
		*/
		~CTcpServerFromMember();
	//==========================================================================
		CTcpServerFromMember(const CTcpServerFromMember&) = delete;
		CTcpServerFromMember(CTcpServerFromMember&&) = delete;
		CTcpServerFromMember& operator=(const CTcpServerFromMember&) = delete;
		CTcpServerFromMember& operator=(CTcpServerFromMember&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* обработчик создания клиента.
		* @return - клиент.
		*/
		std::unique_ptr<CTcpConnectedClient> createClient() override;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** родительный класс */
		CTransferTcpServer* const _pTransferTcpServer;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END