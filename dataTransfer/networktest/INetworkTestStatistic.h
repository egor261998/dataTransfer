#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** ��������� ��������� ���������� */
	class DATATRANSFER CNetworkTest::INetworkTestStatistic
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ��������� ����������.
		* @param sStatisticClient - ����������.
		* @param dwDifTime - ���������� ������.
		*/
		virtual void getStatistic(
			CNetworkTest::SStatisticClient& sStatisticClient,
			const DWORD dwDifTime) noexcept = 0;
	//==========================================================================
		/**
		* ��������� ������.
		* @return - ����� �����������.
		*/
		virtual wname::network::socket::CSocketAddress getAddress() noexcept = 0;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END