#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** объект запроса иформации передачи */
	class DATATRANSFER CTransferControl::CTransferRequestInfo final
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** информация о файле */
		#pragma pack(1)
		typedef struct _SFileInfo
		{
			/** размер файла */
			UINT64 uSize;
			/** относительный путь до файла */
			WCHAR wszFilePath[MAX_PATH];
		}SFileInfo, * PSFileInfo;
		#pragma pack()
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор информации запроса.
		* @param pTransferRequest - объект запроса.
		* @param pMember - участник передачи запроса.
		*/
		CTransferRequestInfo(
			CTransferRequest* const pTransferRequest,
			ITransferMember* const pMember) noexcept;
	//==========================================================================
		/**
		* деструктор.
		*/
		~CTransferRequestInfo() = default;
	//==========================================================================
		CTransferRequestInfo(const CTransferRequestInfo&) = delete;
		CTransferRequestInfo(CTransferRequestInfo&&) = delete;
		CTransferRequestInfo& operator=(const CTransferRequestInfo&) = delete;
		CTransferRequestInfo& operator=(CTransferRequestInfo&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** объект запроса */
		CTransferRequest* const _pTransferRequest;

		/** участник передачи */
		ITransferMember* const _pMember;
		
		/** список файлов */
		std::vector<SFileInfo> _files;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END