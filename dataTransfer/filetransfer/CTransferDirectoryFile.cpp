#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferDirectory::CTransferDirectoryFile::CTransferDirectoryFile(
	CTransferFile* const pTransferFile,
	const std::filesystem::path filePath,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp) :
	CFile(filePath, pIocp),
	_pTransferFile(pTransferFile) 
{ 

}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::CTransferDirectoryFile::asyncReadFile(
	CTransferFileBuffer* const pFileBufferRead)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		const auto ec = __super::startAsyncReadFile(
			&pFileBufferRead->_buffer.data[0],
			pFileBufferRead->_buffer.dwSize,
			pFileBufferRead->_buffer.offset);

		if (!ec)
			counter.release();

		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::CTransferDirectoryFile::asyncWriteFile(
	CTransferFileBuffer* const pFileBufferWrite)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		const auto ec = __super::startAsyncWriteFile(
			&pFileBufferWrite->_buffer.data[0],
			pFileBufferWrite->_buffer.dwSize,
			pFileBufferWrite->_buffer.offset);

		if (!ec)
			counter.release();

		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTransferControl::CTransferDirectory::CTransferDirectoryFile::asyncReadFileComplitionHandler(
	const PBYTE bufferRead,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	assert(bufferRead != nullptr);
	const auto pBufferInfo =
		CONTAINING_RECORD(bufferRead, CTransferFileBuffer::SBufferInfo, data);
	const auto pFileBufferRead =
		CONTAINING_RECORD(pBufferInfo, CTransferFileBuffer, _buffer);
	
	assert(pFileBufferRead->_pTransferFile == _pTransferFile);
	if (!ec)
		assert(pFileBufferRead->_buffer.dwSize == dwReturnSize);

	_pTransferFile->asyncReadComplitionHandler(pFileBufferRead, ec);
	endOperation();
}
//==============================================================================
void CTransferControl::CTransferDirectory::CTransferDirectoryFile::asyncWriteFileComplitionHandler(
	const PBYTE bufferWrite,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	assert(bufferWrite != nullptr);
	const auto pBufferInfo =
		CONTAINING_RECORD(bufferWrite, CTransferFileBuffer::SBufferInfo, data);
	const auto pFileBufferWrite =
		CONTAINING_RECORD(pBufferInfo, CTransferFileBuffer, _buffer);

	assert(pFileBufferWrite->_pTransferFile == _pTransferFile);
	if(!ec)
		assert(pFileBufferWrite->_buffer.dwSize == dwReturnSize);

	_pTransferFile->asyncWriteComplitionHandler(pFileBufferWrite, ec);
	endOperation();
}
//==============================================================================
void CTransferControl::CTransferDirectory::CTransferDirectoryFile::release(
	const bool bIsWait) noexcept
{
	close();
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferDirectory::CTransferDirectoryFile::~CTransferDirectoryFile()
{
	release(true);
}
//==============================================================================