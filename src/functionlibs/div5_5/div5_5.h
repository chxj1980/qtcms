#ifndef div5_5_H
#define div5_5_H

#include "div5_5_global.h"
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <IWindowDivMode.h>
#include <QtWebKit/QWebFrame>
#include <QtGui/QSplitter>

class div5_5 : public QObject,
	public IWindowDivMode
{

public:
	div5_5();
	~div5_5();

	//virtual void setSubWindows( QWidget * windows,int count );
	virtual void setSubWindows( QList<QWidget *> windows,int count );

	virtual void setParentWindow( QWidget * parent );

	virtual void flush();

	virtual void parentWindowResize( QResizeEvent *ev );


	virtual void nextPage();

	virtual void prePage();

	virtual int getCurrentPage();

	virtual int getPages();

	virtual QString getModeName();

	virtual long __stdcall QueryInterface( const IID & iid,void **ppv );

	virtual unsigned long __stdcall AddRef();

	virtual unsigned long __stdcall Release();

	virtual void subWindowDblClick( QWidget *subWindow,QMouseEvent * ev );

private:
	int m_nRef;
	QMutex m_csRef;
//	QWidget * m_subWindows;         //�Ӵ���ָ��
	QList<QWidget *> m_subWindows; 
	int m_nSubWindowCount;          //�ܵ��Ӵ�����Ŀ
	QWidget * m_parentOfSubWindows; //��ǰ�󴰿ڵ�ָ��
	int m_nCurrentPage;             //��ǰҳҳ��ֵ
	int m_nTotalWindowsCount;       //ҳ������

	int m_nColumn;                  //һ�е��Ӵ�����
	int m_nRow;                     //һ�е��Ӵ�����
	bool m_bIsMax;                  //�Ƿ��ѱ�˫���Ŵ�
};


#endif // div5_5_H
