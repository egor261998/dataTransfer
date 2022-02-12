#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** �������� �������� */
	class DATATRANSFER CTransferControl::ITransferMember :
		protected wname::misc::CCounter
	{
		friend class CTransferRequest;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ��������� ��������.
		* @param pTransferControl - ����������� ������.
		* @param sAccessFlag - ����� �������.
		*/
		ITransferMember(
			CTransferControl* const pTransferControl,
			const SAccessFlag& sAccessFlag);
	//==========================================================================
		/**
		* ������ ����� � ���������� �������� ��� �������� �������.
		* @param pTransferRequest - ������ �������.
		* @return - ��������� �������.
		*/
		virtual std::error_code openMember(
			CTransferRequest* const pTransferRequest) = 0;
	//==========================================================================
		/**
		* ������ � ��������� ��������.
		* @param requestInfo - ���������� �������.
		* @return - ��������� �������.
		*/
		virtual std::error_code getRequestInfo(
			CTransferRequestInfo& requestInfo) = 0;
	//==========================================================================
		/**
		* ����� �� ��������� ��������.
		* @param requestInfo - ���������� �������.
		* @return - ��������� �������.
		*/
		virtual std::error_code putRequestInfo(
			const CTransferRequestInfo& requestInfo) = 0;
	//==========================================================================
		/**
		* ������� ���� ��� ������.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� ���������� �������.
		*/
		virtual std::error_code openFile(
			CTransferFile* const pTransferFile) = 0;
	//==========================================================================
		/**
		* ������� ���� ��� ������.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� ���������� �������.
		*/
		virtual std::error_code createFile(
			CTransferFile* const pTransferFile) = 0;
	//==========================================================================
		/**
		* ����������� ������ �����.
		* @param pFileBufferRead - ����� ��������.
		* @return - ��������� ���������� �������.
		*/
		virtual std::error_code startAsyncRead(
			CTransferFileBuffer* const pFileBufferRead) = 0;
	//==========================================================================
		/**
		* ����������� ������ � ����.
		* @param pFileBufferWrite - ����� ��������.
		* @return - ��������� ���������� �������.
		*/
		virtual std::error_code startAsyncWrite(
			CTransferFileBuffer* const pFileBufferWrite) = 0;
	//==========================================================================
		/**
		* ������� ����.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� �������� �����.
		*/
		virtual std::error_code closeFile(
			CTransferFile* const pTransferFile) noexcept = 0;
	//==========================================================================
		/**
		* ��������� ����� � ���������� �������� ��� �������.
		* @param pTransferRequest - ������ �������.
		* @return - ��������� ���������� �����.
		*/
		virtual std::error_code closeMember(
			CTransferRequest* const pTransferRequest) noexcept = 0;
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
		~ITransferMember();
	//==========================================================================
		ITransferMember(const ITransferMember&) = delete;
		ITransferMember(ITransferMember&&) = delete;
		ITransferMember& operator=(const ITransferMember&) = delete;
		ITransferMember& operator=(ITransferMember&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** ����������� ������ */
		CTransferControl* const _pTransferControl;
		/** ����� ������� */
		const SAccessFlag _sAccessFlag;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END