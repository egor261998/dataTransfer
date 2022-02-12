#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** ���������� ������ ��� �������� ������
		[��������]<->[����������] */
	class DATATRANSFER CTransferControl::CTransferDirectory final : 
		public ITransferMember
	{
	#pragma region Private_Inner
	private:
	//==========================================================================
		/** ���� ��������� �������� */
		class CTransferDirectoryFile;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ��������� �������� ��� �����.
		* @param pTransferControl - ����������� ������.
		* @param sAccessFlag - ����� �������.
		* @param directoryPath - ���������� ���� �� ����� ���������.
		*/
		CTransferDirectory(
			CTransferControl* const pTransferControl,
			const SAccessFlag& sAccessFlag,
			const std::filesystem::path directoryPath);
	//==========================================================================
		/**
		* ������ ����� � ���������� �������� ��� �������� �������.
		* @param pTransferRequest - ������ �������.
		* @return - ��������� �������.
		*/
		std::error_code openMember(
			CTransferRequest* const pTransferRequest) override;
	//==========================================================================
		/**
		* ������ � ��������� ��������.
		* @param requestInfo - ���������� �������.
		* @return - ��������� �������.
		*/
		std::error_code getRequestInfo(
			CTransferRequestInfo& requestInfo) override;
	//==========================================================================
		/**
		* ����� �� ��������� ��������.
		* @param requestInfo - ���������� �������.
		* @return - ��������� �������.
		*/
		std::error_code putRequestInfo(
			const CTransferRequestInfo& requestInfo) noexcept override;
	//==========================================================================
		/**
		* ������� ���� ��� ������.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code openFile(
			CTransferFile* const pTransferFile) override;
	//==========================================================================
		/**
		* ������� ���� ��� ������.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code createFile(
			CTransferFile* const pTransferFile) override;
	//==========================================================================
		/**
		* ����������� ������ �����.
		* @param pFileBufferRead - ����� ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code startAsyncRead(
			CTransferFileBuffer* const pFileBufferRead) override;
	//==========================================================================
		/**
		* ����������� ������ � ����.
		* @param pFileBufferWrite - ����� ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code startAsyncWrite(
			CTransferFileBuffer* const pFileBufferWrite) override;
	//==========================================================================
		/**
		* ������� ����.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� �������� �����.
		*/
		std::error_code closeFile(
			CTransferFile* const pTransferFile) noexcept override;
	//==========================================================================
		/**
		* ��������� ����� � ���������� �������� ��� �������.
		* @param pTransferRequest - ������ �������.
		* @return - ��������� ���������� �����.
		*/
		std::error_code closeMember(
			CTransferRequest* const pTransferRequest) noexcept override;
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
		~CTransferDirectory();
	//==========================================================================
		CTransferDirectory(const CTransferDirectory&) = delete;
		CTransferDirectory(CTransferDirectory&&) = delete;
		CTransferDirectory& operator=(const CTransferDirectory&) = delete;
		CTransferDirectory& operator=(CTransferDirectory&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** ���� �� ����� */
		std::filesystem::path _directoryPath;

		/** ������ ������������ ������ */
		using FileList = std::unordered_map<
			CTransferFile*,
			std::shared_ptr<CTransferDirectoryFile>>;

		/** �������� �������� */
		std::unordered_map<CTransferRequest*, FileList> _transferActive;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END