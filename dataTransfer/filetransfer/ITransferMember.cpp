#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::ITransferMember::ITransferMember(
	CTransferControl* const pTransferControl,
	const SAccessFlag& sAccessFlag)
	: _pTransferControl(pTransferControl), 
	_sAccessFlag(sAccessFlag)
{
	if (_pTransferControl == nullptr)
	{
		throw std::invalid_argument("_pFileTransfer == nullptr");
	}

	try
	{
		/** ������ ������ �� ����������� ������ */
		if (!_pTransferControl->startOperation())
		{
			throw std::logic_error("Is not start");
		}
	}
	catch (const std::exception& ex)
	{
		_pTransferControl->_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTransferControl::ITransferMember::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::ITransferMember::~ITransferMember()
{
	release(true);

	/** ������� ������ � ������������ ������� */
	_pTransferControl->endOperation();
}
//==============================================================================