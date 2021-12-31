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