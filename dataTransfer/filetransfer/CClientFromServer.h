#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** TCP ������ */
	class DATATRANSFER CTransferControl::CTransferTcpServer::CClientFromServer final :
	public wname::network::CTcpConnectedClient,
		public ITransferTcpCommunication<wname::network::CTcpConnectedClient>
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� TCP �������.
		* @param pParent - ����������� ����� TCP �������.
		*/
		CClientFromServer(
			wname::network::CTcpServer* const pParent);
	//==========================================================================
		/**
		* ��������� ������.
		* @param bIsWait - ������� ��������.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* ����������.
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
		* ���������� ������� ����������� �������.
		* @param ec - ��� ������.
		*/
		void clientConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������.
		* @param bufferRecv - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		void clientAsyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������.
		* @param bufferSend - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
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