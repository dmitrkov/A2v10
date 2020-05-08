// Copyright � 2015-2020 Alex Kukhtin. All rights reserved.

#include "pch.h"
#include "posterm.h"
#include "commanddata.h"
#include "command.h"
#include "equipmentbase.h"
#include "fiscalprinter.h"
#include "fiscalprinterImpl.h"
#include "acqterminal.h"
#include "errors.h"

const size_t CMD_MAX_LEN = 64;

//static 
PosCommand::COMMAND_BIND PosCommand::_binded_commands[] = 
{
	{L"nullReceipt",    &PosCommand::NullReceipt,    &PosCommand::NullReceiptData},
	{L"xReport",        &PosCommand::XReport,        nullptr},
	{L"zReport",        &PosCommand::ZReport,        nullptr},
	{L"printReceipt",   &PosCommand::PrintReceipt,   &PosCommand::PrintReceiptData},
	{L"hasAcqTerminal", &PosCommand::HasAcqTerminal, nullptr},
	{L"acquirePayment", &PosCommand::AcquirePayment, &PosCommand::AcquirePaymentData},
	{L"serviceInOut",   &PosCommand::ServiceInOut,   &PosCommand::ServiceInOutData},
	{L"connect",        &PosCommand::Connect,        &PosCommand::ConnectData},
	{nullptr, nullptr}
};

PosCommand::PosCommand()
{
}

std::wstring PosCommand::ExecuteCommand()
{
	FiscalPrinter* pPrinter = FiscalPrinter::FindPrinter(_id.c_str());
	if (pPrinter == nullptr)
		throw EQUIPException(FP_E_DEVICE_NOT_FOUND);
	return ExecuteCommandInt(pPrinter);
}

std::wstring PosCommand::ExecuteConnectCommand()
{
	return ExecuteCommandInt(nullptr);
}

std::wstring PosCommand::ExecuteCommandInt(FiscalPrinter* pPrinter)
{
	PosCommand::COMMAND_BIND* entry = _binded_commands;
	while (entry && entry->_name) {
		if (_command == entry->_name) {
			auto func = entry->_func;
			return (this->*entry->_func)(pPrinter, _data.get());
		}
		entry++;
	}
	throw EQUIPException(FP_E_COMMAND_NOT_FOUND);
}

// virtual 
JsonTarget* PosCommand::CreateObject(const wchar_t* szName) 
{
	if (wcsncmp(szName, L"data", CMD_MAX_LEN) == 0) {
		PosCommand::COMMAND_BIND* entry = _binded_commands;
		while (entry && entry->_name) {
			if (_command == entry->_name) {
				auto func = entry->_createData;
				if (func != nullptr) {
					JsonTarget* pData = (this->*entry->_createData)();
					_data.reset(pData);
					return _data.get();
				}
			}
			entry++;
		}
	}
	return _data.get();
}

std::wstring PosCommand::Connect(FiscalPrinter* pPrinter, JsonTarget* data)
{
	PosConnectData* pcd = dynamic_cast<PosConnectData*>(data);
	PosConnectParams prms;
	prms.model = pcd->_model.c_str();
	prms.port = pcd->_port.c_str();
	prms.baud = pcd->_baud;
	//prms.payModes = pcd->PayModes;
	//prms.payModes = pcd->PayModes;
	FiscalPrinter::Connect(prms);
	JsonObject js;
	js.Add(L"model", pcd->_model.c_str());
	js.Add(L"port", pcd->_port.c_str());
	js.Add(L"terminal", pcd->_terminal.c_str());
	return js.Value();
}

JsonTarget* PosCommand::NullReceiptData()
{
	return new PosNullReceiptData();
}

JsonTarget* PosCommand::AcquirePaymentData()
{
	return new PosAcquirePaymentData();
}

JsonTarget* PosCommand::PrintReceiptData()
{
	return new PosPrintReceiptData();
}

JsonTarget* PosCommand::ConnectData()
{
	return new PosConnectData();
}

JsonTarget* PosCommand::ServiceInOutData()
{
	return new PosServiceInOutData();
}

std::wstring PosCommand::XReport(FiscalPrinter* pPrinter, JsonTarget* data)
{
	long no = pPrinter->XReport();
	JsonObject json;
	json.Add(L"no", no);
	return json.Value();
}

std::wstring PosCommand::ZReport(FiscalPrinter* pPrinter, JsonTarget* data)
{
	ZREPORT_RESULT zrr = pPrinter->ZReport();
	JsonObject js;
	js.Add(L"no", (long) zrr.no);
	js.Add(L"zno", (long) zrr.zno);
	return js.Value();
}

std::wstring PosCommand::NullReceipt(FiscalPrinter* pPrinter, JsonTarget* data)
{
	PosNullReceiptData* pnrd = dynamic_cast<PosNullReceiptData*>(data);
	bool bOpenDrawer = false;
	if (pnrd)
		bOpenDrawer = pnrd->m_openCashDrawer;
	long no = pPrinter->NullReceipt(bOpenDrawer);
	JsonObject js;
	js.Add(L"no", no);
	return js.Value();
}

std::wstring PosCommand::PrintReceipt(FiscalPrinter* pPrinter, JsonTarget* data)
{
	PosPrintReceiptData* pprd = dynamic_cast<PosPrintReceiptData*>(data);
	long no = pPrinter->PrintReceipt(pprd);
	JsonObject js;
	js.Add(L"no", no);
	return js.Value();
}

std::wstring PosCommand::HasAcqTerminal(FiscalPrinter* pPrinter, JsonTarget* data)
{
	//TODO: acqTerm id from printer
	bool hasTerminals = AcqTerminal::HasTerminal();
	JsonObject json;
	json.Add(L"hasAcqTerminal", hasTerminals);
	return json.Value();
}

std::wstring PosCommand::AcquirePayment(FiscalPrinter* pPrinter, JsonTarget* data)
{
	//TODO: acqTerm id from printer
	PosAcquirePaymentData* pacqd = dynamic_cast<PosAcquirePaymentData*>(data);
	auto pTerminal = AcqTerminal::FindTerminal(L"");
	pTerminal->Payment(pacqd->_amount);
	// TODO: get element resposnse 
	return pTerminal->Response();
}

std::wstring PosCommand::ServiceInOut(FiscalPrinter* pPrinter, JsonTarget* data)
{
	PosServiceInOutData* siod = dynamic_cast<PosServiceInOutData*>(data);
	SERVICE_SUM_INFO info = pPrinter->ServiceInOut(siod->_out, siod->_amount, siod->_openCashDrawer);
	JsonObject js;
	js.Add(L"no", (long) info.no);
	js.Add(L"cashOnHand", info.sumOnHand);
	return js.Value();
}
