#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferRequest::CTransferRequest(
	CTransferControl* const pTransferControl,
	const ERequest eRequest,
	const std::filesystem::path requestPath,
	ITransferMember* const pMemberIn,
	ITransferMember* const pMemberOut):
	_eRequest(eRequest), 
	_requestPath(requestPath),
	_pTransferControl(pTransferControl),
	_pMemberIn(pMemberIn),
	_pMemberOut(pMemberOut)
{
	if (_pTransferControl == nullptr ||
		_pMemberIn == nullptr || 
		_pMemberOut == nullptr)
	{
		throw std::invalid_argument("_pTransferControl == nullptr || _pMemberIn == nullptr || _pMemberOut == nullptr");
	}

	wname::misc::CCounterScoped counterMemberIn(*_pMemberIn);
	wname::misc::CCounterScoped counterMemberOut(*_pMemberOut);
	if (!counterMemberIn.isStartOperation() ||
		!counterMemberOut.isStartOperation())
	{
		throw std::logic_error("Is not start");
	}	

	/** ��������� ������ */
	counterMemberIn.release();
	counterMemberOut.release();

	/** �������� �������� ������� */
	_pTransferControl->progressRequestHandler(EProgressRequest::eCreate, *this);
}
//==============================================================================
std::error_code CTransferControl::CTransferRequest::request()
{
	/**
	* ������ � ��������� �������� � �������� ���������� ��������� ��������.
	* @param getInfo - ���������� � ������� ��� ���������.
	* @param putInfo - ���������� � ������� ��� ����������.
	* @return - ������ ���������� �������.
	*/
	auto requestInfo = [](
		CTransferRequestInfo& getInfo,
		CTransferRequestInfo& putInfo)
	{
		try
		{
			auto ec = getInfo._pMember->getRequestInfo(getInfo);
			if (ec)
				return ec;

			ec = putInfo._pMember->putRequestInfo(getInfo);
			if (ec)
				return ec;
		}
		catch (const std::exception& ex)
		{
			getInfo._pMember->_pTransferControl->_pIocp->log(
				wname::logger::EMessageType::critical,
				ex);
			throw;
		}

		/** �������� ������ */
		return std::error_code();
	};

	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
	{
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
	}

	if (!_pMemberIn->_sAccessFlag.bIsCopy || !_pMemberOut->_sAccessFlag.bIsPaste)
	{
		/** ��� ������� ��� ������� ������� */
		return std::error_code(ERROR_ACCESS_DENIED, std::system_category());
	}

	/** ������� ������� */
	CTransferRequestInfo requestInfoIn(this, _pMemberIn);
	CTransferRequestInfo requestInfoOut(this, _pMemberOut);

	/** �������� ������ ������� � ���������� */
	_pTransferControl->progressRequestHandler(EProgressRequest::eStartRequest, *this);

	try
	{
		/** �������� ������ � ����������� �������� */
		if (const auto ec = _pMemberIn->openMember(this); ec)
		{
			return ec;
		}
		if (const auto ec = _pMemberOut->openMember(this); ec)
		{
			/** ��������� ���� ������� ��� ��� ������ */
			_pMemberIn->closeMember(this);
			return ec;
		}

		/** ��������� ������� */
		auto resultIn = std::async(std::launch::async, std::bind(
			requestInfo,
			std::ref(requestInfoIn),
			std::ref(requestInfoOut)));
		auto resultOut = std::async(std::launch::async, std::bind(
			requestInfo,
			std::ref(requestInfoOut),
			std::ref(requestInfoIn)));

		/** �������� �������� */
		resultIn.wait();
		resultOut.wait();

		/** �������� ��������� ������� � ���������� */
		_pTransferControl->progressRequestHandler(EProgressRequest::eEndRequest, *this);

		/** �������� ������ �������� */
		if (const auto ec = resultIn.get(); ec)
			return ec;
		if (const auto ec = resultOut.get(); ec)
			return ec;

		/** �������� ������ �������� ������ */
		_pTransferControl->progressRequestHandler(EProgressRequest::eStartTransfer, *this);

		for (const auto& itIn : requestInfoIn._files)
		{
			for (const auto& itOut : requestInfoOut._files)
			{
				if (_wcsicmp(&itIn.wszFilePath[0], &itOut.wszFilePath[0]) == 0 &&
					!_pMemberOut->_sAccessFlag.bIsOverrite)
				{
					/** ��� ������� ��� ������� ������� */
					return std::error_code(ERROR_ACCESS_DENIED, std::system_category());
				}
			}
			
			_files.push_back(itIn);
		}

		/** �������� ����� */
		for (const auto& it : _files)
		{
			/** �������� ����� */
			const auto ec = transferFile(it);
			if (ec)
				return ec;
		}
		
		/** �������� ��������� �������� ������ */
		_pTransferControl->progressRequestHandler(EProgressRequest::eEndTransfer, *this);

		/** ��������� ���������� �������� ��� �������
			� ��������� ������ */
		const auto ecCloseIn = _pMemberIn->closeMember(this);
		const auto ecCloseOut = _pMemberOut->closeMember(this);
		return ecCloseIn ? ecCloseIn : ecCloseOut;
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);

		_pMemberIn->closeMember(this);
		_pMemberOut->closeMember(this);
		throw;
	}
}
//==============================================================================
std::error_code CTransferControl::CTransferRequest::transferFile(
	const CTransferRequestInfo::SFileInfo& fileInfo)
{
	wname::misc::CCounterScoped counter(*this);
	if (!counter.isStartOperation())
	{
		return std::error_code(ERROR_OPERATION_ABORTED, std::system_category());
	}

	try
	{
		/** �������� ����� */
		CTransferFile transferFile(this, fileInfo);
		return transferFile.request();
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(
			wname::logger::EMessageType::critical,
			ex);
		throw;
	}
}
//==============================================================================
void CTransferControl::CTransferRequest::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferRequest::~CTransferRequest()
{
	/** ���������� ����� */
	release(true);

	/** ������� ������ � �������� */
	_pMemberIn->endOperation();
	_pMemberOut->endOperation();

	/** �������� �������� ������� */
	_pTransferControl->progressRequestHandler(EProgressRequest::eDelete, *this);
}
//==============================================================================