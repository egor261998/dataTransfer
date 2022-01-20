#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** интерфейс получения статистики */
	class DATATRANSFER CNetworkTest::INetworkTestStatistic
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* получение статистики.
		* @param sStatisticClient - статистика.
		* @param dwDifTime - промежуток опроса.
		*/
		virtual void getStatistic(
			CNetworkTest::SStatisticClient& sStatisticClient,
			const DWORD dwDifTime) noexcept = 0;
	//==========================================================================
		/**
		* получение адреса.
		* @return - адрес подключения.
		*/
		virtual wname::network::socket::CSocketAddress getAddress() noexcept = 0;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END