
// PopADKillerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PopADKiller.h"
#include "PopADKillerDlg.h"
#include "afxdialogex.h"

#include "DlgAddItem.h"
#include <Psapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static int g_nThreadStat = 0;
// CPopADKillerDlg 对话框

#define WM_SHOWTASKICO (WM_USER + 1)	//最小化到系统托盘消息

#define IDM_RESTOREWINDOWS 1	//恢复窗体菜单ID

#define TIMER_AUTOKILL 1	//自动定时运行杀QQ弹窗

UINT AFX_CDECL ThreadProc(LPVOID pParam);

CPopADKillerDlg::CPopADKillerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPopADKillerDlg::IDD, pParent)
	, m_csOutput(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_ICO_PopADKiller);
	m_bMiniStart = false; 
	m_bMini = false;
	m_bInitFinished = false;

	m_pWndThread = NULL;
}

CPopADKillerDlg::~CPopADKillerDlg()
{
}

void CPopADKillerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	g_nThreadStat = 0;
	while (g_nThreadStat != 2)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (g_nThreadStat == 2)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	SaveConfig();
	DeleteAllItems();
	
	CDialogEx::OnClose();
}
void CPopADKillerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_csOutput);
	//	DDX_Control(pDX, IDC_BTN_CATCH, m_pCaptureButton);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_CHECK1, m_cbMiniStart);
}

BEGIN_MESSAGE_MAP(CPopADKillerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_HANDKILL, &CPopADKillerDlg::OnBnClickedBtnHandkill)
	ON_MESSAGE(WM_SHOWTASKICO, OnTaskIco)
	ON_COMMAND(IDM_RESTOREWINDOWS,OnShowWindow)
	ON_WM_DESTROY()
	//	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BTN_ADDITEM, &CPopADKillerDlg::OnBnClickedBtnAdditem)
	ON_BN_CLICKED(IDC_BTN_DELETEITEM, &CPopADKillerDlg::OnBnClickedBtnDeleteitem)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK1, &CPopADKillerDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CPopADKillerDlg 消息处理程序

