#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** ������ ������� ��������� �������� */
	class DATATRANSFER CTransferControl::CTransferRequestInfo final
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** ���������� � ����� */
		#pragma pack(1)
		typedef struct _SFileInfo
		{
			/** ������ ����� */
			UINT64 uSize;
			/** ������������� ���� �� ����� */
			WCHAR wszFilePath[MAX_PATH];
		}SFileInfo, * PSFileInfo;
		#pragma pack()
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ���������� �������.
		* @param pTransferRequest - ������ �������.
		* @param pMember - �������� �������� �������.
		*/
		CTransferRequestInfo(
			CTransferRequest* const pTransferRequest,
			ITransferMember* const pMember) noexcept;
	//==========================================================================
		/**
		* ����������.
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
		/** ������ ������� */
		CTransferRequest* const _pTransferRequest;

		/** �������� �������� */
		ITransferMember* const _pMember;
		
		/** ������ ������ */
		std::vector<SFileInfo> _files;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END