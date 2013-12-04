#ifndef DIV6_1_H
#define DIV6_1_H

#include "div6_1_global.h"
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <IWindowDivMode.h>

class div6_1 : public QObject,
	public IWindowDivMode
{
public:
	div6_1();
	~div6_1();

//	virtual void setSubWindows( QWidget * windows,int count );
	virtual void setSubWindows( QList<QWidget *> windows,int count );

	virtual void setParentWindow( QWidget * parent );

	virtual void flush();

	virtual void parentWindowResize( QResizeEvent *ev );

	virtual void subWindowDblClick( QWidget *subWindow,QMouseEvent * ev );

	virtual void nextPage();

	virtual void prePage();

	virtual int getCurrentPage();

	virtual int getPages();

	virtual QString getModeName();

	virtual long __stdcall QueryInterface( const IID & iid,void **ppv );

	virtual unsigned long __stdcall AddRef();

	virtual unsigned long __stdcall Release();

	virtual void ChangePosition();
private:
	int m_nRef;
	QMutex m_csRef;
	//QWidget * m_subWindows;
	QList<QWidget *> m_subWindows; 
	int m_nSubWindowCount;
	QWidget * m_parentOfSubWindows;
	int m_nCurrentPage;

	int m_nCloum;//��
	int m_nRow;//��
	int m_nWindowsPerPage;//ÿҳ�Ĵ�����
	int m_nWidth;//�п�
	int m_nHeight;//�и�
	int m_nIndexPerPage;//ÿҳ��һ�����ڵ�����
	bool m_bIsLastPage;//�Ƿ����һҳ
};

#endif // DIV6_1_H
