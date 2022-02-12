#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** �������� ������ */
	class DATATRANSFER CTransferControl : protected wname::misc::CCounter
	{
	#pragma region Protected_Inner
	protected:
	//==========================================================================
		/** ��������� �������� */
		class ITransferMember;

		/** ��������� �������� */
		class CTransferDirectory;
		class CTransferTcpServer;
		class CTransferTcpClient;

		/** ��������� �������� ��� TCP ���������� */
		template<class T>
		class ITransferTcpCommunication;
		
		/** ������ ������� �������� ������*/
		class CTransferRequest;
		class CTransferRequestInfo;
		/** ������ �������� ����� */
		class CTransferFile;
		class CTransferFileBuffer;
	//==========================================================================
	#pragma endregion

	#pragma region Public_Inner
	public:
	//==========================================================================
		/** ����� ������� */
		#pragma pack(1)
		typedef struct _SAccessFlag
		{
			/** ��������� �����������*/
			bool bIsCopy;
			/** ��������� ������� */
			bool bIsPaste;
			/** ��������� ���������� */
			bool bIsOverrite;
		}SAccessFlag, * PSAccessFlag;
		#pragma pack()
	//==========================================================================
		/** ������� */
		enum class ERequest : UINT8
		{
			eNone,
			/** ����������� �� ��������� �������� */
			eCopy,
			/** ������� �� ��������� �������� */
			ePaste,
		};
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ��������� �������� ������ ��� �����.
		* @param sAccessFlag - ����� �������.
		* @param directoryPath - ���������� ���� �� ����� ���������.
		* @param pIocp - �������� Iocp.
		*/
		CTransferControl(
			const SAccessFlag& sAccessFlag,
			const std::filesystem::path directoryPath,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* ����������� ��������� �������� ������ ��� TCP �������.
		* @param sAccessFlag - ����� �������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @param pIocp - �������� Iocp.
		*/
		CTransferControl(
			const SAccessFlag& sAccessFlag,
			const std::string strIp,
			const WORD wPort,
			const std::shared_ptr<wname::io::iocp::CIocp>& pIocp);
	//==========================================================================
		/**
		* �������� ���������� ��� �����.
		* @param sAccessFlag - ����� �������.
		* @param directoryPath - ���������� ���� �� ����� ����������.
		* @return - ��������� ���������� ����������.
		*/
		std::error_code addRecipient(
			const SAccessFlag& sAccessFlag,
			const std::filesystem::path directoryPath);
	//==========================================================================
		/**
		* �������� ���������� ��� TCP ������.
		* @param sAccessFlag - ����� �������.
		* @param strIp - IP ����� �������.
		* @param wPort - ���� �������.
		* @return - ��������� ���������� ����������.
		*/
		std::error_code addRecipient(
			const SAccessFlag& sAccessFlag,
			const std::string strIp,
			const WORD wPort);
	//==========================================================================
		/**
		* ������ �� ��������.
		* @param eRequest - ������.
		* @param requestPath - ���������� ���� �������.
		* @return - ��������� �������.
		*/
		std::error_code request(
			const ERequest eRequest,
			const std::filesystem::path requestPath);
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
		* ������ �� �������� ����� ���������.
		* @param eRequest - ������.
		* @param requestPath - ���������� ���� �������.
		* @param pMember - ������ ����� ���������.
		* @return - ��������� �������.
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
		/** �������� �����/������ */
		const std::shared_ptr<wname::io::iocp::CIocp> _pIocp;
	//==========================================================================
	#pragma endregion

	#pragma region Private_Data
	private:
	//==========================================================================
		/** �������� ������ */
		std::shared_ptr<ITransferMember> _source;
		/** ���������� ������ */
		std::map<std::wstring, std::shared_ptr<ITransferMember>> _recipients;

		/** �������� � ��������� */
		std::unordered_map<CTransferRequest*, std::shared_ptr<CTransferRequest>> _requests;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END