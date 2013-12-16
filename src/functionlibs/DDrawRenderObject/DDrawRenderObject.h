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

private:
	bool                 m_bEnable;
	bool                 m_bStretch;
	int                  m_nWidth;
	int                  m_nHeight;
	LPDIRECTDRAWSURFACE7 m_pOffscreenSurface;
	HWND                 m_hPlayWnd;
	CDDMutex             m_csPlayWnd;
	CDDMutex             m_csOffScreenSurface;
};

#endif