
// PopADKiller.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPopADKillerApp:
// �йش����ʵ�֣������ PopADKiller.cpp
//

class CPopADKillerApp : public CWinApp
{
public:
	CPopADKillerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPopADKillerApp theApp;