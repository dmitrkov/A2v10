﻿// Copyright © 2017-2019 Alex Kukhtin. All rights reserved.

#include "stdafx.h"
#include "A2v10.Application.h"

#include "navtabs.h"
#include "mainframe.h"
#include "workarea.h"
#include "cefclient.h"
#include "cefview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CA2SDIFrameWndBase)

BEGIN_MESSAGE_MAP(CMainFrame, CA2SDIFrameWndBase)
	ON_WM_PAINT()
	ON_MESSAGE(WM_NCHITTEST, OnNcHitTest)
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCMOUSELEAVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_CREATE()
	ON_MESSAGE(WMI_CEF_VIEW_COMMAND, OnCefViewCommand)
	ON_MESSAGE(WMI_CEF_TAB_COMMAND, OnCefTabCommand)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_WM_SYSCOMMAND()
	ON_UPDATE_COMMAND_UI_RANGE(IDM_SYS_FIRST, IDM_SYS_LAST, OnUpdateSysMenu)
	ON_COMMAND(ID_APP_TOOLS, OnAppTools)
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: m_nViewId(AFX_IDW_PANE_FIRST + 1), 
  m_captionButtons(IDMENU_TOOLS)
{
	m_navigateTabs.AddTab(EMPTYSTR, nullptr, m_nViewId);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetMenu(NULL);

	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	//EnableDocking(CBRS_ALIGN_ANY);


	// enable Visual Studio 2005 style docking window behavior
	//CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	//EnableAutoHidePanes(CBRS_ALIGN_ANY);

	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, IDR_MENU_IMAGES);
	// MFC BUG. Sets transparent color only when painting
	CMFCToolBarImages* pImages = CMFCToolBar::GetImages();
	pImages->SetTransparentColor(GetGlobalData()->clrBtnFace);

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CA2VisualManager));

	ModifyStyle(0, FWS_PREFIXTITLE);

	CRect rc;
	GetWindowRect(&rc);
	SetWindowPos(NULL, rc.left, rc.top, rc.Width(), rc.Height(), SWP_FRAMECHANGED | SWP_NOZORDER);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !__super::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	__super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif //_DEBUG



BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work
	if (!__super::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
		return FALSE;

	CWinApp* pApp = AfxGetApp();
	if (pApp->m_pMainWnd == nullptr)
		pApp->m_pMainWnd = this;

	CWnd* pActiveView = GetDlgItem(AFX_IDW_PANE_FIRST);
	CNavTab* pFirstTab = m_navigateTabs.GetTab(0);

	pFirstTab->SetHwnd(pActiveView->GetSafeHwnd());
	m_navigateTabs.SetActiveTab(pFirstTab);
	return TRUE;
}

void CMainFrame::OnFileClose()
{
	DestroyWindow();
}

void CMainFrame::CreateNewView(CEF_VIEW_INFO* pViewInfo)
{
	// примерно так
	UINT nViewId = ++m_nViewId;
	CWnd* pActiveView = GetDlgItem(AFX_IDW_PANE_FIRST);
	CCreateContext ctx;
	ctx.m_pCurrentDoc = GetActiveDocument();
	ctx.m_pNewViewClass = RUNTIME_CLASS(CCefView);
	ctx.m_pCurrentFrame = this;
	CWnd* pNewView = CreateView(&ctx, nViewId);
	ATLASSERT(pNewView);
	::SetWindowLong(pActiveView->m_hWnd, GWL_ID, nViewId);
	::SetWindowLong(pNewView->m_hWnd, GWL_ID, AFX_IDW_PANE_FIRST);

	m_navigateTabs.AddTab(pViewInfo->szUrl, pNewView->GetSafeHwnd(), nViewId);

	pActiveView->ShowWindow(SW_HIDE);
	RecalcLayout();
	// send required
	pNewView->SendMessage(WMI_CEF_VIEW_COMMAND, WMI_CEF_VIEW_COMMAND_OPEN, reinterpret_cast<LPARAM>(pViewInfo));
	SetActiveView(reinterpret_cast<CView*>(pNewView));
	pNewView->Invalidate();
	SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);
	Invalidate(TRUE);
}

