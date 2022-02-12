#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** передача файлов */
	class DATATRANSFER CTransferControl::CTransferFile final : 
		protected wname::misc::CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор источника передачи файлов для папки.
		* @param pTransferRequest - объект запроса.
		* @param fileInfo - информация о файле.
		*/
		CTransferFile(
			CTransferRequest* const pTransferRequest,
			const CTransferRequestInfo::SFileInfo& fileInfo);
	//==========================================================================
		/**
		* запрос на источник.
		* @return - результат запроса.
		*/
		std::error_code request();
	//==========================================================================
		/**
		* обработчик события завершения асинхронного чтения.
		* @param pFileBufferRead- буфер передачи.
		* @param ec - результат завершения.
		*/
		void asyncReadComplitionHandler(
			CTransferFileBuffer* const pFileBufferRead,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи.
		* @param pFileBufferWrite - буфер передачи.
		* @param ec - результат завершения.
		*/
		void asyncWriteComplitionHandler(
			CTransferFileBuffer* const pFileBufferWrite,
			const std::error_code ec) noexcept;
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
		~CTransferFile();
	//==========================================================================
		CTransferFile(const CTransferFile&) = delete;
		CTransferFile(CTransferFile&&) = delete;
		CTransferFile& operator=(const CTransferFile&) = delete;
		CTransferFile& operator=(CTransferFile&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* начать новый цикл работы с вводом/выводом.
		*/
		void fileIo();
	//==========================================================================
		/**
		* асинхронное чтение.
		* @return - результат запроса.
		*/
		std::error_code asyncRead();
	//==========================================================================
		/**
		* асинхронная запись.
		* @return - результат запроса.
		*/
		std::error_code asyncWrite();
	//==========================================================================
		/**
		* окончание передачи.
		* @parma ec - завершения передачи.
		*/
		void endTransfer(
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** максимальное количество буферов в очереди */
		static constexpr DWORD _dwCountBuffer = 100;

		/** объект запроса */
		CTransferRequest* const _pTransferRequest;

		/** прочитанный размер */
		UINT64 _uReadSize = 0;
		/** записанный размер */
		UINT64 _uWriteSize = 0;
		/** оффет передачи на чтение */
		UINT64 _uReadOffset = 0;
		/** оффет передачи на запись */
		UINT64 _uWriteOffset = 0;

		/** событие завершения передачи файла */
		wname::handle::CEvent _evEndTransfer;

		/** информация о файле */
		const CTransferRequestInfo::SFileInfo _fileInfo;

		/** активность передачи */
		bool _bIsTransfer = false;

		/** завершения передачи */
		std::error_code _ec;

		using TBufferList = std::unordered_map<
			CTransferFileBuffer*, std::unique_ptr<CTransferFileBuffer>>;
		/** список буферов на чтение */
		TBufferList _listBufferRead;
		/** список буферов за запись */
		TBufferList _listBufferWrite;
		/** буферы в обработке на чтение*/
		TBufferList _listBufferReadBusy;
		/** буферы в обработке на запись*/
		TBufferList _listBufferWriteBusy;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END