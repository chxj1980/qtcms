#ifndef __IVIDEORENDERDIGITALZOOM_HEAD_FILE_ASDN8998YGF780AGW__
#define __IVIDEORENDERDIGITALZOOM_HEAD_FILE_ASDN8998YGF780AGW__
#include <libpcom.h>
#include <QtGui/QWidget>

interface IVideoRenderDigitalZoom : public IPComBase
{
	virtual bool addExtendWnd(QWidget * wnd,const QString sName)=0;//��Ӷ�����Ⱦ�Ĵ���
	virtual void setRenderRect(int nX,int nY,int nEndX,int nEndY)=0;//���û����ε����꣬nX,nY Ϊ��Ļ�ľ�������
	virtual void drawRectToOriginalWnd(int nX,int nY,int nEndX,int nEndY)=0;
	virtual void removeExtendWnd(const QString sName)=0;//�Ƴ�ָ�����ֵĶ�����Ⱦ����
	virtual void setRenderRectPen(int nLineWidth,int nR,int nG,int nB)=0;//���û����ε��߿����ɫ
};

#endif