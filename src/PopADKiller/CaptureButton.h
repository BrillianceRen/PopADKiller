#pragma once

#define WM_CAPTURE_LBTNDOWN (WM_USER + 2)
#define WM_CAPTURE_LBTNUP (WM_USER + 3)
#define WM_CAPTURE_MOUSEMOVE (WM_USER + 4)
// CCaptureButton

class CCaptureButton : public CButton
{
	DECLARE_DYNAMIC(CCaptureButton)

public:
	CCaptureButton();
	virtual ~CCaptureButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
private:
	CWnd* m_pParent;
	BOOL m_bLKeyDown;	//ÊÇ·ñ°´ÏÂ×ó¼ü
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void SetParent(CWnd* val) { m_pParent = val; }
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};


