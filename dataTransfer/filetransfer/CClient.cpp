#include "../stdafx.h"

using datatransfer::filetransfer::CTransferControl;

//==============================================================================
CTransferControl::CTransferTcpClient::CClient::CClient(
	CTransferTcpClient* const pTransferTcpClient,
	const std::string strIp,
	const WORD wPort,
	const std::shared_ptr<wname::io::iocp::CIocp>& pIocp) :
	wname::network::CTcpClient(strIp, wPort, pIocp),
	ITransferTcpCommunication(this),
	_pTransferTcpClient(pTransferTcpClient)
{
	try
	{
		if (_pTransferTcpClient == nullptr)
		{
			throw std::invalid_argument("_pTransferTcpClient == nullptr");
		}

		/** вешаем ссылку на родительный объект */
		if (!_pTransferTcpClient->startOperation())
		{
			throw std::logic_error("Is not start");
		}
	}
	catch (const std::exception& ex)
	{
		_pIocp->log(wname::logger::EMessageType::critical, ex);
		throw;
	}
}
//==============================================================================
void CTransferControl::CTransferTcpClient::CClient::clientAsyncRecvComplitionHandler(
	const PBYTE bufferRecv,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	ITransferTcpCommunication::asyncRecvComplitionHandler(
		bufferRecv, dwReturnSize, ec);
}
//==============================================================================
void CTransferControl::CTransferTcpClient::CClient::clientAsyncSendComplitionHandler(
	const PBYTE bufferSend,
	const DWORD dwReturnSize,
	const std::error_code ec) noexcept
{
	ITransferTcpCommunication::asyncSendComplitionHandler(
		bufferSend, dwReturnSize, ec);
}
//==============================================================================
void CTransferControl::CTransferTcpClient::CClient::release(
	const bool bIsWait) noexcept
{
	__super::release(bIsWait);
}
//==============================================================================
CTransferControl::CTransferTcpClient::CClient::~CClient()
{
	release(true);

	/** снимаем ссылку с родительного объекта */
	_pTransferTcpClient->endOperation();
}
//==============================================================================