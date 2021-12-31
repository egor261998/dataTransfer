#pragma once

/** экспорт */
#ifdef _WINDLL
#define DATATRANSFER __declspec(dllexport)
#else
#define DATATRANSFER 
#endif

/** пространство имен */
#define _DATATRANSFER_BEGIN namespace datatransfer {
#define _DATATRANSFER_END }

/** библиотека WNAME */
#include "../wname/wname/wname.h"

/** тестирование скорости */
#include "networktest/CNetworkTest.h"
#include "networktest/INetworkTestStatistic.h"
#include "networktest/CTcpServerNetworkTest.h"
#include "networktest/CTcpClientNetworkTest.h"
#include "networktest/CTcpConnectedClientNetworkTest.h"