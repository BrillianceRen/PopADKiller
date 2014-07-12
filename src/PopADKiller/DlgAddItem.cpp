// DlgAddItem.cpp : 实现文件
//

#include "stdafx.h"
#include "PopADKiller.h"
#include "DlgAddItem.h"
#include "afxdialogex.h"
#include <locale.h>
#include <Psapi.h>

#include "PopADKillerDlg.h"

// CDlgAddItem 对话框

IMPLEMENT_DYNAMIC(CDlgAddItem, CDialogEx)

CDlgAddItem::CDlgAddItem(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgAddItem::IDD, pParent)
	, m_csTitle(_T(""))
	, m_csClass(_T(""))
	, m_csRect(_T(""))
	, m_csProcessId(_T(""))
	, m_csProcessName(_T(""))
	, m_csProcessPath(_T(""))
{
	m_pParent = (CPopADKillerDlg*)pParent;
}

CDlgAddItem::~CDlgAddItem()
{
}

void CDlgAddItem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_csTitle);
	DDX_Text(pDX, IDC_EDIT_CLASS, m_csClass);
	DDX_Text(pDX, IDC_EDIT_RECT, m_csRect);
	DDX_Text(pDX, IDC_EDIT_PROCESSID, m_csProcessId);
	DDX_Text(pDX, IDC_EDIT_PROCESSNAME, m_csProcessName);
	DDX_Control(pDX, IDC_VBTN_CAPTURE, m_CaptureButton);
	DDX_Text(pDX, IDC_EDIT_PROCESSPATH, m_csProcessPath);
}


BEGIN_MESSAGE_MAP(CDlgAddItem, CDialogEx)
	ON_MESSAGE(WM_CAPTURE_LBTNDOWN, OnCaptureLBtnDown)
	ON_MESSAGE(WM_CAPTURE_LBTNUP, OnCaptureLBtnUp)
	ON_MESSAGE(WM_CAPTURE_MOUSEMOVE, OnCaptureMouseMove)
	ON_BN_CLICKED(IDC_BTN_OK, &CDlgAddItem::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCLE, &CDlgAddItem::OnBnClickedBtnCancle)
END_MESSAGE_MAP()


// CDlgAddItem 消息处理程序
BOOL CDlgAddItem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
 	//CRect rt;
 	//GetDlgItem(IDC_VBTN_CAPTURE)->GetWindowRect(&rt);
 	//ScreenToClient(&rt);

	//m_pCaptureButton = new CCaptureButton(this);
	//m_pCaptureButton->Create(_T("+"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rt, this, IDC_BTN_CAPTURE);
	//m_pCaptureButton->ShowWindow(SW_SHOW);

	//setlocale(LC_ALL, "chs");	//解决TRACE中文乱码
	m_CaptureButton.SetParent(this);
	m_CaptureButton.ModifyStyle(0, BS_OWNERDRAW);
	m_CaptureButton.ShowWindow(SW_SHOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


LRESULT CDlgAddItem::OnCaptureLBtnDown( WPARAM wParam, LPARAM lParam )
{
	CString csText;
	csText.Format(_T("左键按下\r\n"));
	TRACE(csText);
	//m_csOutput = m_csOutput + csText;
	//UpdateData(FALSE);

	return 0;
}

LRESULT CDlgAddItem::OnCaptureLBtnUp( WPARAM wParam, LPARAM lParam )
{
	// 根据坐标获取窗体 [7/10/2014 Brilliance]
	POINT pot;
	GetCursorPos(&pot);
	CWnd* pWnd = WindowFromPoint(pot);
	// 获取标题 [7/10/2014 Brilliance]
	pWnd->GetWindowText(m_csTitle);	
	// 获取类名 [7/10/2014 Brilliance]
	TCHAR sClass[128] = {0};
	GetClassName(pWnd->GetSafeHwnd(), sClass, sizeof(sClass)/sizeof(TCHAR));
	m_csClass = sClass;
	// 获取大小 [7/10/2014 Brilliance]
	CRect rt;
	pWnd->GetClientRect(&rt);
	ClientToScreen(&rt);
	//m_csRect.Format(_T("(%d,%d),%d*%d"), rt.left, rt.top, rt.Width(), rt.Height());
	m_csRect.Format(_T("%d,%d"), rt.Width(), rt.Height());
	// 获取进程ID [7/10/2014 Brilliance]
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(pWnd->GetSafeHwnd(), &dwProcessId);
	m_csProcessId.Format(_T("%d"), dwProcessId);
	// 获取进程名 [7/10/2014 Brilliance]
	HANDLE hProcess =  OpenProcess(/*PROCESS_ALL_ACCESS*/PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, dwProcessId);
	TCHAR sProcessName[MAX_PATH] = {0};
	TCHAR sProcessPath[MAX_PATH] = {0};
	if (NULL != hProcess )
	{
		HMODULE hMod;
		DWORD cbNeeded;
		if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
		{
			GetModuleBaseName( hProcess, hMod, sProcessName, sizeof(sProcessName)/sizeof(TCHAR) );
			GetModuleFileName(hMod, sProcessPath, sizeof(sProcessPath)/sizeof(TCHAR));
		}
	}	
	m_csProcessName = sProcessName;
	m_csProcessPath = sProcessPath;
	// 获取线程ID [7/10/2014 Brilliance]
// 	DWORD dwThreadId = 0;
// 	dwThreadId = GetThreadId(pWnd->m_hWnd);
	
	CString csText;
	csText.Format(_T("左键弹起,x=%d,y=%d,Title:%s,Class:%s\r\n"), pot.x, pot.y, m_csTitle, m_csClass);
	TRACE(csText);
	//m_csOutput = m_csOutput + csText;
	//UpdateData(FALSE);

	UpdateData(FALSE);
	return 0;
}

LRESULT CDlgAddItem::OnCaptureMouseMove( WPARAM wParam, LPARAM lParam )
{
	return 0;
}


void CDlgAddItem::OnBnClickedBtnOk()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bKeyword = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck()==1;

	UpdateData(TRUE);
	if(m_csTitle.IsEmpty())
	{
		MessageBox(_T("标题不能为空"));
		return;
	}

	ListItem* item = new ListItem();
	item->csTitle = m_csTitle;
	item->csClass = m_csClass;
	item->csRect = m_csRect;
	item->csProcessId = m_csProcessId;
	item->csProcessName = m_csProcessName;
	item->csProcessPath = m_csProcessPath;
	item->bKeyword = bKeyword;

	m_pParent->AddItem(item);
	OnOK();
}


void CDlgAddItem::OnBnClickedBtnCancle()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
