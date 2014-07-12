#pragma once

#include <vector>

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

typedef struct _st_config_item
{
	_st_config_item()
	{
		bKeyword = false;
		memset(sTitle, 0, sizeof(sTitle));
		memset(sClass, 0, sizeof(sClass));
		memset(sRect, 0, sizeof(sRect));
		memset(sProcessId, 0, sizeof(sProcessId));
		memset(sProcessName, 0, sizeof(sProcessName));
		memset(sProcessPath, 0, sizeof(sProcessPath));
	}
	bool bKeyword;
	TCHAR sTitle[MAX_PATH];
	TCHAR sClass[MAX_PATH];
	TCHAR sRect[64];
	TCHAR sProcessId[32];
	TCHAR sProcessName[MAX_PATH];
	TCHAR sProcessPath[MAX_PATH];
}CONFIGITEM;

typedef struct _st_config_data
{
	_st_config_data()
	{
		bMiniStart = false;
		nCount = 0;
		items = NULL;
	}
	bool bMiniStart;
	unsigned long nCount;
	CONFIGITEM* items;	
}CONFIGDATA;


class CConfigManager
{
public:
	CConfigManager(const tstring& sFilename);
	~CConfigManager(void);
private:
	tstring m_sFilename;
public:
//	int Init(const std::string& sFilename);
	int LoadConfig(CONFIGDATA* data);
	int SaveConfig(const CONFIGDATA* data);
};

