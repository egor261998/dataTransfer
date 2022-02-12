#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** объект запроса передачи */
	class DATATRANSFER CTransferControl::CTransferRequest final : 
		public wname::misc::CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор запроса.
		* @param pTransferControl - родительный объект.
		* @param eRequest - запрос.
		* @param requestPath - относительный путь запроса.
		* @param pMemberIn - источник.
		* @param pMemberOut - получатель.
		*/
		CTransferRequest(
			CTransferControl* const pTransferControl,
			const ERequest eRequest,
			const std::filesystem::path requestPath,
			ITransferMember* const pMemberIn,
			ITransferMember* const pMemberOut);
	//==========================================================================
		/**
		* начать запрос.
		* @return - результат запроса.
		*/
		std::error_code request();
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
		~CTransferRequest();
	//==========================================================================
		CTransferRequest(const CTransferRequest&) = delete;
		CTransferRequest(CTransferRequest&&) = delete;
		CTransferRequest& operator=(const CTransferRequest&) = delete;
		CTransferRequest& operator=(CTransferRequest&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Method
	private:
	//==========================================================================
		/**
		* передача файла.
		* @param fileInfo - информация о файле.
		* @return - результат передачи.
		*/
		std::error_code transferFile(
			const CTransferRequestInfo::SFileInfo& fileInfo);
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** родительный объект */
		CTransferControl* const _pTransferControl;

		/** источник файлов */
		ITransferMember* const _pMemberIn;
		/** получатель файлов */
		ITransferMember* const _pMemberOut;

		/** запрос для источника */
		const ERequest _eRequest;
		/** относительный путь запроса */
		const std::filesystem::path _requestPath;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END