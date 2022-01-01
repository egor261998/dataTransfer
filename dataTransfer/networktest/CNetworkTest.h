#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** ������������ ���� */
	class CNetworkTest : protected wname::misc::CCounter
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** ���������� � ���� ������� */
		#pragma pack(1)
		typedef struct _SInfoClient
		{
			/** ������ ������ */
			DWORD dwSizeBuffer;
			/** ���������� ����������� */
			DWORD dwBandWidth;
			/** ���������� ������� �������� */
			WORD wBufferCount;

			/** ������� ������ ������ */
			bool bIsRecv;
			/** ������� �������� ������ */
			bool bIsSend;
		}SInfoClient, * PSInfoClient;
		#pragma pack()
	//==========================================================================
		/** ���������� ������� */
		typedef struct _SStatisticClient
		{	
			/** ��������� ����� ������� */
			UINT64 tickCount;
			/** ���������� ���������� ������ */
			UINT64 nRecvData;
			UINT64 nSendData;
			UINT64 nAvgRecvData;
			UINT64 nAvgSendData;
			UINT64 nAllRecvData;
			UINT64 nAllSendData;

			/** ���������� � ���� ������� */
			SInfoClient info;
		}SStatisticClient, * PSStatisticClient;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Inner
	protected:
	//==========================================================================
		/** ����� ������������ ����*/
		class INetworkTestStatistic;
		class CTcpServerNetworkTest;
		class CTcpClientNetworkTest;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ������������ ���� ��� ������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param dwCountClient - ���������� ��������.
		* @param sInfoClient - ��������� ������������ ����.
		* @param pIocp - �������� Iocp.
		*/
		DATATRANSFER CNetworkTest(
			const std::string strIp,
			const WORD wPort,
			const DWORD dwCountClient,
			const SInfoClient& sInfoClient,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ����������� ������������ ���� ��� ������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		DATATRANSFER CNetworkTest(
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ���������� ������������ ����.
		*/
		DATATRANSFER ~CNetworkTest();
	//==========================================================================
		CNetworkTest(const CNetworkTest&) = delete;
		CNetworkTest(CNetworkTest&&) = delete;
		CNetworkTest& operator=(const CNetworkTest&) = delete;
		CNetworkTest& operator=(CNetworkTest&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* ����������� ����������� � ����������� �������.
		* @param pClient - ������.
		*/
		DATATRANSFER virtual void connectedClientHandler(
			INetworkTestStatistic* const pClient) noexcept;
	//==========================================================================
		/**
		* ����������� ����������� �� ���� ����� ����������.
		* @param statisticClients - ����������.
		*/
		DATATRANSFER virtual void statisticHandler(
			const std::unordered_map<INetworkTestStatistic*, SStatisticClient> statisticClients) noexcept;
	//==========================================================================
		/**
		* ����������� ����������� �� ���������� �������.
		* @param pClient - ������.
		*/
		DATATRANSFER virtual void disconnectedClientHandler(
			INetworkTestStatistic* const pClient) noexcept;
	//==========================================================================
		/**
		* ����������� �������.
		* @param pClient - ������.
		*/
		DATATRANSFER void connectedClient(
			INetworkTestStatistic* const pClient) noexcept;
	//==========================================================================
		/**
		* ���������� �������.
		* @param pClient - ������.
		*/
		DATATRANSFER void disconnectedClient(
			INetworkTestStatistic* const pClient) noexcept;
	//==========================================================================
		/**
		* ���������� ����������.
		*/
		DATATRANSFER void threadUpdate() noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** �������� �����/������ */
		std::shared_ptr<wname::io::iocp::CIocp> _pIocp;

		/** ���������� � ������� */
		SInfoClient _sInfoClient {0};
		/** ������� */
		std::list<std::unique_ptr<CTcpClientNetworkTest>> _pTcpClientNetworkTest;

		/** ������ */
		std::unique_ptr<CTcpServerNetworkTest> _pTcpServerNetworkTest;

		/** ���� ���������� */
		std::future<void> _threadUpdate;
		wname::handle::CEvent _evThreadUpdate;
		std::unordered_map<INetworkTestStatistic*, SStatisticClient> _statisticClients;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END