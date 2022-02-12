#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** ���������� ������
		[��������]<->[����������] */
	class DATATRANSFER CTransferControl::CTransferTcpServer final : 
		public ITransferMember
	{
	#pragma region Private_Inner
	private:
	//==========================================================================
		/** TCP ������ */
		class CClientFromServer;
		friend class CClientFromServer;
	//==========================================================================
		/** TCP ������ ��� ������������ ���� */
		class CTcpServerFromMember;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ��������� �������� ������ ��� TCP �������.
		* @param pTransferControl - ����������� ������.
		* @param sAccessFlag - ����� �������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		*/
		CTransferTcpServer(
			CTransferControl* const pTransferControl,
			const SAccessFlag& sAccessFlag,
			const std::string strIp,
			const WORD wPort);
	//==========================================================================
		/**
		* ������ ����� � ���������� �������� ��� �������� �������.
		* @param pTransferRequest - ������ �������.
		* @return - ��������� �������.
		*/
		std::error_code openMember(
			CTransferRequest* const pTransferRequest) override;
	//==========================================================================
		/**
		* ������ � ��������� ��������.
		* @param requestInfo - ���������� �������.
		* @return - ��������� �������.
		*/
		std::error_code getRequestInfo(
			CTransferRequestInfo& requestInfo) override;
	//==========================================================================
		/**
		* ����� �� ��������� ��������.
		* @param requestInfo - ���������� �������.
		* @return - ��������� �������.
		*/
		std::error_code putRequestInfo(
			const CTransferRequestInfo& requestInfo) override;
	//==========================================================================
		/**
		* ������� ���� ��� ������.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code openFile(
			CTransferFile* const pTransferFile) override;
	//==========================================================================
		/**
		* ������� ���� ��� ������.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code createFile(
			CTransferFile* const pTransferFile) override;
	//==========================================================================
		/**
		* ����������� ������ �����.
		* @param pFileBufferRead - ����� ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code startAsyncRead(
			CTransferFileBuffer* const pFileBufferRead) override;
	//==========================================================================
		/**
		* ����������� ������ � ����.
		* @param pFileBufferWrite - ����� ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code startAsyncWrite(
			CTransferFileBuffer* const pFileBufferWrite) override;
	//==========================================================================
		/**
		* ������� ����.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� �������� �����.
		*/
		std::error_code closeFile(
			CTransferFile* const pTransferFile) noexcept override;
	//==========================================================================
		/**
		* ��������� ����� � ���������� �������� ��� �������.
		* @param pTransferRequest - ������ �������.
		* @return - ��������� ���������� �����.
		*/
		std::error_code closeMember(
			CTransferRequest* const pTransferRequest) noexcept override;
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
		~CTransferTcpServer();
	//==========================================================================
		CTransferTcpServer(const CTransferTcpServer&) = delete;
		CTransferTcpServer(CTransferTcpServer&&) = delete;
		CTransferTcpServer& operator=(const CTransferTcpServer&) = delete;
		CTransferTcpServer& operator=(CTransferTcpServer&&) = delete;
	//==========================================================================
	#pragma endregion

	#pragma region Protected_Data
	protected:
	//==========================================================================
		/** TCP ������ */
		std::unique_ptr<CTcpServerFromMember> _pTcpServerFromMember;

		/** �������� �������� */
		std::unordered_map<std::wstring, std::list<CClientFromServer*>> _transferFree;
		std::unordered_map<CTransferRequest*, CClientFromServer*> _transferActive;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END