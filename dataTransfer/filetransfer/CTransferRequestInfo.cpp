#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferRequestInfo::CTransferRequestInfo(
	CTransferRequest* const pTransferRequest,
	ITransferMember* const pMember) noexcept :
	_pTransferRequest(pTransferRequest),
	_pMember(pMember)
{ 

}
//==============================================================================