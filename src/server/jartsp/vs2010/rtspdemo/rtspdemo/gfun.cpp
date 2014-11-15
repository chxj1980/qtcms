#include "stdafx.h"
#include "gfun.h"

LPDIRECTDRAW7			g_pDirectDraw7=NULL;				                       	//����Ԥ����DirectDraw����
LPDIRECTDRAWSURFACE7	g_pPrimarySurface=NULL;	
LPDIRECTDRAWCLIPPER	    g_pClipper=NULL;
CRITICAL_SECTION		g_csClipper;


CRITICAL_SECTION g_csDecInit;

int DdrawInit(HWND hWnd)
{   
	HRESULT hr;
	
	
	//����DDraw����
	hr = DirectDrawCreateEx(NULL,(LPVOID *)&g_pDirectDraw7,IID_IDirectDraw7,NULL);
	if (FAILED(hr))
	{
		g_pDirectDraw7 = NULL;
		return -1;
	}
	
	//����Э���ȼ���ִ�гɹ�������DD_OK
	hr = g_pDirectDraw7->SetCooperativeLevel(NULL,DDSCL_NORMAL);
	if (FAILED(hr))
	{
		g_pDirectDraw7->Release();
		g_pDirectDraw7 = NULL;
		return -1;
	}
	
	//�����ü���
	hr = g_pDirectDraw7->CreateClipper(0,&g_pClipper,NULL);
	if (FAILED(hr))
	{
		g_pDirectDraw7->Release();
		g_pDirectDraw7 = NULL;
		return -1;
	}
	
	InitializeCriticalSection(&g_csClipper);
	
	//������ҳ��
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd,sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	
	hr = g_pDirectDraw7->CreateSurface(&ddsd,&g_pPrimarySurface,NULL);
	if (FAILED(hr))
	{
		g_pClipper->Release();
		g_pClipper = NULL;
		g_pDirectDraw7->Release();
		g_pDirectDraw7 = NULL;
		g_pPrimarySurface = NULL;
		return -1;
	}
	
	
	
	//���زü�������ҳ��
	hr = g_pClipper->SetHWnd(NULL,hWnd);
	if (FAILED(hr))
	{
		g_pClipper->Release();
		g_pClipper = NULL;
		g_pPrimarySurface->Release();
		g_pPrimarySurface = NULL;
		g_pDirectDraw7->Release();
		g_pDirectDraw7 = NULL;
		return -1;
	}
	//���������������ӵ������� 
	hr = g_pPrimarySurface->SetClipper(g_pClipper);
	if (FAILED(hr))
	{
		g_pClipper->Release();
		g_pClipper = NULL;
		g_pPrimarySurface->Release();
		g_pPrimarySurface = NULL;
		g_pDirectDraw7->Release();
		g_pDirectDraw7 = NULL;
		return -1;
	}
	
	
	
	return 0;
}

void DdrawDeinit()
{
	if (g_pClipper)
	{
		g_pClipper->Release();
		g_pClipper = NULL;
	}
	if (g_pPrimarySurface)
	{	
		g_pPrimarySurface->Release();
		g_pPrimarySurface = NULL;
	}
	if (g_pDirectDraw7)
	{
		g_pDirectDraw7->Release();
		g_pDirectDraw7 = NULL;
	}

}

CTime time_t_to_ctime(time_t _timet)
{
	TIME_ZONE_INFORMATION tzi;
	GetSystemTime(&tzi.StandardDate);
	GetTimeZoneInformation(&tzi);
	
	CTime time(_timet + (tzi.Bias * 60));
//	CTime time(_timet);	
	return time;
}