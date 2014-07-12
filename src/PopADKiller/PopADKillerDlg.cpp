
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

static int g_bThreadRunning = false;
// CPopADKillerDlg 对话框

#define WM_SHOWTASKICO (WM_USER + 1)	//最小化到系统托盘消息

#define IDM_RESTOREWINDOWS 1	//恢复窗体菜单ID

#define TIMER_AUTOKILL 1	//自动定时运行杀QQ弹窗

DWORD WINAPI ThreadProc(LPVOID pParam);

CPopADKillerDlg::CPopADKillerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPopADKillerDlg::IDD, pParent)
	, m_csOutput(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_ICO_PopADKiller/*IDR_MAINFRAME*/);
	m_bVisible = true;
	m_bInitFinished = false;

	m_pWndThread = NULL;
	m_pCfgData = new CONFIGDATA;
}

CPopADKillerDlg::~CPopADKillerDlg()
{
}

void CPopADKillerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	g_bThreadRunning = false;

	tagMSG msg;
	int sum = 50;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && --sum)     
	{    
		DispatchMessage(&msg);  
		Sleep(10);
		//DWORD code = 0;
		//BOOL bRet = ::GetExitCodeThread(m_pWndThread->m_hThread, &code);
		//if(code != STILL_ACTIVE)
		//	break;
		//if(WAIT_OBJECT_0 == WaitForSingleObject(m_pWndThread->m_hThread, 100))
		//{
		//	//delete m_pWndThread; 
		//	//m_pWndThread = NULL;
		//	break;
		//}
	} 

	DeleteAllItems();

	if(m_pCfgData->items)
		free(m_pCfgData->items);
	delete m_pCfgData;

	CDialogEx::OnClose();
}
void CPopADKillerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_csOutput);
	//	DDX_Control(pDX, IDC_BTN_CATCH, m_pCaptureButton);
	DDX_Control(pDX, IDC_LIST1, m_List);
}

