
// A2v10.Application.h : main header file for the A2v10.Application application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMainApp:
// See A2v10.Application.cpp for the implementation of this class
//

class CMainApp : public CA2WinApp
{
public:
	CMainApp();

protected:
	CMultiDocTemplate* m_pDocTemplate;
public:

// Overrides
public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;

// Implementation
	BOOL  m_bHiColorIcons;

	afx_msg void OnFileNewFrame();
	afx_msg void OnFileNew();
	DECLARE_MESSAGE_MAP()
};

extern CMainApp theApp;
