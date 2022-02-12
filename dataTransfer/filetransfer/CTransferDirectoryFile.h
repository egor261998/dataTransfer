#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** ���� ��������� �������� */
	class DATATRANSFER CTransferControl::CTransferDirectory::CTransferDirectoryFile final :
		public wname::filesystem::CFile
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ����� ��� �����.
		* @param pTransferFile - ������������ ����.
		* @param filePath - ���������� ���� �� �����.
		* @param pIocp - �������� �����/������.
		*/
		CTransferDirectoryFile(
			CTransferFile* const pTransferFile,
			const std::filesystem::path filePath,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ����� ������������ ������ �� �����.
		* @param pFileBufferRead - ����� ��������.
		* @return - ��� ������.
		*/
		std::error_code asyncReadFile(
			CTransferFileBuffer* const pFileBufferRead);
	//==========================================================================
		/**
		* ����� ����������� ������ � ����.
		* @param pFileBufferWrite - ����� ��������.
		* @return - ��� ������.
		*/
		std::error_code asyncWriteFile(
			CTransferFileBuffer* const pFileBufferWrite);
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������.
		* @param bufferRead - ����� ������.
		* @param dwReturnSize - ���������� ����������� ����.
		* @param ec - ��� ������ ����������.
		*/
		void asyncReadFileComplitionHandler(
			const PBYTE bufferRead,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������.
		* @param bufferWrite - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ������ ����������.
		*/
		void asyncWriteFileComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
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
		~CTransferDirectoryFile();
	//==========================================================================
		CTransferDirectoryFile(const CTransferDirectoryFile&) = delete;
		CTransferDirectoryFile(CTransferDirectoryFile&&) = delete;
		CTransferDirectoryFile& operator=(const CTransferDirectoryFile&) = delete;
		CTransferDirectoryFile& operator=(CTransferDirectoryFile&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** ������������ ���� */
		CTransferFile* const _pTransferFile;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END