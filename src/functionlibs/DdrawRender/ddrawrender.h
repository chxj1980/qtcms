#ifndef DDRAWRENDER_H
#define DDRAWRENDER_H
#include "ddrawrender_global.h"
#include <IVideoRender.h>
#include <IVideoRenderDigitalZoom.h>
#include <QtCore/QMutex>
#include <platform/IDDrawRender.h>

class DdrawRender : public IVideoRender,public IVideoRenderDigitalZoom
{
public:
	DdrawRender();
	~DdrawRender();

	virtual int init( int nWidth,int nHeight );

	virtual int deinit();

	virtual int setRenderWnd( QWidget * wnd );

	virtual int render( char *data,char *pYData,char *pUData,char *pVData,int nWidth,int nHeight,int nYStride,int nUVStride,int lineStride,const QString & pixelFormat,int flags );

	virtual int enable( bool bEnable );

	virtual int enableStretch( bool bEnable );

	virtual bool isRenderEnable();

	virtual bool isStretchEnable();

	virtual bool isPixelFormatAvalible( const QString &sFormat );

	virtual long __stdcall QueryInterface( const IID & iid,void **ppv );

	virtual unsigned long __stdcall AddRef();

	virtual unsigned long __stdcall Release();


	virtual bool addExtendWnd(QWidget * wnd,const QString sName);//��Ӷ�����Ⱦ�Ĵ���
	virtual void setRenderRect(int nStartX,int nStartY,int nEndX,int nEndY);//���û����ε����꣬nX,nY Ϊ��Ļ�ľ�������
	virtual void drawRectToOriginalWnd(int nStartX,int nStartY,int nEndX,int nEndY);
	virtual void removeExtendWnd(const QString sName);//�Ƴ�ָ�����ֵĶ�����Ⱦ����
	virtual void setRenderRectPen(int nLineWidth,int nR,int nG,int nB);//���û����ε��߿����ɫ

private:
	int m_nRef;
	QMutex m_csRef;
	bool m_bStretch;
	bool m_bEnable;
	IDDrawRender * m_renderObj;

};

#endif // DDRAWRENDER_H
