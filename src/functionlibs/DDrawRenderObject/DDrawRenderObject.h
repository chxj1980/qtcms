// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� DDRAWRENDEROBJECT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// DDRAWRENDEROBJECT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifndef __DDRAWRENDEROBJECT_HEAD_FILE_SANDU9YS8B9S8__
#define __DDRAWRENDEROBJECT_HEAD_FILE_SANDU9YS8B9S8__
#include <platform/IDDrawRender.h>
#include <ddraw.h>
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"ddraw.lib")
#include "DDMutex.h"

class CDDrawRenderObject : public IDDrawRender
{
public:
	CDDrawRenderObject();
	~CDDrawRenderObject();


	virtual int init( int nWidth,int nHeight );

	virtual int deinit();

	virtual int setRenderWnd( HWND wnd );

	virtual int render( char *pYData,char *pUData,char *pVData,int nWidth,int nHeight,int nYStride,int nUVStride,int lineStride );

	virtual int enableStretch( bool bEnable );

	virtual int enable( bool bEnable );

	virtual bool addExtendWnd(HWND wnd,const char* sName);//��Ӷ�����Ⱦ�Ĵ���
	virtual void setRenderRect(int nStartX,int nStartY,int nEndX,int nEndY);//���û����ε����� 
	virtual void drawRectToOriginalWnd( int nX,int nY,int nEndX,int nEndY );
	virtual void removeExtendWnd(const char* sName);//�Ƴ�ָ�����ֵĶ�����Ⱦ����
	virtual void setRenderRectPen(int nLineWidth,int nR,int nG,int nB);//���û����ε��߿����ɫ
private:
	void DrawARectangle(HDC hdc,int nRectStartX,int nRectStartY,int nRectEndX,int nRectEndY,HWND WND) ;
	void setZoomRect(RECT &tRect,int nWidth,int nHeight);
	void checkHr(HRESULT hr);
private:
	bool                 m_bEnable;
	bool                 m_bStretch;
	int                  m_nWidth;
	int                  m_nHeight;
	LPDIRECTDRAWSURFACE7 m_pOffscreenSurface;
	LPDIRECTDRAWSURFACE7 m_pOffOsdScreenSurface;
	HWND                 m_hPlayWnd;
	CDDMutex             m_csPlayWnd;
	CDDMutex             m_csOffScreenSurface;
	CDDMutex             m_csOffOsdScreenSurface;

	HWND				 m_hExtendWnd;
	CDDMutex			 m_csExtendWnd;

	int m_nRectStartX;
	int m_nRectStartY;
	int m_nRectEndX;
	int m_nRectEndY;
	int m_nRectSurfaceWidth;
	int m_nRectSurfaceHeight;
	RECT m_nLastExtendWndRect;

	int m_nOriginalRectStartX;
	int m_nOriginalRectStartY;
	int m_nOriginalRectEndX;
	int m_nOriginalRectEndY;
};

#endif