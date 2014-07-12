// CaptureButton.cpp : 实现文件
//

#include "stdafx.h"
#include "PopADKiller.h"
#include "CaptureButton.h"


// CCaptureButton

IMPLEMENT_DYNAMIC(CCaptureButton, CButton)

CCaptureButton::CCaptureButton()
{
	m_pParent = NULL;
	m_bLKeyDown = FALSE;
}

CCaptureButton::~CCaptureButton()
{
}


BEGIN_MESSAGE_MAP(CCaptureButton, CButton)
//	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CCaptureButton 消息处理程序

void CCaptureButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bLKeyDown = TRUE;
	SetCapture();

	if(m_pParent)
		m_pParent->SendMessage(WM_CAPTURE_LBTNDOWN, 0, 0);
	CButton::OnLButtonDown(nFlags, point);
}


void CCaptureButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bLKeyDown = FALSE;
	ReleaseCapture();

	if(m_pParent)
		m_pParent->SendMessage(WM_CAPTURE_LBTNUP, 0, 0);
	CButton::OnLButtonUp(nFlags, point);
}

void CCaptureButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_bLKeyDown)
	{
		//HCURSOR hCur = LoadCursor(NULL, IDC_CROSS);	//系统自带"十字"光标
		HCURSOR hCur = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR1));
		::SetCursor(hCur);
	}

	if(m_pParent)
		m_pParent->SendMessage(WM_CAPTURE_MOUSEMOVE, 0, 0);
	CButton::OnMouseMove(nFlags, point);
}

void CCaptureButton::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	CButton::PreSubclassWindow();
}


void CCaptureButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  添加您的代码以绘制指定项
	//绘制按钮框架
	UINT uStyle = DFCS_BUTTONPUSH;
	//是否按下去了？
// 	if (lpDrawItemStruct->itemState & ODS_SELECTED)
// 		uStyle |= DFCS_PUSHED;
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	dc.SetBkMode(TRANSPARENT);
//	dc.DrawFrameControl(&lpDrawItemStruct->rcItem, DFC_BUTTON, uStyle);
	//输出文字
	/*dc.SelectObject(&m_Font);
	dc.SetTextColor(RGB(0, 0, 255));
	dc.SetBkMode(TRANSPARENT);
	CString sText;
	m_HelloCFan.GetWindowText(sText);
	dc.TextOut(lpDrawItemStruct->rcItem.left + 20, lpDrawItemStruct->rcItem.top + 20, sText);*/
	CBitmap CBmp;
	BITMAP Bmp;
	BOOL bRes = CBmp.LoadBitmap(IDB_BITMAP1);
	CBmp.GetBitmap(&Bmp);
	CRect rtBtn;
	GetClientRect(&rtBtn);
	CDC memDc;
	memDc.CreateCompatibleDC(&dc);
	CBitmap* oldCBmp = memDc.SelectObject(&CBmp);
	//dc.StretchBlt(0, 0, rtBtn.Width(), rtBtn.Height(), &memDc, 0, 0, Bmp.bmWidth, Bmp.bmHeight, SRCCOPY);
	dc.TransparentBlt(0, 0, rtBtn.Width(), rtBtn.Height(), &memDc, 0, 0, Bmp.bmWidth, Bmp.bmHeight, RGB(255,255,255));
	memDc.SelectObject(oldCBmp);
	//是否得到焦点
	//if(lpDrawItemStruct->itemState & ODS_FOCUS)
	//{
	//	//画虚框
	//	CRect rtFocus = lpDrawItemStruct->rcItem;
	//	rtFocus.DeflateRect(3, 3);
	//	dc.DrawFocusRect(&rtFocus);
	//}
}
