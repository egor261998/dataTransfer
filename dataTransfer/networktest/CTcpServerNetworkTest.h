#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** TCP ������ ��� ������������ ���� */
	class DATATRANSFER CNetworkTest::CTcpServerNetworkTest final :
		public wname::network::CTcpServer
	{
		friend class CNetworkTest;
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** ����� ������������� ������� � TCP ������� */
		class CTcpConnectedClientNetworkTest;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� TCP ������� ��� ������������ ����.
		* @param pNetworkTest - ����������� �����.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		CTcpServerNetworkTest(
			CNetworkTest* const pNetworkTest,
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
		* ���������� TCP ������� ��� ������������.
		*/
		~CTcpServerNetworkTest();
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
		CNetworkTest* const _pNetworkTest;

		/** ���������� ��������������� */
		std::atomic_uint _dwRecconect = 0;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END