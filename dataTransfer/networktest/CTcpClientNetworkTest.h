#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** TCP ������ ��� ������������ ���� */
	class DATATRANSFER CNetworkTest::CTcpClientNetworkTest final :
		public wname::network::CTcpClient, public INetworkTestStatistic
	{
	#pragma region Public_Method
	public:
		//==========================================================================
		/**
		* ����������� TCP �������.
		* @param pNetworkTest - ����������� �����.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		CTcpClientNetworkTest(
			CNetworkTest* const pNetworkTest,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
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
		~CTcpClientNetworkTest();
	//==========================================================================
		CTcpClientNetworkTest(const CTcpClientNetworkTest&) = delete;
		CTcpClientNetworkTest(CTcpClientNetworkTest&&) = delete;
		CTcpClientNetworkTest& operator=(const CTcpClientNetworkTest&) = delete;
		CTcpClientNetworkTest& operator=(CTcpClientNetworkTest&&) = delete;
	//==========================================================================
	#pragma endregion

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
		* ����������� ���������� ������� ���������� �������.
		* @param ec - ��� ������.
		*/
		void clientDisconnected(
			const std::error_code ec) noexcept override;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ����������� ����� */
		CNetworkTest* const _pNetworkTest;

		/** ����� ������ ������ */
		std::list<std::vector<BYTE>> _listBufferRecv;
		/** ����� �������� ������ */
		std::list<std::vector<BYTE>> _listBufferSend;

		/** ���������� ��������������� */
		std::atomic_uint _dwReconnect = 0;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END