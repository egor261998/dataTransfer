#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** TCP ������ ��� ������������ ���� */
	class CNetworkTest::CTcpServerNetworkTest : public wname::network::CTcpServer
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
		DATATRANSFER CTcpServerNetworkTest(
			CNetworkTest* const pNetworkTest,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		DATATRANSFER void serverConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		DATATRANSFER void serverDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� TCP ������� ��� ������������.
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
		* ���������� �������� �������.
		* @return - ������.
		*/
		DATATRANSFER std::unique_ptr<CTcpConnectedClient> createClient() override;
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