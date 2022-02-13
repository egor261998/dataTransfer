#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	class DATATRANSFER CTransferControl::CTransferFileBuffer final
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** информация о буфере*/
		#pragma pack(1)
		typedef struct _SBufferInfo
		{
			/** смещение передачи */
			UINT64 offset;
			/** размер буфера */
			DWORD dwSize;
			/** ошибка передачи для протоколов передачи */
			DWORD dwError;
			/** буфер передачи */
			BYTE data[BUFFER_64K];
		}SBufferInfo, * PSBufferInfo;
		#pragma pack()
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* конструктор буфера для передачи.
		* @param pTransferFile - передаваемый файл.
		*/
		CTransferFileBuffer(
			CTransferFile* const pTransferFile);
	//==========================================================================
		/**
		* деструктор.
		*/
		~CTransferFileBuffer() = default;
	//==========================================================================
		CTransferFileBuffer(const CTransferFileBuffer&) = delete;
		CTransferFileBuffer(CTransferFileBuffer&&) = delete;
		CTransferFileBuffer& operator=(const CTransferFileBuffer&) = delete;
		CTransferFileBuffer& operator=(CTransferFileBuffer&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** передаваемый файл */
		CTransferFile* const _pTransferFile;

		/** буфер */
		SBufferInfo _buffer { 0 };
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END