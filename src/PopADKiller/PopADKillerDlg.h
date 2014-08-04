
// PopADKillerDlg.h : 头文件
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
	unsigned long lLastKillTime;	//用于防止两次发送关闭请求间隔太短
};

// CPopADKillerDlg 对话框
class CPopADKillerDlg : public CDialogEx
{
// 构造
public:
	CPopADKillerDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CPopADKillerDlg();

// 对话框数据
	enum { IDD = IDD_PopADKiller_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	bool m_bMiniStart;	// 启动程序最小化到系统托盘
	bool m_bMini;	// 正常运行时最小化状态
	bool m_bInitFinished;	//为防止未初始化完成OnSize调用控件对象造成崩溃
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
	// 启动程序不显示界面,最小化到系统托盘
	CButton m_cbMiniStart;
	afx_msg void OnBnClickedCheck1();
};
