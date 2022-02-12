#pragma once

/** ������� */
#ifdef _WINDLL
#define DATATRANSFER __declspec(dllexport)
#else
#define DATATRANSFER 
#endif

/** ������������ ���� */
#define _DATATRANSFER_BEGIN namespace datatransfer {
#define _DATATRANSFER_END }

/** ���������� WNAME */
#include "../wname/wname/wname.h"

/** ������������ �������� */
#include "networktest/CNetworkTest.h"
#include "networktest/INetworkTestStatistic.h"
#include "networktest/CTcpServerNetworkTest.h"
#include "networktest/CTcpClientNetworkTest.h"
#include "networktest/CTcpConnectedClientNetworkTest.h"

/** �������� ������ */
#include "filetransfer/CTransferControl.h"
#include "filetransfer/ITransferMember.h"
#include "filetransfer/CTransferRequestInfo.h"
#include "filetransfer/CTransferRequest.h"
#include "filetransfer/CTransferFile.h"
#include "filetransfer/CTransferFileBuffer.h"
#include "filetransfer/CTransferDirectory.h"
#include "filetransfer/CTransferDirectoryFile.h"

#include "filetransfer/ITransferTcpCommunication.h"
#include "filetransfer/CTransferTcpClient.h"
#include "filetransfer/CTransferTcpServer.h"
#include "filetransfer/CClient.h"
#include "filetransfer/CTcpServerFromMember.h"
#include "filetransfer/CClientFromServer.h"