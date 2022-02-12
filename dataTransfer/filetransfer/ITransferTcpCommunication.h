#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** общение между TCP клиентами */
	template<class T>
	class DATATRANSFER CTransferControl::ITransferTcpCommunication
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** тип сообщения */
		enum class EMessageType : UINT8
		{
			eError,
			eRequestInfo,
			eOpenFile,
			eIo,
			eCloseFile
		};
	//==========================================================================
		/** информация о буфере*/
		#pragma pack(1)
		typedef struct _SHeader
		{
			/** тип сообщения */
			EMessageType eMessageType;
		}SHeader, * PSHeader;
		#pragma pack()
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор интерфейса общения.
		* @param pClient - клиент.
		*/
		ITransferTcpCommunication(
			T* const pClient) noexcept;
	//==========================================================================
		/**
		* отправить запрос.
		* @param eRequest - запроса.
		* @param requestPath - путь запроса.
		* @return - результат запроса.
		*/
		std::error_code sendRequestPath(
			const ERequest eRequest,
			const std::wstring requestPath);
	//==========================================================================
		/**
		* принять запрос.
		* @param eRequest - запроса.
		* @param requestPath - путь запроса.
		* @return - результат запроса.
		*/
		std::error_code recvRequestPath(
			ERequest& eRequest,
			std::wstring requestPath);
	//==========================================================================
		/**
		* запрос у участника передачи.
		* @param requestInfo - информация запроса.
		* @return - результат запроса.
		*/
		std::error_code getRequestInfo(
			CTransferRequestInfo& requestInfo);
	//==========================================================================
		/**
		* ответ от участнику передачи.
		* @param requestInfo - информация запроса.
		* @return - результат запроса.
		*/
		std::error_code putRequestInfo(
			const CTransferRequestInfo& requestInfo);
	//==========================================================================
		/**
		* асинхронное чтение файла.
		* @param pFileBufferRead - буфер передачи.
		* @return - результат выполнения запроса.
		*/
		std::error_code startAsyncRead(
			CTransferFileBuffer* const pFileBufferRead);
	//==========================================================================
		/**
		* асинхронная запись в файл.
		* @param pFileBufferWrite - буфер передачи.
		* @return - результат выполнения запроса.
		*/
		std::error_code startAsyncWrite(
			CTransferFileBuffer* const pFileBufferWrite);
	//==========================================================================
		/**
		* операция с файлом.
		* @param pTransferFile - объект передачи.
		* @return - результат выполнения запроса.
		*/
		std::error_code operationFile(
			CTransferFile* const pTransferFile);
	//==========================================================================
	#pragma endregion	

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* обработчик события завершения асинхронного чтения.
		* @param bufferRecv - буфер данных.
		* @param dwReturnSize - количество полученных байт.
		* @param ec - код завершения.
		*/
		void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* обработчик события завершения асинхронной записи.
		* @param bufferSend - буфер данных.
		* @param dwReturnSize - количество переданных байт.
		* @param ec - код завершения.
		*/
		void asyncSendComplitionHandler(
			const PBYTE bufferSend,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** объект общения */
		T* const _pClient;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END