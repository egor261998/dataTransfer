#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** �������� ������ */
	class DATATRANSFER CTransferControl::CTransferFile final : 
		protected wname::misc::CCounter
	{
	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ��������� �������� ������ ��� �����.
		* @param pTransferRequest - ������ �������.
		* @param fileInfo - ���������� � �����.
		*/
		CTransferFile(
			CTransferRequest* const pTransferRequest,
			const CTransferRequestInfo::SFileInfo& fileInfo);
	//==========================================================================
		/**
		* ������ �� ��������.
		* @return - ��������� �������.
		*/
		std::error_code request();
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������.
		* @param pFileBufferRead- ����� ��������.
		* @param ec - ��������� ����������.
		*/
		void asyncReadComplitionHandler(
			CTransferFileBuffer* const pFileBufferRead,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������.
		* @param pFileBufferWrite - ����� ��������.
		* @param ec - ��������� ����������.
		*/
		void asyncWriteComplitionHandler(
			CTransferFileBuffer* const pFileBufferWrite,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ��������� ������.
		* @param bIsWait - ������� ��������.
		*/
		void release(
			const bool bIsWait) noexcept override;
	//==========================================================================
		/**
		* ����������.
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
		* ������ ����� ���� ������ � ������/�������.
		*/
		void fileIo();
	//==========================================================================
		/**
		* ����������� ������.
		* @return - ��������� �������.
		*/
		std::error_code asyncRead();
	//==========================================================================
		/**
		* ����������� ������.
		* @return - ��������� �������.
		*/
		std::error_code asyncWrite();
	//==========================================================================
		/**
		* ��������� ��������.
		* @parma ec - ���������� ��������.
		*/
		void endTransfer(
			const std::error_code ec) noexcept;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Data
	public:
	//==========================================================================
		/** ������������ ���������� ������� � ������� */
		static constexpr DWORD _dwCountBuffer = 100;

		/** ������ ������� */
		CTransferRequest* const _pTransferRequest;

		/** ����������� ������ */
		UINT64 _uReadSize = 0;
		/** ���������� ������ */
		UINT64 _uWriteSize = 0;
		/** ����� �������� �� ������ */
		UINT64 _uReadOffset = 0;
		/** ����� �������� �� ������ */
		UINT64 _uWriteOffset = 0;

		/** ������� ���������� �������� ����� */
		wname::handle::CEvent _evEndTransfer;

		/** ���������� � ����� */
		const CTransferRequestInfo::SFileInfo _fileInfo;

		/** ���������� �������� */
		bool _bIsTransfer = false;

		/** ���������� �������� */
		std::error_code _ec;

		using TBufferList = std::unordered_map<
			CTransferFileBuffer*, std::unique_ptr<CTransferFileBuffer>>;
		/** ������ ������� �� ������ */
		TBufferList _listBufferRead;
		/** ������ ������� �� ������ */
		TBufferList _listBufferWrite;
		/** ������ � ��������� �� ������*/
		TBufferList _listBufferReadBusy;
		/** ������ � ��������� �� ������*/
		TBufferList _listBufferWriteBusy;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END