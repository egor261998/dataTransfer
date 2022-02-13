#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	class DATATRANSFER CTransferControl::CTransferFileBuffer final
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** ���������� � ������*/
		#pragma pack(1)
		typedef struct _SBufferInfo
		{
			/** �������� �������� */
			UINT64 offset;
			/** ������ ������ */
			DWORD dwSize;
			/** ������ �������� ��� ���������� �������� */
			DWORD dwError;
			/** ����� �������� */
			BYTE data[BUFFER_64K];
		}SBufferInfo, * PSBufferInfo;
		#pragma pack()
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ������ ��� ��������.
		* @param pTransferFile - ������������ ����.
		*/
		CTransferFileBuffer(
			CTransferFile* const pTransferFile);
	//==========================================================================
		/**
		* ����������.
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
		/** ������������ ���� */
		CTransferFile* const _pTransferFile;

		/** ����� */
		SBufferInfo _buffer { 0 };
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END