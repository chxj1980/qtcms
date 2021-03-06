#ifndef div7_7_H
#define div7_7_H

#include "div7_7_global.h"
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <IWindowDivMode.h>

class div7_7 : public QObject,
	public IWindowDivMode
{
public:

#define PAGE_SUBCOUNT 49
	div7_7();
	~div7_7();

	//virtual void setSubWindows( QWidget * windows,int count );
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
private:
	int m_nRef;
	QMutex m_csRef;
//	QWidget * m_subWindows;
	QList<QWidget *> m_subWindows; 
	int m_nSubWindowCount;
	QWidget * m_parentOfSubWindows;
	int m_nCurrentPage;
	
	int m_PageSubCount;
	int m_row;
	int m_column;
	bool m_singeldisplay;
	QSize m_parentSize;
	//QWidget * m_currSubWindows;
	int m_keyPage;

	int getSubVindowIndex(QWidget * pSubWindow);
	//void adjustSubWindow(int index);
	//void adjustSubWindow(QWidget * pSubWindow);
	void setSingelDiaplay(QWidget * pSubWindow);
	void setTotalDisplay();

	void reSizeSubWindows();
};

#endif // div7_7_H