BEGIN_MESSAGE_MAP(CPopADKillerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_HANDKILL, &CPopADKillerDlg::OnBnClickedBtnHandkill)
	ON_BN_CLICKED(IDC_BTN_SMALL, &CPopADKillerDlg::OnBnClickedBtnShowTaskIco)
	ON_MESSAGE(WM_SHOWTASKICO, OnShowTaskIco)
	ON_COMMAND(IDM_RESTOREWINDOWS,OnRestoreWindow)
	ON_WM_DESTROY()
	//	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BTN_ADDITEM, &CPopADKillerDlg::OnBnClickedBtnAdditem)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CPopADKillerDlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BTN_DELETEITEM, &CPopADKillerDlg::OnBnClickedBtnDeleteitem)
	ON_WM_CLOSE()
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

	CRect rt;
	GetClientRect(&rt);
	//列表
	int x = 4;
	int y = 4;
	int w = rt.Width() - 8;
	int h = ((rt.Height()-8)-(4+26+4+20+4))*2/3;
	GetDlgItem(IDC_LIST1)->MoveWindow(x, y, w, h);
	//按钮
	y += h + 4;
	w = 60;
	h = 26;
	GetDlgItem(IDC_BTN_ADDITEM)->MoveWindow(x, y, w, h);	//添加按钮
	x += w + 4;
	w = 120;
	GetDlgItem(IDC_BTN_SMALL)->MoveWindow(x, y, w, h);	//最小化到托盘按钮
	x += w + 4;
	w = 60;
	GetDlgItem(IDC_BTN_DELETEITEM)->MoveWindow(x, y, w, h);	//手杀按钮
	GetDlgItem(IDC_BTN_HANDKILL)->ShowWindow(SW_HIDE);
	//日志
	x = 4;
	y += h + 4;
	w = 60;
	h = 20;
	GetDlgItem(IDC_STATIC1)->MoveWindow(x, y, w, h);
	y += h + 4;
	w = rt.Width() - 8;
	h = (rt.Height()-8) - y;
	GetDlgItem(IDC_EDIT1)->MoveWindow(x, y, w, h);

	m_List.InsertColumn(0, _T("标题"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(1, _T("类名"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(2, _T("大小"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(3, _T("进程ID"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(4, _T("进程名"), LVCFMT_LEFT, 100);
	m_List.InsertColumn(5, _T("路径"), LVCFMT_LEFT, 400);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	InitConfig();

	if(!m_pCfgData->bMiniStart)
		ShowWindow(SW_SHOWNORMAL);
	//SetTimer(TIMER_AUTOKILL, 1000, NULL);

	g_bThreadRunning = true;
	m_pWndThread = AfxBeginThread((AFX_THREADPROC)ThreadProc, this/*, THREAD_PRIORITY_TIME_CRITICAL*/);

	m_bVisible = false;
	m_bInitFinished = true;
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
		/*	CTime t = CTime::GetCurrentTime();
		CString csText;
		csText.Format(_T("Time:%s Title:%s, Class:%s\r\n"), t.Format(_T("%H:%M:%S")), szTitle, szClass);
		pthis->m_csOutput = pthis->m_csOutput + csText;
		if(pthis->m_csOutput.GetLength() > 20000)
		pthis->m_csOutput = csText;
		pthis->GetDlgItem(IDC_EDIT1)->SetWindowText(pthis->m_csOutput);*/
		//pthis->UpdateData(FALSE);
	}

	//if(csClass == _T("TXGuiFoundation") && csTitle.Find(_T("腾讯"))!=-1)
	//{
	//	PostMessage(hwnd, WM_CLOSE, 0, 0);
	//	CTime t = CTime::GetCurrentTime();
	//	csTitle.Format(_T("Time:%s Title:%s, Class:%s\r\n"), t.Format(_T("%H:%M:%S")), szTitle, szClass);
	//	pthis->m_csOutput = pthis->m_csOutput + csTitle;
	//	pthis->UpdateData(FALSE);
	//	return FALSE;	//返回FALSE停止枚举
	//}
	// End Add [7/12/2014 Brilliance]
	return TRUE;
}

void CPopADKillerDlg::OnBnClickedBtnShowTaskIco()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bVisible = false;
	ShowWindow(SW_HIDE); // 当最小化市，隐藏主窗口 
}

LRESULT CPopADKillerDlg::OnShowTaskIco( WPARAM wParam, LPARAM lParam )
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
			OnRestoreWindow();
			ShowWindow(SW_SHOW);
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

void CPopADKillerDlg::OnRestoreWindow()
{
	m_bVisible = true;
	this->ShowWindow(SW_SHOWNORMAL);         // 显示主窗口
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
	cx -= 8;
	cy -= 8;
	if(m_bInitFinished)
	{
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
		w = 120;
		GetDlgItem(IDC_BTN_SMALL)->MoveWindow(x, y, w, h);	//最小化到托盘按钮
		x += w + 4;
		w = 60;
		GetDlgItem(IDC_BTN_DELETEITEM)->MoveWindow(x, y, w, h);	//手杀按钮
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
		m_bVisible = false;
		ShowWindow(SW_HIDE); // 当最小化市，隐藏主窗口              
	}  
}


void CPopADKillerDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if(!m_bVisible)
	{
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
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


void CPopADKillerDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

int CPopADKillerDlg::AddItem( const ListItem* item )
{
	int nIndex = m_List.GetItemCount();
	m_List.InsertItem(nIndex, _T(""));
	m_List.SetItemData(nIndex, (DWORD_PTR)item);
	m_List.SetItemText(nIndex, 0, item->csTitle);
	m_List.SetItemText(nIndex, 1, item->csClass);
	m_List.SetItemText(nIndex, 2, item->csRect);
	m_List.SetItemText(nIndex, 3, item->csProcessId);
	m_List.SetItemText(nIndex, 4, item->csProcessName);
	m_List.SetItemText(nIndex, 5, item->csProcessPath);	

	CONFIGITEM cfgItem;
	cfgItem.bKeyword = !!item->bKeyword;
 	_stprintf_s(cfgItem.sTitle, sizeof(cfgItem.sTitle)/sizeof(TCHAR), _T("%s"), item->csTitle);
 	_stprintf_s(cfgItem.sClass, sizeof(cfgItem.sClass)/sizeof(TCHAR), _T("%s"), item->csClass);
 	_stprintf_s(cfgItem.sRect, sizeof(cfgItem.sRect)/sizeof(TCHAR), _T("%s"), item->csRect);
 	_stprintf_s(cfgItem.sProcessId, sizeof(cfgItem.sProcessId)/sizeof(TCHAR), _T("%s"), item->csProcessId);
 	_stprintf_s(cfgItem.sProcessName, sizeof(cfgItem.sProcessName)/sizeof(TCHAR), _T("%s"), item->csProcessName);
 	_stprintf_s(cfgItem.sProcessPath, sizeof(cfgItem.sProcessPath)/sizeof(TCHAR), _T("%s"), item->csProcessPath);
	
	//_tcscpy_s(cfgItem.sTitle, sizeof(cfgItem.sTitle)/sizeof(TCHAR), item->csTitle);
	//_tcscpy_s(cfgItem.sClass, sizeof(cfgItem.sClass)/sizeof(TCHAR), item->csClass);
	//_tcscpy_s(cfgItem.sRect, sizeof(cfgItem.sRect)/sizeof(TCHAR), item->csRect);
	//_tcscpy_s(cfgItem.sProcessId, sizeof(cfgItem.sProcessId)/sizeof(TCHAR), item->csProcessId);
	//_tcscpy_s(cfgItem.sProcessName, sizeof(cfgItem.sProcessName)/sizeof(TCHAR), item->csProcessName);
	//_tcscpy_s(cfgItem.sProcessPath, sizeof(cfgItem.sProcessPath)/sizeof(TCHAR), item->csProcessPath);
	
	if(m_pCfgData->items == NULL)
	{
		m_pCfgData->items = (CONFIGITEM*)calloc(1, sizeof(CONFIGITEM));
		if(m_pCfgData->items)
			memcpy_s(m_pCfgData->items, sizeof(CONFIGITEM), &cfgItem, sizeof(CONFIGITEM));
	}
	else
	{
		unsigned long nSize = m_pCfgData->nCount * sizeof(CONFIGITEM);
		CONFIGITEM* pCfgItemTmp = &m_pCfgData->items[0];
		pCfgItemTmp = &m_pCfgData->items[1];

		CONFIGITEM* pCfgItem = (CONFIGITEM*)realloc(m_pCfgData->items, (nSize + sizeof(CONFIGITEM)));
		if(pCfgItem)
		{
			m_pCfgData->items = pCfgItem;
			memcpy_s(&m_pCfgData->items[m_pCfgData->nCount], sizeof(CONFIGITEM), &cfgItem, sizeof(CONFIGITEM));
		}
	}
	m_pCfgData->nCount++;

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

			return TRUE;
		}
	}
	return FALSE;
}

DWORD WINAPI ThreadProc(LPVOID pParam)
{
	CPopADKillerDlg* pThis = (CPopADKillerDlg*)pParam;
	while(1)
	{
		if(!g_bThreadRunning)
			break;
		EnumWindows(EnumWindowsProc, (LPARAM)pThis); // 枚举窗口,杀QQ弹窗
		Sleep(200);
	}
	return 100;
}


int CPopADKillerDlg::InitConfig()
{
	CConfigManager CfgManager(_T("config.dat"));
	m_pCfgData->bMiniStart = false;
	m_pCfgData->nCount = 0;
	m_pCfgData->items = NULL;

	CONFIGDATA *pCfgData = new CONFIGDATA;
	if(0 == CfgManager.LoadConfig(pCfgData))
	{
		//读取成功
		for(unsigned int i=0; i<m_pCfgData->nCount; i++)
		{
			ListItem* item = new ListItem;
			item->bKeyword = pCfgData->items[i].bKeyword;
			item->csTitle = pCfgData->items[i].sTitle;
			item->csClass = pCfgData->items[i].sClass;
			item->csRect = pCfgData->items[i].sRect;
			item->csProcessId = pCfgData->items[i].sProcessId;
			item->csProcessName = pCfgData->items[i].sProcessName;
			item->csProcessPath = pCfgData->items[i].sProcessPath;	
			AddItem(item);
		}
	}
	else
	{
		//读取失败,写入默认数据
		ListItem* item = new ListItem;
		item->bKeyword = 1;
		item->csTitle = _T("腾讯");
		item->csClass = _T("TXGuiFoundation");
		AddItem(item);

		item = new ListItem;
		item->bKeyword = 1;
		item->csTitle = _T("京东");
		item->csClass = _T("TXGuiFoundation");
		AddItem(item);

		CfgManager.SaveConfig(m_pCfgData);
	}

	if(pCfgData->items)
		free(pCfgData->items);
	return 0;
}

int CPopADKillerDlg::DelItem( unsigned int nIndex )
{

	return 0;
}

int CPopADKillerDlg::DelItem( const ListItem* item )
{
	return 0;
}
