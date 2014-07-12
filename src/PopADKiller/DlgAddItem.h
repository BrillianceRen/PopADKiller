#pragma once

#include "capturebutton.h"

// CDlgAddItem �Ի���
class CPopADKillerDlg;
class CDlgAddItem : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAddItem)

public:
	CDlgAddItem(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAddItem();

// �Ի�������
	enum { IDD = IDD_ADDITEM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	CCaptureButton m_CaptureButton;
	CPopADKillerDlg* m_pParent;
public:
	LRESULT OnCaptureLBtnDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnCaptureLBtnUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnCaptureMouseMove(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	CString m_csTitle;
	CString m_csClass;
	CString m_csRect;
	CString m_csProcessId;
	CString m_csProcessName;
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancle();
	CString m_csProcessPath;
};
