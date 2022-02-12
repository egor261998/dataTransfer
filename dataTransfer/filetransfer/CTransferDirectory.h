#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** получатель файлов или источник файлов
		[источник]<->[получатель] */
	class DATATRANSFER CTransferControl::CTransferDirectory final : 
		public ITransferMember
	{
	#pragma region Private_Inner
	private:
	//==========================================================================
		/** файл участника передачи */
		class CTransferDirectoryFile;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор участника передачи для папки.
		* @param pTransferControl - родительный объект.
		* @param sAccessFlag - флаги доступа.
		* @param directoryPath - абсолютный путь до папки источника.
		*/
		CTransferDirectory(
			CTransferControl* const pTransferControl,
			const SAccessFlag& sAccessFlag,
			const std::filesystem::path directoryPath);
	//==========================================================================
		/**
		* начало работ с участником передачи для текущего запроса.
		* @param pTransferRequest - объект запроса.
		* @return - результат запроса.
		*/
		std::error_code openMember(
			CTransferRequest* const pTransferRequest) override;
	//==========================================================================
		/**
		* запрос у участника передачи.
		* @param requestInfo - информация запроса.
		* @return - результат запроса.
		*/
		std::error_code getRequestInfo(
			CTransferRequestInfo& requestInfo) override;
	//==========================================================================
		/**
		* ответ от участнику передачи.
		* @param requestInfo - информация запроса.
		* @return - результат запроса.
		*/
		std::error_code putRequestInfo(
			const CTransferRequestInfo& requestInfo) noexcept override;
	//==========================================================================
		/**
		* открыть файл для чтения.
		* @param pTransferFile - объект передачи.
		* @return - результат выполнения запроса.
		*/
		std::error_code openFile(
			CTransferFile* const pTransferFile) override;
	//==========================================================================
		/**
		* создать файл для записи.
		* @param pTransferFile - объект передачи.
		* @return - результат выполнения запроса.
		*/
		std::error_code createFile(
			CTransferFile* const pTransferFile) override;
	//==========================================================================
		/**
		* асинхронное чтение файла.
		* @param pFileBufferRead - буфер передачи.
		* @return - результат выполнения запроса.
		*/
		std::error_code startAsyncRead(
			CTransferFileBuffer* const pFileBufferRead) override;
	//==========================================================================
		/**
		* асинхронная запись в файл.
		* @param pFileBufferWrite - буфер передачи.
		* @return - результат выполнения запроса.
		*/
		std::error_code startAsyncWrite(
			CTransferFileBuffer* const pFileBufferWrite) override;
	//==========================================================================
		/**
		* закрыть файл.
		* @param pTransferFile - объект передачи.
		* @return - результат закрытия файла.
		*/
		std::error_code closeFile(
			CTransferFile* const pTransferFile) noexcept override;
	//==========================================================================
		/**
		* окончание работ с участником передачи для запроса.
		* @param pTransferRequest - объект запроса.
		* @return - результат завершения работ.
		*/
		std::error_code closeMember(
			CTransferRequest* const pTransferRequest) noexcept override;
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
		/** путь до папки */
		std::filesystem::path _directoryPath;

		/** список передаваемых файлов */
		using FileList = std::unordered_map<
			CTransferFile*,
			std::shared_ptr<CTransferDirectoryFile>>;

		/** активные передачи */
		std::unordered_map<CTransferRequest*, FileList> _transferActive;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END