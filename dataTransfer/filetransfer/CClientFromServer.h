#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** TCP клиент */
	class DATATRANSFER CTransferControl::CTransferTcpServer::CClientFromServer final :
	public wname::network::CTcpConnectedClient,
		public ITransferTcpCommunication<wname::network::CTcpConnectedClient>
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор TCP клиента.
		* @param pParent - родительный класс TCP сервера.
		*/
		CClientFromServer(
			wname::network::CTcpServer* const pParent);
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
		~CClientFromServer();
	//==========================================================================
		CClientFromServer(const CClientFromServer&) = delete;
		CClientFromServer(CClientFromServer&&) = delete;
		CClientFromServer& operator=(const CClientFromServer&) = delete;
		CClientFromServer& operator=(CClientFromServer&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* обработчик события подключения клиента.
		* @param ec - код ошибки.
		*/
		void clientConnected(
			const std::error_code ec) noexcept override;
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
	};
}
_DATATRANSFER_END