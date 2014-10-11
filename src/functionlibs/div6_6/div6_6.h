#ifndef DIV6_6_H
#define DIV6_6_H

#include "div6_6_global.h"

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <IWindowDivMode.h>
#include <QtWebKit/QWebFrame>
#include <QtGui/QSplitter>
class  div6_6 : public QObject,
	public IWindowDivMode
{
public:
	div6_6();
	~div6_6();

	virtual void setSubWindows( QList<QWidget *> windows,int count );
	virtual void setParentWindow( QWidget * parent );
	virtual void flush();
	virtual void parentWindowResize( QResizeEvent *ev );
	virtual void nextPage();
	virtual void prePage();
	virtual int getCurrentPage();
	virtual int getPages();
	virtual QString getModeName();
	virtual void subWindowDblClick( QWidget *subWindow,QMouseEvent * ev );

	virtual long __stdcall QueryInterface( const IID & iid,void **ppv );
	virtual unsigned long __stdcall AddRef();
	virtual unsigned long __stdcall Release();
private:
	int m_nRef;
	QMutex m_csRef;

	QList<QWidget *> m_subWindows; 
	int m_nSubWindowCount;          //�ܵ��Ӵ�����Ŀ
	QWidget * m_parentOfSubWindows; //��ǰ�󴰿ڵ�ָ��
	int m_nCurrentPage;             //��ǰҳҳ��ֵ
	int m_nTotalWindowsCount;       //ҳ������

	int m_nColumn;                  //һ�е��Ӵ�����
	int m_nRow;                     //һ�е��Ӵ�����
	bool m_bIsMax;                  //�Ƿ��ѱ�˫���Ŵ�
};

#endif // DIV6_6_H
