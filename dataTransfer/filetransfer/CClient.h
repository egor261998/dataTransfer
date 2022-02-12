#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** TCP клиент */
	class DATATRANSFER CTransferControl::CTransferTcpClient::CClient final :
		public wname::network::CTcpClient,
		public ITransferTcpCommunication<wname::network::CTcpClient>
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор TCP клиента.
		* @param pTransferTcpClient - родительный объект.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		CClient(
			CTransferTcpClient* const pTransferTcpClient,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
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
		~CClient();
	//==========================================================================
		CClient(const CClient&) = delete;
		CClient(CClient&&) = delete;
		CClient& operator=(const CClient&) = delete;
		CClient& operator=(CClient&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
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
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** родительный объект */
		CTransferTcpClient* const _pTransferTcpClient;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END