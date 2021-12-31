#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** ������ TCP ������� ��� ����� ���� */
	class CNetworkTest::CTcpServerNetworkTest::CTcpConnectedClientNetworkTest : 
		public wname::network::CTcpConnectedClient, public INetworkTestStatistic
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ������������� �������.
		* @param pParent - ����������� ����� TCP �������.
		*/
		DATATRANSFER CTcpConnectedClientNetworkTest(
			CTcpServer* const pParent);
	//==========================================================================
		/**
		* ��������� ����������.
		* @param sStatisticClient - ����������.
		* @param dwDifTime - ���������� ������.
		*/
		DATATRANSFER void getStatistic(
			SStatisticClient& sStatisticClient,
			const DWORD dwDifTime) noexcept override;
	//==========================================================================
		/**
		* ����������.
		*/
		DATATRANSFER ~CTcpConnectedClientNetworkTest();
	//==========================================================================

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������.
		* @param bufferRecv - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		DATATRANSFER void clientAsyncRecvComplitionHandler(
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
		DATATRANSFER void clientAsyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ����������� �������.
		* @param ec - ��� ������.
		*/
		DATATRANSFER void clientConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		DATATRANSFER void clientDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		CTcpConnectedClientNetworkTest(const CTcpConnectedClientNetworkTest&) = delete;
		CTcpConnectedClientNetworkTest(CTcpConnectedClientNetworkTest&&) = delete;
		CTcpConnectedClientNetworkTest& operator=(const CTcpConnectedClientNetworkTest&) = delete;
		CTcpConnectedClientNetworkTest& operator=(CTcpConnectedClientNetworkTest&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ���������� � ���� */
		SInfoClient _sInfoClient {0};

		/** ����� ������ ������ */
		std::vector<BYTE> _bufferRecv;
		/** ����� �������� ������ */
		std::vector<BYTE> _bufferSend;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END