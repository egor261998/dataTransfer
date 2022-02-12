#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** источник файлов
		[источник]<->[получатель] */
	class DATATRANSFER CTransferControl::CTransferTcpClient final : 
		public ITransferMember
	{
	#pragma region Private_Inner
	private:
	//==========================================================================
		/** TCP клиент */
		class CClient;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор участника передачи файлов для TCP клиента.
		* @param pTransferControl - родительный объект.
		* @param sAccessFlag - флаги доступа.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		*/
		CTransferTcpClient(
			CTransferControl* const pTransferControl,
			const SAccessFlag& sAccessFlag,
			const std::string strIp,
			const WORD wPort);
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
			const CTransferRequestInfo& requestInfo) override;
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
		~CTransferTcpClient();
	//==========================================================================
		CTransferTcpClient(const CTransferTcpClient&) = delete;
		CTransferTcpClient(CTransferTcpClient&&) = delete;
		CTransferTcpClient& operator=(const CTransferTcpClient&) = delete;
		CTransferTcpClient& operator=(CTransferTcpClient&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** адрес до сервера */
		const std::string _strIp;
		/** порт сервера */
		const WORD _wPort;

		/** активная передача */
		std::unordered_map<CTransferRequest*, std::shared_ptr<CClient>> _transferActive;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END