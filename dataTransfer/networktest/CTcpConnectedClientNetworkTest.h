#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** ������ TCP ������� ��� ����� ���� */
	class DATATRANSFER CNetworkTest::CTcpServerNetworkTest::CTcpConnectedClientNetworkTest final :
		public wname::network::CTcpConnectedClient, public INetworkTestStatistic
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ������������� �������.
		* @param pParent - ����������� ����� TCP �������.
		*/
		CTcpConnectedClientNetworkTest(
			CTcpServer* const pParent);
	//==========================================================================
		/**
		* ��������� ����������.
		* @param sStatisticClient - ����������.
		* @param dwDifTime - ���������� ������.
		*/
		void getStatistic(
			SStatisticClient& sStatisticClient,
			const DWORD dwDifTime) noexcept override;
	//==========================================================================
		/**
		* ��������� ������.
		* @return - ����� �����������.
		*/
		wname::network::socket::CSocketAddress getAddress() noexcept override;
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
		~CTcpConnectedClientNetworkTest();
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
		/**
		* ���������� ������� ����������� �������.
		* @param ec - ��� ������.
		*/
		void clientConnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		void clientDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
		CTcpConnectedClientNetworkTest(const CTcpConnectedClientNetworkTest&) = delete;
		CTcpConnectedClientNetworkTest(CTcpConnectedClientNetworkTest&&) = delete;
		CTcpConnectedClientNetworkTest& operator=(const CTcpConnectedClientNetworkTest&) = delete;
		CTcpConnectedClientNetworkTest& operator=(CTcpConnectedClientNetworkTest&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** ���������� � ���� */
		SInfoClient _sInfoClient {0};

		/** ����� ������ ������ */
		std::list<std::vector<BYTE>> _listBufferRecv;
		/** ����� �������� ������ */
		std::list<std::vector<BYTE>> _listBufferSend;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END