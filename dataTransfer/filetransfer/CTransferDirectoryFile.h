#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** файл участника передачи */
	class DATATRANSFER CTransferControl::CTransferDirectory::CTransferDirectoryFile final :
		public wname::filesystem::CFile
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор файла для папки.
		* @param pTransferFile - передаваемый файл.
		* @param filePath - абсолютный путь до файла.
		* @param pIocp - механизм ввода/вывода.
		*/
		CTransferDirectoryFile(
			CTransferFile* const pTransferFile,
			const std::filesystem::path filePath,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* старт асинхронного чтения из файла.
		* @param pFileBufferRead - буфер передачи.
		* @return - код ошибки.
		*/
		std::error_code asyncReadFile(
			CTransferFileBuffer* const pFileBufferRead);
	//==========================================================================
		/**
		* старт асинхронной записи в файл.
		* @param pFileBufferWrite - буфер передачи.
		* @return - код ошибки.
		*/
		std::error_code asyncWriteFile(
			CTransferFileBuffer* const pFileBufferWrite);
	//==========================================================================
		/**
		* обработчик события завершения асинхронного чтения.
		* @param bufferRead - буфер данных.
		* @param dwReturnSize - количество прочитанных байт.
		* @param ec - код ошибки завершения.
		*/
		void asyncReadFileComplitionHandler(
			const PBYTE bufferRead,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи.
		* @param bufferWrite - буфер данных.
		* @param dwReturnSize - количество записанных байт.
		* @param ec - код ошибки завершения.
		*/
		void asyncWriteFileComplitionHandler(
			const PBYTE bufferWrite,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept override;
	//==========================================================================
		/**
		* закончить работу.
		* @param bIsWait - признак ожидания.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* деструктор.
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
		/** передаваемый файл */
		CTransferFile* const _pTransferFile;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END