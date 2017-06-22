
#include "stdafx.h"

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "A2v10.Designer.h"
#endif

#include "formitem.h"
#include "a2formdoc.h"

#include "mainfrm.h"
#include "recttracker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CA2FormDocument, CDocument)

CA2FormDocument::CA2FormDocument()
{
}

// virtual 
CA2FormDocument::~CA2FormDocument()
{

}

BEGIN_MESSAGE_MAP(CA2FormDocument, CDocument)
END_MESSAGE_MAP()

// virtual 
BOOL CA2FormDocument::OnNewDocument()
{
	return __super::OnNewDocument();
}

// virtual 
void CA2FormDocument::OnCloseDocument()
{
	__super::OnCloseDocument();
}

// virtual 
BOOL CA2FormDocument::OnOpenDocument(LPCTSTR lpszPathName)
{
	return __super::OnOpenDocument(lpszPathName);
}

// virtual 
BOOL CA2FormDocument::OnSaveDocument(LPCTSTR lpszPathName)
{
	return __super::OnSaveDocument(lpszPathName);
}

//virtual 
BOOL CA2FormDocument::CanCloseFrame(CFrameWnd* pFrame)
{
	return __super::CanCloseFrame(pFrame);
}

// virtual 
void CA2FormDocument::Serialize(CArchive& ar)
{
	ATLASSERT(FALSE);
}

// virtual 
void CA2FormDocument::SetModifiedFlag(BOOL bModified /*= TRUE*/)
{
	__super::SetModifiedFlag(bModified);
}

void CA2FormDocument::DrawContent(RENDER_INFO& ri)
{
	//ATLASSERT(m_pRoot);

	ri.pDC->SetBkMode(TRANSPARENT);
	HGDIOBJ pOldFont = ri.pDC->SelectObject(CTheme::GetUIFont(CTheme::FontUiDefault));

	//m_pRoot->Draw(ri);
	//DrawSelection(ri);

	ri.pDC->SelectObject(pOldFont);
}
