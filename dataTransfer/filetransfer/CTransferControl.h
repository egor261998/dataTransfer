#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** передача файлов */
	class DATATRANSFER CTransferControl : protected wname::misc::CCounter
	{
	#pragma region Protected_Inner
	protected:
	//==========================================================================
		/** интерфейс передачи */
		class ITransferMember;

		/** участники передачи */
		class CTransferDirectory;
		class CTransferTcpServer;
		class CTransferTcpClient;

		/** интерфейс передачи для TCP участников */
		template<class T>
		class ITransferTcpCommunication;
		
		/** объект запроса передачи файлов*/
		class CTransferRequest;
		class CTransferRequestInfo;
		/** объект передачи файла */
		class CTransferFile;
		class CTransferFileBuffer;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Inner
	public:
	//==========================================================================
		/** флаги доступа */
		#pragma pack(1)
		typedef struct _SAccessFlag
		{
			/** разрешает копирование*/
			bool bIsCopy;
			/** разрешает вставку */
			bool bIsPaste;
			/** разрешает перезапись */
			bool bIsOverrite;
		}SAccessFlag, * PSAccessFlag;
		#pragma pack()
	//==========================================================================
		/** запросы */
		enum class ERequest : UINT8
		{
			eNone,
			/** копирование из участника передачи */
			eCopy,
			/** вставка из участника передачи */
			ePaste,
		};
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор источника передачи файлов для папки.
		* @param sAccessFlag - флаги доступа.
		* @param directoryPath - абсолютный путь до папки источника.
		* @param pIocp - механизм Iocp.
		*/
		CTransferControl(
			const SAccessFlag& sAccessFlag,
			const std::filesystem::path directoryPath,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* конструктор источника передачи файлов для TCP клиента.
		* @param sAccessFlag - флаги доступа.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @param pIocp - механизм Iocp.
		*/
		CTransferControl(
			const SAccessFlag& sAccessFlag,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* добавить получателя как папку.
		* @param sAccessFlag - флаги доступа.
		* @param directoryPath - абсолютный путь до папки получателя.
		* @return - результат завершения добавления.
		*/
		std::error_code addRecipient(
			const SAccessFlag& sAccessFlag,
			const std::filesystem::path directoryPath);
	//==========================================================================
		/**
		* добавить получателя как TCP сервер.
		* @param sAccessFlag - флаги доступа.
		* @param strIp - IP адрес сервера.
		* @param wPort - порт сервера.
		* @return - результат завершения добавления.
		*/
		std::error_code addRecipient(
			const SAccessFlag& sAccessFlag,
			const std::string strIp,
			const WORD wPort);
	//==========================================================================
		/**
		* запрос на источник.
		* @param eRequest - запрос.
		* @param requestPath - абсолютный путь запроса.
		* @return - результат запроса.
		*/
		std::error_code request(
			const ERequest eRequest,
			const std::filesystem::path requestPath);
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
		~CTransferControl();
	//==========================================================================
		CTransferControl(const CTransferControl&) = delete;
		CTransferControl(CTransferControl&&) = delete;
		CTransferControl& operator=(const CTransferControl&) = delete;
		CTransferControl& operator=(CTransferControl&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* запрос на источник через участника.
		* @param eRequest - запрос.
		* @param requestPath - абсолютный путь запроса.
		* @param pMember - запрос через участника.
		* @return - результат запроса.
		*/
		std::error_code request(
			const ERequest eRequest,
			const std::filesystem::path requestPath,
			ITransferMember* const pMember);
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** механизм ввода/вывода */
		const std::shared_ptr<wname::io::iocp::CIocp> _pIocp;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** источник файлов */
		std::shared_ptr<ITransferMember> _source;
		/** получатели файлов */
		std::map<std::wstring, std::shared_ptr<ITransferMember>> _recipients;

		/** запросов в обработке */
		std::unordered_map<CTransferRequest*, std::shared_ptr<CTransferRequest>> _requests;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END