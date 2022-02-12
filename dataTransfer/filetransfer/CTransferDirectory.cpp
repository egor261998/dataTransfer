#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferDirectory::CTransferDirectory(
	CTransferControl* const pTransferControl,
	const SAccessFlag& sAccessFlag,
	const std::filesystem::path directoryPath) :
	ITransferMember(pTransferControl, sAccessFlag)
{
	try
	{
		_directoryPath = directoryPath;
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::openMember(
	CTransferRequest* const pTransferRequest)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		if (_transferActive.find(pTransferRequest) != _transferActive.end())
		{
			/** такой запрос уже есть */
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());
		}

		_transferActive[pTransferRequest] = FileList();
		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::getRequestInfo(
	CTransferRequestInfo& requestInfo)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		std::filesystem::path requestPath;
		requestPath /= _directoryPath;
		requestPath /= requestInfo._pTransferRequest->_requestPath;

		auto directory = wname::filesystem::CDirectory(
			requestPath, _pTransferControl->_pIocp);

		auto ec = directory.createDirectory(
			true, false,
			GENERIC_READ, FILE_SHARE_READ,
			OPEN_EXISTING);
		if (ec)
			return ec;

		const auto fileList = directory.getFileList(true, _directoryPath);
		directory.close();
		requestInfo._files.resize(fileList.size());
		auto itFileInfo = requestInfo._files.begin();
		for (const auto& it : fileList)
		{
			wcscpy_s(&itFileInfo->wszFilePath[0],
				sizeof(itFileInfo->wszFilePath) / sizeof(wchar_t) - 1,
				it.c_str());
			itFileInfo->uSize = std::filesystem::file_size(_directoryPath / it);

			itFileInfo++;
		}

		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::putRequestInfo(
	const CTransferRequestInfo& requestInfo) noexcept
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	/** мы крайние, не куда ложить */
	UNREFERENCED_PARAMETER(requestInfo);
	return std::error_code();
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::openFile(
	CTransferFile* const pTransferFile)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		std::filesystem::path filePath;
		filePath /= _directoryPath;
		filePath /= pTransferFile->_fileInfo.wszFilePath;

		auto pFile = std::make_shared<CTransferDirectoryFile>(
			pTransferFile,
			filePath,
			_pTransferControl->_pIocp);

		auto& pTransfer = _transferActive.at(pTransferFile->_pTransferRequest);
		if (pTransfer.find(pTransferFile) != pTransfer.end())
		{
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());
		}

		const auto ec = pFile->createFile(
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_DELETE,
			OPEN_EXISTING);

		if (ec)
			return ec;

		pTransfer[pTransferFile] = std::move(pFile);
		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::createFile(
	CTransferFile* const pTransferFile)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		std::filesystem::path filePath;
		filePath /= _directoryPath;
		filePath /= pTransferFile->_fileInfo.wszFilePath;

		auto pFile = std::make_shared<CTransferDirectoryFile>(
			pTransferFile,
			filePath,
			_pTransferControl->_pIocp);

		auto& pTransfer = _transferActive.at(pTransferFile->_pTransferRequest);
		if (pTransfer.find(pTransferFile) != pTransfer.end())
		{
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());
		}

		const auto ec = pFile->createFile(
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_DELETE,
			CREATE_ALWAYS);

		if (ec)
			return ec;

		pTransfer[pTransferFile] = std::move(pFile);
		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::startAsyncRead(
	CTransferFileBuffer* const pFileBufferRead)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::shared_ptr<CTransferDirectoryFile> pFile;
		{
			wname::cs::CCriticalSectionScoped lock(_csCounter);

			pFile = _transferActive.at(
				pFileBufferRead->_pTransferFile->_pTransferRequest)
				.at(pFileBufferRead->_pTransferFile);
		}
		const auto ec = pFile->asyncReadFile(pFileBufferRead);

		if (ec)
			return ec;

		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::startAsyncWrite(
	CTransferFileBuffer* const pFileBufferWrite)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::shared_ptr<CTransferDirectoryFile> pFile;
		{
			wname::cs::CCriticalSectionScoped lock(_csCounter);

			pFile = _transferActive.at(
				pFileBufferWrite->_pTransferFile->_pTransferRequest)
				.at(pFileBufferWrite->_pTransferFile);
		}
		const auto ec = pFile->asyncWriteFile(pFileBufferWrite);

		if (ec)
			return ec;

		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::closeFile(
	CTransferFile* const pTransferFile) noexcept
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		if (_transferActive.at(pTransferFile->_pTransferRequest).erase(pTransferFile) == 0)
			return std::error_code(ERROR_NOT_FOUND, std::system_category());

		return std::error_code();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		return std::error_code(ERROR_INVALID_FUNCTION, std::system_category());
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferDirectory::closeMember(
	CTransferRequest* const pTransferRequest) noexcept
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	wname::cs::CCriticalSectionScoped lock(_csCounter);

	if (_transferActive.erase(pTransferRequest) == 0)
		return std::error_code(ERROR_NOT_FOUND, std::system_category());

	return std::error_code();
}
//==============================================================================
void CTransferControl::CTransferDirectory::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferDirectory::~CTransferDirectory()
{
	release(true);
}
//==============================================================================