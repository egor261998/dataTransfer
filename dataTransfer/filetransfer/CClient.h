#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** TCP ������ */
	class DATATRANSFER CTransferControl::CTransferTcpClient::CClient final :
		public wname::network::CTcpClient,
		public ITransferTcpCommunication<wname::network::CTcpClient>
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� TCP �������.
		* @param pTransferTcpClient - ����������� ������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		CClient(
			CTransferTcpClient* const pTransferTcpClient,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
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

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ����������� ������ */
		CTransferTcpClient* const _pTransferTcpClient;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END