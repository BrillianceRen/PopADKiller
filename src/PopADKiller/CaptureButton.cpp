// CaptureButton.cpp : ʵ���ļ�
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



// CCaptureButton ��Ϣ�������

void CCaptureButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_bLKeyDown = TRUE;
	SetCapture();

	if(m_pParent)
		m_pParent->SendMessage(WM_CAPTURE_LBTNDOWN, 0, 0);
	CButton::OnLButtonDown(nFlags, point);
}


void CCaptureButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_bLKeyDown = FALSE;
	ReleaseCapture();

	if(m_pParent)
		m_pParent->SendMessage(WM_CAPTURE_LBTNUP, 0, 0);
	CButton::OnLButtonUp(nFlags, point);
}

void CCaptureButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(m_bLKeyDown)
	{
		//HCURSOR hCur = LoadCursor(NULL, IDC_CROSS);	//ϵͳ�Դ�"ʮ��"���
		HCURSOR hCur = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR1));
		::SetCursor(hCur);
	}

	if(m_pParent)
		m_pParent->SendMessage(WM_CAPTURE_MOUSEMOVE, 0, 0);
	CButton::OnMouseMove(nFlags, point);
}

void CCaptureButton::PreSubclassWindow()
{
	// TODO: �ڴ����ר�ô����/����û���

	CButton::PreSubclassWindow();
}


void CCaptureButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  ������Ĵ����Ի���ָ����
	//���ư�ť���
	UINT uStyle = DFCS_BUTTONPUSH;
	//�Ƿ���ȥ�ˣ�
// 	if (lpDrawItemStruct->itemState & ODS_SELECTED)
// 		uStyle |= DFCS_PUSHED;
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	dc.SetBkMode(TRANSPARENT);
//	dc.DrawFrameControl(&lpDrawItemStruct->rcItem, DFC_BUTTON, uStyle);
	//�������
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
	//�Ƿ�õ�����
	//if(lpDrawItemStruct->itemState & ODS_FOCUS)
	//{
	//	//�����
	//	CRect rtFocus = lpDrawItemStruct->rcItem;
	//	rtFocus.DeflateRect(3, 3);
	//	dc.DrawFocusRect(&rtFocus);
	//}
}
