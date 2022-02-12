#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** участник передачи */
	class DATATRANSFER CTransferControl::ITransferMember :
		protected wname::misc::CCounter
	{
		friend class CTransferRequest;
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор участника передачи.
		* @param pTransferControl - родительный объект.
		* @param sAccessFlag - флаги доступа.
		*/
		ITransferMember(
			CTransferControl* const pTransferControl,
			const SAccessFlag& sAccessFlag);
	//==========================================================================
		/**
		* начало работ с участником передачи для текущего запроса.
		* @param pTransferRequest - объект запроса.
		* @return - результат запроса.
		*/
		virtual std::error_code openMember(
			CTransferRequest* const pTransferRequest) = 0;
	//==========================================================================
		/**
		* запрос у участника передачи.
		* @param requestInfo - информация запроса.
		* @return - результат запроса.
		*/
		virtual std::error_code getRequestInfo(
			CTransferRequestInfo& requestInfo) = 0;
	//==========================================================================
		/**
		* ответ от участнику передачи.
		* @param requestInfo - информация запроса.
		* @return - результат запроса.
		*/
		virtual std::error_code putRequestInfo(
			const CTransferRequestInfo& requestInfo) = 0;
	//==========================================================================
		/**
		* открыть файл для чтения.
		* @param pTransferFile - объект передачи.
		* @return - результат выполнения запроса.
		*/
		virtual std::error_code openFile(
			CTransferFile* const pTransferFile) = 0;
	//==========================================================================
		/**
		* создать файл для записи.
		* @param pTransferFile - объект передачи.
		* @return - результат выполнения запроса.
		*/
		virtual std::error_code createFile(
			CTransferFile* const pTransferFile) = 0;
	//==========================================================================
		/**
		* асинхронное чтение файла.
		* @param pFileBufferRead - буфер передачи.
		* @return - результат выполнения запроса.
		*/
		virtual std::error_code startAsyncRead(
			CTransferFileBuffer* const pFileBufferRead) = 0;
	//==========================================================================
		/**
		* асинхронная запись в файл.
		* @param pFileBufferWrite - буфер передачи.
		* @return - результат выполнения запроса.
		*/
		virtual std::error_code startAsyncWrite(
			CTransferFileBuffer* const pFileBufferWrite) = 0;
	//==========================================================================
		/**
		* закрыть файл.
		* @param pTransferFile - объект передачи.
		* @return - результат закрытия файла.
		*/
		virtual std::error_code closeFile(
			CTransferFile* const pTransferFile) noexcept = 0;
	//==========================================================================
		/**
		* окончание работ с участником передачи для запроса.
		* @param pTransferRequest - объект запроса.
		* @return - результат завершения работ.
		*/
		virtual std::error_code closeMember(
			CTransferRequest* const pTransferRequest) noexcept = 0;
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
		/** родительный объект */
		CTransferControl* const _pTransferControl;
		/** флаги доступа */
		const SAccessFlag _sAccessFlag;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END