#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** TCP ������ ��� ������������ ���� */
	class DATATRANSFER CTransferControl::CTransferTcpServer::CTcpServerFromMember final :
		public wname::network::CTcpServer
	{
		friend class CClientFromServer;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� TCP ������� ���.
		* @param pTransferTcpServer - ����������� �����.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		CTcpServerFromMember(
			CTransferTcpServer* const pTransferTcpServer,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		void serverConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		void serverDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ��������� ������.
		* @param bIsWait - ������� ��������.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* ���������� TCP �������.
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
		* ���������� �������� �������.
		* @return - ������.
		*/
		std::unique_ptr<CTcpConnectedClient> createClient() override;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ����������� ����� */
		CTransferTcpServer* const _pTransferTcpServer;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END