LRESULT CMainFrame::OnCefTabCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == WMI_CEF_TAB_COMMAND_SELECT) {
		HWND targetHWnd = reinterpret_cast<HWND>(lParam);
		SwitchToTab(targetHWnd);
	}
	else if (wParam == WMI_CEF_TAB_COMMAND_CLOSE) {
		HWND targetHWnd = reinterpret_cast<HWND>(lParam);
		CloseTab(targetHWnd);

	}
	return 0L;
}


void CMainFrame::SwitchToTab(HWND targetHWnd)
{
	CWnd* pActiveView = GetDlgItem(AFX_IDW_PANE_FIRST);
	HWND activeHwnd = pActiveView->GetSafeHwnd();
	if (activeHwnd == targetHWnd) {
		return;
	}
	CNavTab* pActiveTab = m_navigateTabs.FindTab(activeHwnd);
	if (!pActiveTab)
		return;
	CWnd* pNewView = CWnd::FromHandle(targetHWnd);
	::SetWindowLong(pActiveView->m_hWnd, GWL_ID, pActiveTab->GetID());
	::SetWindowLong(pNewView->m_hWnd, GWL_ID, AFX_IDW_PANE_FIRST);
	pActiveView->ShowWindow(SW_HIDE);
	pNewView->ShowWindow(SW_SHOW);
	SetActiveView(reinterpret_cast<CView*>(pNewView));
	pNewView->Invalidate();
	m_navigateTabs.SetActiveTab(m_navigateTabs.FindTab(targetHWnd));
	Invalidate(TRUE);
}

void CMainFrame::CloseTab(HWND targetHWnd)
{
	CNavTab* pTab = m_navigateTabs.FindTab(targetHWnd);
	if (!pTab)
		return;
	if (m_navigateTabs.GetButtonsCount() == 1) {
		PostMessage(WM_COMMAND, ID_FILE_CLOSE);
		return;
	}
	if (!m_navigateTabs.RemoveTab(pTab, this))
		return;
	::DestroyWindow(targetHWnd);
	RecalcLayout();
	Invalidate();
}

// afx_msg
LRESULT CMainFrame::OnCefViewCommand(WPARAM wParam, LPARAM lParam)
{
	CEF_VIEW_INFO* pViewInfo = reinterpret_cast<CEF_VIEW_INFO*>(lParam);
	if (pViewInfo == nullptr)
		return 0L;
	if (wParam == WMI_CEF_VIEW_COMMAND_CREATETAB) {
		CreateNewView(pViewInfo);
	}
	else if (wParam == WMI_CEF_VIEW_COMMAND_SETTILE) {
		CWnd* pWnd = GetDlgItem(AFX_IDW_PANE_FIRST);
		if (pWnd) {
			CNavTab* pTab = m_navigateTabs.FindTab(pWnd->GetSafeHwnd());
			if (pTab && pTab->SetText(pViewInfo->szTitle)) {
				RecalcLayout();
				Invalidate();
			}
		}
	}
	return 0L;
}

// afx_msg
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_SYS_ABOUTBOX)
		PostMessage(WM_COMMAND, ID_APP_ABOUT);
	else if ((nID & 0xFFF0) == IDM_SYS_OPTIONS)
		PostMessage(WM_COMMAND, ID_TOOLS_OPTIONS);
	else if ((nID & 0xFFF0) == IDM_SYS_DEVTOOLS)
		PostMessage(WM_COMMAND, ID_SHOW_DEVTOOLS);
	else
		__super::OnSysCommand(nID, lParam);
}

// afx_msg
void CMainFrame::OnUpdateSysMenu(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

// afx_msg
void CMainFrame::OnAppTools() {
	AfxMessageBox(L"On App Tools");
}
