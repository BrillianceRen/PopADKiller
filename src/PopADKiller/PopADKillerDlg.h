
// PopADKillerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include "ConfigManager.h"
#include <vector>

class ListItem
{
public:
	ListItem(){};
	~ListItem(){};
public:
	CString csTitle;
	CString csClass;
	CString csRect;
	CString csProcessId;
	CString csProcessName;
	CString csProcessPath;
	BOOL bKeyword;
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
	afx_msg void OnBnClickedBtnShowTaskIco();
	LRESULT OnShowTaskIco(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRestoreWindow();
private:
	NOTIFYICONDATA m_nid;
	bool m_bVisible;
	bool m_bInitFinished;
	CWinThread* m_pWndThread;
	CONFIGDATA* m_pCfgData;
	std::vector<ListItem> m_vListItems;
public:
	afx_msg void OnDestroy();
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	CListCtrl m_List;
	afx_msg void OnBnClickedBtnAdditem();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	int AddItem(const ListItem* item);
	int DelItem(unsigned int nIndex);
	int DelItem(const ListItem* item);
	afx_msg void OnBnClickedBtnDeleteitem();
	void DeleteAllItems();

	BOOL HandleItem(const ListItem* item);
	afx_msg void OnClose();
	int InitConfig();
};
