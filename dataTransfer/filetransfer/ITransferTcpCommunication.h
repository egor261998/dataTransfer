#pragma once

_DATATRANSFER_BEGIN
namespace filetransfer
{
	/** ������� ����� TCP ��������� */
	template<class T>
	class DATATRANSFER CTransferControl::ITransferTcpCommunication
	{
	#pragma region Public_Inner
	public:
	//==========================================================================
		/** ��� ��������� */
		enum class EMessageType : UINT8
		{
			eError,
			eRequestInfo,
			eOpenFile,
			eIo,
			eCloseFile
		};
	//==========================================================================
		/** ���������� � ������*/
		#pragma pack(1)
		typedef struct _SHeader
		{
			/** ��� ��������� */
			EMessageType eMessageType;
		}SHeader, * PSHeader;
		#pragma pack()
	//==========================================================================
	#pragma endregion

	#pragma region Public_Method
	public:
	//==========================================================================
		/**
		* ����������� ���������� �������.
		* @param pClient - ������.
		*/
		ITransferTcpCommunication(
			T* const pClient) noexcept;
	//==========================================================================
		/**
		* ��������� ������.
		* @param eRequest - �������.
		* @param requestPath - ���� �������.
		* @return - ��������� �������.
		*/
		std::error_code sendRequestPath(
			const ERequest eRequest,
			const std::wstring requestPath);
	//==========================================================================
		/**
		* ������� ������.
		* @param eRequest - �������.
		* @param requestPath - ���� �������.
		* @return - ��������� �������.
		*/
		std::error_code recvRequestPath(
			ERequest& eRequest,
			std::wstring requestPath);
	//==========================================================================
		/**
		* ������ � ��������� ��������.
		* @param requestInfo - ���������� �������.
		* @return - ��������� �������.
		*/
		std::error_code getRequestInfo(
			CTransferRequestInfo& requestInfo);
	//==========================================================================
		/**
		* ����� �� ��������� ��������.
		* @param requestInfo - ���������� �������.
		* @return - ��������� �������.
		*/
		std::error_code putRequestInfo(
			const CTransferRequestInfo& requestInfo);
	//==========================================================================
		/**
		* ����������� ������ �����.
		* @param pFileBufferRead - ����� ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code startAsyncRead(
			CTransferFileBuffer* const pFileBufferRead);
	//==========================================================================
		/**
		* ����������� ������ � ����.
		* @param pFileBufferWrite - ����� ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code startAsyncWrite(
			CTransferFileBuffer* const pFileBufferWrite);
	//==========================================================================
		/**
		* �������� � ������.
		* @param pTransferFile - ������ ��������.
		* @return - ��������� ���������� �������.
		*/
		std::error_code operationFile(
			CTransferFile* const pTransferFile);
	//==========================================================================
	#pragma endregion	

	#pragma region Protected_Method
	protected:
	//==========================================================================
		/**
		* ���������� ������� ���������� ������������ ������.
		* @param bufferRecv - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
		*/
		void asyncRecvComplitionHandler(
			const PBYTE bufferRecv,
			const DWORD dwReturnSize,
			const std::error_code ec) noexcept;
	//==========================================================================
		/**
		* ���������� ������� ���������� ����������� ������.
		* @param bufferSend - ����� ������.
		* @param dwReturnSize - ���������� ���������� ����.
		* @param ec - ��� ����������.
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
		/** ������ ������� */
		T* const _pClient;
	//==========================================================================
	#pragma endregion
	};
}
_DATATRANSFER_END