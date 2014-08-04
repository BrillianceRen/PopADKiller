
// PopADKillerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include "ConfigManager.h"
#include <vector>
#include "afxwin.h"

class ListItem
{
public:
	ListItem()
	{
		bKeyword = false;
		lLastKillTime = 0;
	};
	~ListItem(){};
public:
	CString csTitle;
	CString csClass;
	CString csRect;
	CString csProcessId;
	CString csProcessName;
	CString csProcessPath;
	bool bKeyword;
	unsigned long lLastKillTime;	//���ڷ�ֹ���η��͹ر�������̫��
};

// CPopADKillerDlg �Ի���
class CPopADKillerDlg : public CDialogEx
{
// ����
public:
	CPopADKillerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CPopADKillerDlg();

// �Ի�������
	enum { IDD = IDD_PopADKiller_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnHandkill();
	CString m_csOutput;
	LRESULT OnTaskIco(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTaskShowWindow();
	afx_msg void OnTaskClose();
private:
	NOTIFYICONDATA m_nid;
	bool m_bMiniStart;	// ����������С����ϵͳ����
	bool m_bMini;	// ��������ʱ��С��״̬
	bool m_bInitFinished;	//Ϊ��ֹδ��ʼ�����OnSize���ÿؼ�������ɱ���
	CWinThread* m_pWndThread;
public:
	afx_msg void OnDestroy();
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	CListCtrl m_List;
	afx_msg void OnBnClickedBtnAdditem();
	int AddItem(const ListItem* item);
	int DelItem(unsigned int nIndex);
	int DelItem(const ListItem* item);
	afx_msg void OnBnClickedBtnDeleteitem();
	void DeleteAllItems();

	BOOL HandleItem(const ListItem* item);
	afx_msg void OnClose();
	int InitConfig();
	int SaveConfig();
	// ����������ʾ����,��С����ϵͳ����
	CButton m_cbMiniStart;
	afx_msg void OnBnClickedCheck1();
};
