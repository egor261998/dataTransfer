#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferFileBuffer::CTransferFileBuffer(
	CTransferFile* const pTransferFile)	:
	_pTransferFile(pTransferFile)
{
	if (_pTransferFile == nullptr)
	{
		throw std::invalid_argument("_pTransferFile == nullptr");
	}
}
//==============================================================================