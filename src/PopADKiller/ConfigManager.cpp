#include "StdAfx.h"
#include "ConfigManager.h"


CConfigManager::CConfigManager(const tstring& sFilename)
{
	m_sFilename = sFilename;
}


CConfigManager::~CConfigManager(void)
{
}

int CConfigManager::LoadConfig( CONFIGDATA* data )
{
	FILE* pFile = NULL;
	int nRet = _tfopen_s(&pFile, m_sFilename.c_str(), _T("r"));
	if(nRet != 0)
		return nRet;
	
	fread(data, sizeof(data->bMiniStart)+sizeof(data->nCount), 1, pFile);
	data->items = (CONFIGITEM*)calloc(data->nCount, sizeof(CONFIGITEM));
	fread(data->items, sizeof(CONFIGITEM), data->nCount, pFile);
	fclose(pFile);
	return 0;
}

int CConfigManager::SaveConfig( const CONFIGDATA* data )
{
	FILE* pFile = NULL;
	int nRet = _tfopen_s(&pFile, m_sFilename.c_str(), _T("w"));
	if(nRet != 0)
		return nRet;

	fwrite(data, sizeof(data->bMiniStart)+sizeof(data->nCount), 1, pFile);
	fwrite(data->items, sizeof(CONFIGITEM), data->nCount, pFile);
	fclose(pFile);
	return 0;
}

// int CConfigManager::Init( const std::string& sFilename )
// {
// }

