#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferControl(
	const SAccessFlag& sAccessFlag,
	const std::filesystem::path directoryPath,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp) :
	_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		throw std::invalid_argument("_pIocp == nullptr");
	}

	try
	{
		auto path = directoryPath.lexically_normal();
		_source = std::make_shared<CTransferDirectory>(
			this, sAccessFlag, path);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
CTransferControl::CTransferControl(
	const SAccessFlag& sAccessFlag,
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp)
	:_pIocp(pIocp)
{
	if (_pIocp == nullptr)
	{
		throw std::invalid_argument("_pIocp == nullptr");
	}

	try
	{
		_source = std::make_shared<CTransferTcpClient>(
			this, sAccessFlag, strIp, wPort);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::addRecipient(
	const SAccessFlag& sAccessFlag,
	const std::filesystem::path directoryPath)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		auto path = directoryPath.lexically_normal();
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		if (_recipients.end() != _recipients.find(path))
		{
			/** такой путь уже есть */
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());
		}
	
		_recipients[path] = std::make_shared<CTransferDirectory>(
			this, sAccessFlag, path);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}

	return std::error_code();
}
//==============================================================================
std::error_code CTransferControl::addRecipient(
	const SAccessFlag& sAccessFlag,
	const std::string strIp,
	const WORD wPort)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		auto wStrInfo =
			std::wstring(strIp.begin(), strIp.end()) + L":" + std::to_wstring(wPort);

		wname::cs::CCriticalSectionScoped lock(_csCounter);

		if (_recipients.end() != _recipients.find(wStrInfo))
		{
			/** такой путь уже есть */
			return std::error_code(ERROR_ALREADY_EXISTS, std::system_category());
		}

		_recipients[wStrInfo] = std::make_shared<CTransferTcpServer>(
			this, sAccessFlag, strIp, wPort);
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}

	return std::error_code();
}
//==============================================================================
std::error_code CTransferControl::request(
	const ERequest eRequest,
	const std::filesystem::path requestPath)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::shared_ptr<ITransferMember> pMemberRecipient;
		auto requestPathNormal = requestPath.lexically_normal().wstring();
		std::filesystem::path path;
		{
			wname::cs::CCriticalSectionScoped lock(_csCounter);
			for (const auto& it : _recipients)
			{
				if (it.first.find_first_of(requestPathNormal) != 0)
				{
					/** смотрим дальше */
					continue;
				}

				path = requestPathNormal.substr(it.first.length());
				pMemberRecipient = it.second;
				break;
			}
		}

		if (pMemberRecipient == nullptr)
		{
			/** такой путь никто не поддерживает */
			return std::error_code(ERROR_FILE_NOT_FOUND, std::system_category());
		}

		return request(eRequest, path, pMemberRecipient.get());
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::request(
	const ERequest eRequest,
	const std::filesystem::path requestPath,
	ITransferMember* const pMember)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());

	try
	{
		std::shared_ptr<CTransferRequest> pRequest;
		{
			wname::cs::CCriticalSectionScoped lock(_csCounter);
			switch (eRequest)
			{
			case ERequest::eCopy:
			{
				pRequest = std::make_shared<CTransferRequest>(
					this, eRequest, requestPath,
					_source.get(),
					pMember);
				break;
			}
			case ERequest::ePaste:
			{
				pRequest = std::make_shared<CTransferRequest>(
					this, eRequest, requestPath,
					pMember,
					_source.get());
				break;
			}
			default:
				/** аргумент запроса кривой */
				return std::error_code(ERROR_INVALID_PARAMETER, std::system_category());
			}

			/** добавляем запрос в список */
			_requests[pRequest.get()] = pRequest;
		}

		if (pRequest == nullptr)
		{
			/** такой путь никто не поддерживает */
			return std::error_code(ERROR_FILE_NOT_FOUND, std::system_category());
		}

		const auto ec = pRequest->request();
		{
			wname::cs::CCriticalSectionScoped lock(_csCounter);

			/** убираем из списка */
			_requests.erase(pRequest.get());
		}

		pRequest.reset();
		return ec;
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTransferControl::progressRequestHandler(
	const EProgressRequest eProgressRequest,
	const CTransferRequest& transferRequest) noexcept
{
	UNREFERENCED_PARAMETER(eProgressRequest);
	UNREFERENCED_PARAMETER(transferRequest);
}
//==============================================================================
void CTransferControl::progressTransferFileHandler(
	const EProgressTransferFileHandler eProgressTransferFileHandler,
	const CTransferRequest& transferRequest,
	const CTransferFile& transferFile) noexcept
{
	UNREFERENCED_PARAMETER(eProgressTransferFileHandler);
	UNREFERENCED_PARAMETER(transferRequest);
	UNREFERENCED_PARAMETER(transferFile);
}
//==============================================================================
void CTransferControl::release(
	const bool bIsWait) noexcept
{
	__super::release(false);

	std::shared_ptr<ITransferMember> source;
	std::map<std::wstring, std::shared_ptr<ITransferMember>> recipients;
	std::unordered_map<CTransferRequest*, std::shared_ptr<CTransferRequest>> requests;
	{
		/** отдельная область видимости на закрытие */
		wname::cs::CCriticalSectionScoped lock(_csCounter);

		source = std::move(_source);
		recipients = std::move(_recipients);
		requests = std::move(_requests);
	}

	requests.clear();
	recipients.clear();
	source.reset();
	
	if (bIsWait)
	{
		__super::release(bIsWait);
	}
}
//==============================================================================
CTransferControl::~CTransferControl()
{
	release(true);
}
//==============================================================================