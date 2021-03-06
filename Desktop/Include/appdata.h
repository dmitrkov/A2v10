// Copyright � 2008-2017 Alex Kukhtin. All rights reserved.

#pragma once


#undef AFX_DATA
#define AFX_DATA AFX_BASE_DATA

class CAppData
{
	// declaration only
	CAppData(void); 
	~CAppData(void); 
public:
	enum Lang {
		LangUk = 0,
		LangEn = 1,
		LangRu = 2
	};

	static void SetDebug(bool bDebug = true);
	static bool IsDebug();

	static void Trace(TRACE_TYPE tt, TRACE_CATEGORY tc, LPCWSTR szFile, LPCWSTR szMsg, va_list args);

	static void TraceINFO(TRACE_CATEGORY tc, LPCWSTR szFile, LPCWSTR szMsg, ...);
	static void TraceWARNING(TRACE_CATEGORY tc, LPCWSTR szFile, LPCWSTR szMsg, ...);
	static void TraceERROR(TRACE_CATEGORY tc, LPCWSTR szFile, LPCWSTR szMsg, ...);
	static void ClearTrace();

	static int GetCurrentUILang();
	static LPCWSTR GetCurrentUILangCode();
	static int GetProfileUiLang();
	static void SetProfileUiLang(int nLang);
};