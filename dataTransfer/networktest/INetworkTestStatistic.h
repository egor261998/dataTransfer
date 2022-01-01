#pragma once

_DATATRANSFER_BEGIN
namespace networktest
{
	/** интерфейс получения статистики */
	class CNetworkTest::INetworkTestStatistic
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* получение статистики.
		* @param sStatisticClient - статистика.
		* @param dwDifTime - промежуток опроса.
		*/
		DATATRANSFER virtual void getStatistic(
			CNetworkTest::SStatisticClient& sStatisticClient,
			const DWORD dwDifTime) noexcept = 0;
	//==========================================================================
		/**
		* получение адреса.
		* @return - адрес подключения.
		*/
		DATATRANSFER virtual const wname::network::socket::CSocketAddress& getAddress() noexcept = 0;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END