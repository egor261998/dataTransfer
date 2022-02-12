#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;
template class CTransferControl::ITransferTcpCommunication<wname::network::CTcpConnectedClient>;
template class CTransferControl::ITransferTcpCommunication<wname::network::CTcpClient>;

//==============================================================================
template<class T>
CTransferControl::ITransferTcpCommunication<T>::ITransferTcpCommunication(
	T* const pClient) noexcept :
	_pClient(pClient) 
{

}
//==============================================================================
template<class T>
std::error_code CTransferControl::ITransferTcpCommunication<T>::sendRequestPath(
	const ERequest eRequest,
	const std::wstring requestPath)
{
	#pragma warning(disable: 26493)
	const DWORD dwSize = (DWORD)(requestPath.length() * sizeof(WCHAR));
	DWORD dwReturnedByte = 0;
	auto ec = _pClient->startSend(
		(PBYTE)&eRequest,
		sizeof(eRequest),
		&dwReturnedByte);
	if (ec)
	{
		/** �� ������� ��������� */
		return ec;
	}
	if (dwReturnedByte != sizeof(eRequest))
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}
	ec = _pClient->startSend(
		(PBYTE)&dwSize,
		sizeof(dwSize),
		&dwReturnedByte);
	if (ec)
	{
		/** �� ������� ��������� */
		return ec;
	}
	if (dwReturnedByte != sizeof(dwSize))
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}
	if (dwSize == 0)
	{
		/** ���� ������� */
		return ec;
	}
	ec = _pClient->startSend(
		(PBYTE)requestPath.data(),
		dwSize,
		&dwReturnedByte);
	if (ec)
	{
		/** �� ������� ��������� */
		return ec;
	}

	if (dwReturnedByte != dwSize)
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}

	return ec;
}
//==============================================================================
template<class T>
std::error_code CTransferControl::ITransferTcpCommunication<T>::recvRequestPath(
	ERequest& eRequest,
	std::wstring requestPath)
{
	#pragma warning(disable: 26493)
	DWORD dwSize = 0;
	DWORD dwReturnedByte = 0;
	auto ec = _pClient->startRecv(
		(PBYTE)&eRequest,
		sizeof(eRequest),
		&dwReturnedByte,
		MSG_WAITALL);
	if (ec)
	{
		/** �� ������� ��������� */
		return ec;
	}
	if (dwReturnedByte != sizeof(eRequest))
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}
	ec = _pClient->startRecv(
		(PBYTE)&dwSize,
		sizeof(dwSize),
		&dwReturnedByte),
		MSG_WAITALL;
	if (ec)
	{
		/** �� ������� ��������� */
		return ec;
	}
	if (dwReturnedByte != sizeof(dwSize))
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}
	if (dwSize == 0)
	{
		/** ���� ������� */
		return ec;
	}
	requestPath.resize(dwSize / sizeof(WCHAR));
	ec = _pClient->startRecv(
		(PBYTE)requestPath.data(),
		dwSize,
		&dwReturnedByte,
		MSG_WAITALL);
	if (ec)
	{
		/** �� ������� ������� */
		return ec;
	}

	if (dwReturnedByte != dwSize)
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}

	return ec;
}
//==============================================================================
template<class T>
std::error_code CTransferControl::ITransferTcpCommunication<T>::getRequestInfo(
	CTransferRequestInfo& requestInfo)
{
	#pragma warning(disable: 26493)
	DWORD dwSize = 0;
	DWORD dwReturnedByte = 0;
	auto ec = _pClient->startRecv(
		(PBYTE)&dwSize,
		sizeof(dwSize),
		&dwReturnedByte,
		MSG_WAITALL);
	if (ec)
	{
		/** �� ������� ��������� */
		return ec;
	}
	if (dwReturnedByte != sizeof(dwSize))
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}
	if (dwSize == 0)
	{
		/** ������ ������ */
		return ec;
	}
	requestInfo._files.resize(dwSize);
	dwSize *= sizeof(*requestInfo._files.data());
	ec = _pClient->startRecv(
		(PBYTE)requestInfo._files.data(),
		dwSize,
		&dwReturnedByte,
		MSG_WAITALL);
	if (ec)
	{
		/** �� ������� ������� */
		return ec;
	}

	if (dwReturnedByte != dwSize)
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}

	return ec;
}
//==============================================================================
template<class T>
std::error_code CTransferControl::ITransferTcpCommunication<T>::putRequestInfo(
	const CTransferRequestInfo& requestInfo)
{
	#pragma warning(disable: 26493)
	DWORD dwSize = (DWORD)requestInfo._files.size();
	DWORD dwReturnedByte = 0;
	auto ec = _pClient->startSend(
		(PBYTE)&dwSize,
		sizeof(dwSize),
		&dwReturnedByte);
	if (ec)
	{
		/** �� ������� ��������� */
		return ec;
	}
	if (dwReturnedByte != sizeof(dwSize))
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}
	if (dwSize == 0)
	{
		/** ������ ������ */
		return ec;
	}
	dwSize *= sizeof(*requestInfo._files.data());
	ec = _pClient->startSend(
		(PBYTE)requestInfo._files.data(),
		dwSize,
		&dwReturnedByte);
	if (ec)
	{
		/** �� ������� ������� */
		return ec;
	}

	if (dwReturnedByte != dwSize)
	{
		/** ������ �������� */
		return std::error_code(ERROR_INVALID_DATA, std::system_category());
	}

	return ec;
}
//==============================================================================
template<class T>
std::error_code CTransferControl::ITransferTcpCommunication<T>::startAsyncRead(
	CTransferFileBuffer* const pFileBufferRead)
{
	if (pFileBufferRead == nullptr)
		throw std::invalid_argument("pFileBufferRead == nullptr");

	const auto ec = _pClient->startAsyncRecv(
		&pFileBufferRead->_buffer.data[0],
		pFileBufferRead->_buffer.dwSize,
		MSG_WAITALL);

	return ec;
}
//==============================================================================
template<class T>
std::error_code CTransferControl::ITransferTcpCommunication<T>::startAsyncWrite(
	CTransferFileBuffer* const pFileBufferWrite)
{
	if (pFileBufferWrite == nullptr)
		throw std::invalid_argument("pFileBufferRead == nullptr");

	const auto ec = _pClient->startAsyncSend(
		&pFileBufferWrite->_buffer.data[0],
		pFileBufferWrite->_buffer.dwSize);

	return ec;
}
//==============================================================================
template<class T>
std::error_code CTransferControl::ITransferTcpCommunication<T>::operationFile(
	CTransferFile* const pTransferFile)
{
	#pragma warning(disable: 26493)
	try
	{
		if (pTransferFile == nullptr)
			throw std::invalid_argument("pTransferFile == nullptr");
	
		DWORD dwReturnedByte = 0;
		DWORD dwError = pTransferFile->_ec.value();
		auto ec = _pClient->startSend(
			(PBYTE)&dwError,
			sizeof(dwError),
			&dwReturnedByte);
		if (ec)
		{
			/** �� ������� ��������� */
			return ec;
		}
		if (dwReturnedByte != sizeof(dwError))
		{
			/** ������ �������� */
			return std::error_code(ERROR_INVALID_DATA, std::system_category());
		}
		ec = _pClient->startRecv(
			(PBYTE)&dwError,
			sizeof(dwError),
			&dwReturnedByte,
			MSG_WAITALL);
		if (ec)
		{
			/** �� ������� ������� */
			return ec;
		}

		if (dwReturnedByte != sizeof(dwError))
		{
			/** ������ �������� */
			return std::error_code(ERROR_INVALID_DATA, std::system_category());
		}

		return std::error_code(dwError, std::system_category());
	}
	catch (const std::exception& ex)
	{
		if (pTransferFile != nullptr)
		{
			pTransferFile->_pTransferRequest->_pTransferControl->_pIocp->log(
				wname::logger::EMessageType::critical, ex);
		}

		throw;
	}	
}
//==============================================================================
template<class T>
void CTransferControl::ITransferTcpCommunication<T>::asyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(dwReturnSize);

	assert(bufferRecv != nullptr);
	const auto pBufferInfo =
		CONTAINING_RECORD(bufferRecv, CTransferFileBuffer::SBufferInfo, data);

	assert(pBufferInfo != nullptr);
	const auto pFileBufferRecv =
		CONTAINING_RECORD(pBufferInfo, CTransferFileBuffer, _buffer);

	assert(pFileBufferRecv != nullptr);
	pFileBufferRecv->_pTransferFile->asyncReadComplitionHandler(pFileBufferRecv, ec);
}
//==============================================================================
template<class T>
void CTransferControl::ITransferTcpCommunication<T>::asyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	UNREFERENCED_PARAMETER(dwReturnSize);

	assert(bufferSend != nullptr);
	const auto pBufferInfo =
		CONTAINING_RECORD(bufferSend, CTransferFileBuffer::SBufferInfo, data);

	assert(pBufferInfo != nullptr);
	const auto pFileBufferSend =
		CONTAINING_RECORD(pBufferInfo, CTransferFileBuffer, _buffer);

	assert(pFileBufferSend != nullptr);
	pFileBufferSend->_pTransferFile->asyncWriteComplitionHandler(pFileBufferSend, ec);
}
//==============================================================================