BOOL CPopADKillerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_nid.cbSize  = (DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd    = this->m_hWnd;
	m_nid.uID     = IDR_ICO_PopADKiller;
	m_nid.uFlags  = NIF_ICON | NIF_MESSAGE | NIF_TIP ;
	m_nid.uCallbackMessage = WM_SHOWTASKICO;             // 自定义的消息名称
	m_nid.hIcon   = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICO_PopADKiller));

	wcscpy_s(m_nid.szTip, 128, _T("干掉QQ弹窗"));
	//strcpy(m_nid.szTip, "干掉QQ弹窗");                // 信息提示条为"服务器程序"，VS2008 UNICODE编码用wcscpy_s()函数
	Shell_NotifyIcon(NIM_ADD, &m_nid);                // 在托盘区添加图标

	//CRect rt;
	//GetClientRect(&rt);
	////列表
	//int x = 4;
	//int y = 4;
	//int w = rt.Width() - 8;
	//int h = ((rt.Height()-8)-(4+26+4+20+4))*2/3;
	//GetDlgItem(IDC_LIST1)->MoveWindow(x, y, w, h);
	////按钮
	//y += h + 4;
	//w = 60;
	//h = 26;
	//GetDlgItem(IDC_BTN_ADDITEM)->MoveWindow(x, y, w, h);	//添加按钮
	//x += w + 4;
	//w = 60;
	//GetDlgItem(IDC_BTN_DELETEITEM)->MoveWindow(x, y, w, h);		//删除按钮
	//x += w + 4;
	//w = 120;
	//GetDlgItem(IDC_BTN_SMALL)->MoveWindow(x, y, w, h);	//最小化到托盘按钮
	////GetDlgItem(IDC_BTN_HANDKILL)->ShowWindow(SW_HIDE);	//手杀按钮
	////日志
	//x = 4;
	//y += h + 4;
	//w = 60;
	//h = 20;
	//GetDlgItem(IDC_STATIC1)->MoveWindow(x, y, w, h);
	//y += h + 4;
	//w = rt.Width() - 8;
	//h = (rt.Height()-8) - y;
	//GetDlgItem(IDC_EDIT1)->MoveWindow(x, y, w, h);

	m_List.InsertColumn(0, _T("标题"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(1, _T("类名"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(2, _T("大小"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(3, _T("进程ID"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(4, _T("进程名"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(5, _T("路径"), LVCFMT_LEFT, 400);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	InitConfig();

	g_nThreadStat = 1; 
	m_pWndThread = AfxBeginThread(ThreadProc, this/*, THREAD_PRIORITY_TIME_CRITICAL*/);

	//SetTimer(TIMER_AUTOKILL, 1000, NULL);

	m_bInitFinished = true;

	m_cbMiniStart.SetCheck(m_bMiniStart);
	m_bMini = m_bMiniStart;
	if (!m_bMini)
	{
		OnShowWindow();
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPopADKillerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPopADKillerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam); // 回调函数

void CPopADKillerDlg::OnBnClickedBtnHandkill()
{
	// TODO: 在此添加控件通知处理程序代码
	EnumWindows(EnumWindowsProc, (LPARAM)this); // 枚举窗口
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) // 回调函数
{
	CPopADKillerDlg* pthis = (CPopADKillerDlg*)lParam;


	// Begin Add [7/12/2014 Brilliance]
	ListItem item;
	// 获取标题 
	TCHAR szTitle[200];
	GetWindowText(hwnd, szTitle, sizeof(szTitle) / sizeof(TCHAR)); // 获取窗口名称	
	item.csTitle = szTitle;
	// 获取类名
	TCHAR szClass[200];
	GetClassName(hwnd, szClass, sizeof(szClass) / sizeof(TCHAR)); // 窗口类
	item.csClass = szClass;	

	// 暂时不用,CPU太高 ( 罒ω罒) [7/12/2014 Brilliance]
#if 0
	// 获取大小
	CRect rt;
	GetClientRect(hwnd, &rt);
	//ClientToScreen(&rt);
	CString csRect;
	//m_csRect.Format(_T("(%d,%d),%d*%d"), rt.left, rt.top, rt.Width(), rt.Height());
	item.csRect.Format(_T("%d,%d"), rt.Width(), rt.Height());
	// 获取进程ID
	DWORD dwProcessId = 0;
	CString csProcessId;
	GetWindowThreadProcessId(hwnd, &dwProcessId);
	item.csProcessId.Format(_T("%d"), dwProcessId);
	// 获取进程名
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
	CloseHandle(hProcess);
	item.csProcessName = sProcessName;
	item.csProcessPath = sProcessPath;
#endif

	if(pthis->HandleItem(&item))
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		//输出日志
		CTime t = CTime::GetCurrentTime();
		CString csText;
		csText.Format(_T("[%s] [Title]:%s, [Class]:%s\r\n"), t.Format(_T("%H:%M:%S")), szTitle, szClass);
		CString csOutput;
		pthis->GetDlgItem(IDC_EDIT1)->GetWindowText(csOutput);
		csOutput = csOutput + csText;
		if (csOutput.GetLength() > 20000)
			csOutput = csText;
		pthis->GetDlgItem(IDC_EDIT1)->SetWindowText(csOutput);
		//pthis->UpdateData(FALSE); //工作线程内不能用UpdataData()
	}
	return TRUE;
}

//针对托盘图标的操作
LRESULT CPopADKillerDlg::OnTaskIco( WPARAM wParam, LPARAM lParam )
{
	if(wParam != IDR_ICO_PopADKiller)
		return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP:                                        // 右键起来时弹出菜单
		{

			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);                    // 得到鼠标位置
			CMenu menu;
			menu.CreatePopupMenu();                    // 声明一个弹出式菜单
			menu.AppendMenu(MF_STRING, WM_DESTROY, _T("退出"));
			menu.AppendMenu(MF_STRING, IDM_RESTOREWINDOWS, _T("恢复"));
			menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x ,lpoint->y, this);
			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
		}
		break;
	case WM_LBUTTONDBLCLK:                                 // 双击左键的处理
		{
			OnShowWindow();
		}
		break;
	}
	return 0;
}


void CPopADKillerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	Shell_NotifyIcon(NIM_DELETE, &m_nid); 
}

//显示窗体
void CPopADKillerDlg::OnShowWindow()
{
	m_bMini = false;
	CRect rt;
	GetClientRect(&rt);
	int cx = GetSystemMetrics(SM_CXFULLSCREEN/*SM_CXSCREEN*/);	//SM_CXFULLSCREEN 不包括状态栏, SM_CXSCREEN 包括状态栏
	int cy = GetSystemMetrics(SM_CYFULLSCREEN/*SM_CYSCREEN*/);	//SM_CYFULLSCREEN 不包括状态栏, SM_CYSCREEN 包括状态栏
	MoveWindow((cx - rt.Width()) / 2, (cy - rt.Height()) / 2, rt.Width(), rt.Height());
	ShowWindow(SW_SHOWNORMAL);
}


//void CPopADKillerDlg::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	switch(nIDEvent)
//	{
//	case TIMER_AUTOKILL:
//		{
//			KillTimer(nIDEvent);
//			EnumWindows(EnumWindowsProc, (LPARAM)this); // 枚举窗口,杀QQ弹窗
//			SetTimer(nIDEvent, 200, NULL);
//		}
//		break;
//	default:
//		break;
//	}
//
//	CDialogEx::OnTimer(nIDEvent);
//}


void CPopADKillerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_bInitFinished)
	{
		cx -= 8;
		cy -= 8;
		//列表
		int x = 4;
		int y = 4;
		int w = cx;
		int h = (cy-(4+26+4+20+4))*2/3;
		GetDlgItem(IDC_LIST1)->MoveWindow(x, y, w, h);
		//按钮
		y += h + 4;
		w = 60;
		h = 26;
		GetDlgItem(IDC_BTN_ADDITEM)->MoveWindow(x, y, w, h);	//添加按钮
		x += w + 4;
		w = 60;
		GetDlgItem(IDC_BTN_DELETEITEM)->MoveWindow(x, y, w, h);	//删除按钮
		x = cx - 120;
		w = 120;
		GetDlgItem(IDC_CHECK1)->MoveWindow(x, y, w, h);	//启动最小化
		//日志
		x = 4;
		y += h + 4;
		w = 60;
		h = 20;
		GetDlgItem(IDC_STATIC1)->MoveWindow(x, y, w, h);
		y += h + 4;
		w = cx;
		h = cy - y;
		GetDlgItem(IDC_EDIT1)->MoveWindow(x, y, w, h);
	}

	if(nType == SIZE_MINIMIZED)  
	{  
		ShowWindow(SW_HIDE); // 当最小化，隐藏主窗口              
	}  
}


void CPopADKillerDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (m_bMini)
		lpwndpos->flags &= ~SWP_SHOWWINDOW;	//对话框启动时隐藏

	CDialogEx::OnWindowPosChanging(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
}


void CPopADKillerDlg::OnBnClickedBtnAdditem()
{
	// TODO: 在此添加控件通知处理程序代码
	CDlgAddItem dlg(this);
	int nId = dlg.DoModal();
	if(nId == IDOK)
	{
		//Ok
		m_List.SetFocus();	
		m_List.SetItemState(0, LVNI_FOCUSED|LVIS_SELECTED, LVNI_FOCUSED|LVIS_SELECTED);

	}
	else 
	{
		//Cancel
	}
}



int CPopADKillerDlg::InitConfig()
{
	CConfigManager cfgManager(_T("config.dat"));

	CONFIGDATA cfgData;
	if (0 == cfgManager.LoadConfig(&cfgData))
	{
		m_bMiniStart = cfgData.bMiniStart;
		//读取成功
		for (unsigned int i = 0; i<cfgData.nCount; i++)
		{
			ListItem* item = new ListItem;
			item->bKeyword = cfgData.items[i].bKeyword;
			item->csTitle = cfgData.items[i].sTitle;
			item->csClass = cfgData.items[i].sClass;
			item->csRect = cfgData.items[i].sRect;
			item->csProcessId = cfgData.items[i].sProcessId;
			item->csProcessName = cfgData.items[i].sProcessName;
			item->csProcessPath = cfgData.items[i].sProcessPath;
			AddItem(item);
		}
	}
	else
	{
		//读取失败,写入默认数据
		ListItem* item = new ListItem;
		item->bKeyword = true;
		item->csTitle = _T("腾讯");
		item->csClass = _T("TXGuiFoundation");
		AddItem(item);

		item = new ListItem;
		item->bKeyword = true;
		item->csTitle = _T("京东");
		item->csClass = _T("TXGuiFoundation");
		AddItem(item);
	}

	if (cfgData.items)
		free(cfgData.items);
	return 0;
}

int CPopADKillerDlg::SaveConfig()
{
	CConfigManager cfgManager(_T("config.dat"));
	CONFIGDATA cfgData;

	cfgData.bMiniStart = m_bMiniStart;
	cfgData.nCount = m_List.GetItemCount();
	if (cfgData.nCount > 0)
	{
		cfgData.items = (CONFIGITEM*)calloc(cfgData.nCount, sizeof(CONFIGITEM));
		for (unsigned int nIndex = 0; nIndex < cfgData.nCount; nIndex++)
		{
			ListItem* item = (ListItem*)m_List.GetItemData(nIndex);
			cfgData.items[nIndex].bKeyword = item->bKeyword;
			/*
			_stprintf_s(cfgItem.sTitle, sizeof(cfgItem.sTitle) / sizeof(TCHAR), _T("%s"), item->csTitle);
			_stprintf_s(cfgItem.sClass, sizeof(cfgItem.sClass) / sizeof(TCHAR), _T("%s"), item->csClass);
			_stprintf_s(cfgItem.sRect, sizeof(cfgItem.sRect) / sizeof(TCHAR), _T("%s"), item->csRect);
			_stprintf_s(cfgItem.sProcessId, sizeof(cfgItem.sProcessId) / sizeof(TCHAR), _T("%s"), item->csProcessId);
			_stprintf_s(cfgItem.sProcessName, sizeof(cfgItem.sProcessName) / sizeof(TCHAR), _T("%s"), item->csProcessName);
			_stprintf_s(cfgItem.sProcessPath, sizeof(cfgItem.sProcessPath) / sizeof(TCHAR), _T("%s"), item->csProcessPath);
			*/

#define COPYITEM(dst,src) (_tcscpy_s(dst, sizeof(dst) / sizeof(TCHAR), src))

			COPYITEM(cfgData.items[nIndex].sTitle, item->csTitle);
			COPYITEM(cfgData.items[nIndex].sClass, item->csClass);
			COPYITEM(cfgData.items[nIndex].sRect, item->csRect);
			COPYITEM(cfgData.items[nIndex].sProcessId, item->csProcessId);
			COPYITEM(cfgData.items[nIndex].sProcessName, item->csProcessName);
			COPYITEM(cfgData.items[nIndex].sProcessPath, item->csProcessPath);
		}
	}
	cfgManager.SaveConfig(&cfgData);
	return 0;
}

int CPopADKillerDlg::AddItem( const ListItem* item )
{
	int nIndex = m_List.GetItemCount();
	for (int i = 0; i < nIndex; i++)
	{
		CString csTitle = m_List.GetItemText(i, 0);
		CString csClass = m_List.GetItemText(i, 1);
		if (csTitle == item->csTitle && csClass == item->csClass)
		{
			MessageBox(_T("标题和类名不可同时重复,请重新添加"));
			return 0;
		}
	}
	//插入一条记录
	m_List.InsertItem(nIndex, _T(""));
	//设置记录数据
	m_List.SetItemData(nIndex, (DWORD_PTR)item);
	//设置记录项
	m_List.SetItemText(nIndex, 0, item->csTitle);
	m_List.SetItemText(nIndex, 1, item->csClass);
	m_List.SetItemText(nIndex, 2, item->csRect);
	m_List.SetItemText(nIndex, 3, item->csProcessId);
	m_List.SetItemText(nIndex, 4, item->csProcessName);
	m_List.SetItemText(nIndex, 5, item->csProcessPath);	
	return 0;
}


void CPopADKillerDlg::OnBnClickedBtnDeleteitem()
{
	// TODO: 在此添加控件通知处理程序代码
	int  nItem = 0;
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	while(pos)
	{
		nItem = m_List.GetNextSelectedItem(pos);
		ListItem* item = (ListItem*)m_List.GetItemData(nItem);
		if(item)
			delete item;
		m_List.DeleteItem(nItem);
	}
}

void CPopADKillerDlg::OnBnClickedCheck1()
{
	// TODO:  在此添加控件通知处理程序代码
	int stat = m_cbMiniStart.GetCheck();
	m_bMiniStart = (stat == 1);
}

void CPopADKillerDlg::DeleteAllItems()
{
	int nCount = m_List.GetItemCount();
	for(int nItem=0; nItem<nCount; nItem++)
	{
		ListItem* item = (ListItem*)m_List.GetItemData(nItem);
		if(item)
			delete item;
	}
	m_List.DeleteAllItems();
}

BOOL CPopADKillerDlg::HandleItem( const ListItem* item )
{
	int nCount = m_List.GetItemCount();
	for(int nItem=0; nItem<nCount; nItem++)
	{
		ListItem* pListItem = (ListItem*)m_List.GetItemData(nItem);
		if(pListItem)
		{
			if(pListItem->bKeyword)
			{
				if(item->csTitle.Find(pListItem->csTitle) == -1)
					continue;
			}
			else
			{
				if(item->csTitle != pListItem->csTitle)
					continue;
			}

			if(!pListItem->csClass.IsEmpty() && !item->csClass.IsEmpty())
			{
				if(pListItem->csClass != item->csClass)
					continue;
			}

			if(!pListItem->csProcessName.IsEmpty() && !item->csProcessName.IsEmpty())
			{
				if(pListItem->csProcessName != item->csProcessName)
					continue;
			}

			if (GetTickCount() - pListItem->lLastKillTime > 5000)
			{
				pListItem->lLastKillTime = GetTickCount();
				return TRUE;
			}
			else
				return FALSE;
		}
	}
	return FALSE;
}

UINT AFX_CDECL ThreadProc(LPVOID pParam)
{
	CPopADKillerDlg* pThis = (CPopADKillerDlg*)pParam;
	while (g_nThreadStat != 0)
	{
		// 完成某些工作的其它行程序    
		EnumWindows(EnumWindowsProc, (LPARAM)pThis); // 枚举窗口,杀QQ弹窗
		Sleep(200);
	}
	g_nThreadStat = 2;
	TRACE(_T("退出线程\r\n"));
	return 0;
}