#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** ������ ������� �������� */
	class DATATRANSFER CTransferControl::CTransferRequest final : 
		public wname::misc::CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� �������.
		* @param pTransferControl - ����������� ������.
		* @param eRequest - ������.
		* @param requestPath - ������������� ���� �������.
		* @param pMemberIn - ��������.
		* @param pMemberOut - ����������.
		*/
		CTransferRequest(
			CTransferControl* const pTransferControl,
			const ERequest eRequest,
			const std::filesystem::path requestPath,
			ITransferMember* const pMemberIn,
			ITransferMember* const pMemberOut);
	//==========================================================================
		/**
		* ������ ������.
		* @return - ��������� �������.
		*/
		std::error_code request();
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
		~CTransferRequest();
	//==========================================================================
		CTransferRequest(const CTransferRequest&) = delete;
		CTransferRequest(CTransferRequest&&) = delete;
		CTransferRequest& operator=(const CTransferRequest&) = delete;
		CTransferRequest& operator=(CTransferRequest&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* �������� �����.
		* @param fileInfo - ���������� � �����.
		* @return - ��������� ��������.
		*/
		std::error_code transferFile(
			const CTransferRequestInfo::SFileInfo& fileInfo);
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** ����������� ������ */
		CTransferControl* const _pTransferControl;

		/** �������� ������ */
		ITransferMember* const _pMemberIn;
		/** ���������� ������ */
		ITransferMember* const _pMemberOut;

		/** ������ ��� ��������� */
		const ERequest _eRequest;
		/** ������������� ���� ������� */
		const std::filesystem::path _requestPath;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END