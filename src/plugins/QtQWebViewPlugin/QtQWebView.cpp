#include "QtQWebView.h"
#include <QFileDialog>
#include <QMap>


QtQWebView::QtQWebView():QWebPluginFWBase(this)
{

}


QtQWebView::~QtQWebView()
{
	QList<tagViewPage>::const_iterator item;
	for (item=m_ViewPageList.constBegin();item!=m_ViewPageList.constEnd();++item)
	{
		qDebug()<<item->url;
		item->m_SubWebView->close();
		delete item->m_SubWebView;
	}
	m_ViewPageList.clear();
}

void QtQWebView::LoadNewPage( QString url )
{
	QList<tagViewPage>::const_iterator ite;
	for (ite=m_ViewPageList.constBegin();ite!=m_ViewPageList.constEnd();++ite)
	{
		ite->m_SubWebView->hide();
	}

	QList<tagViewPage>::const_iterator item;
	for (item=m_ViewPageList.constBegin();item!=m_ViewPageList.constEnd();++item)
	{
		qDebug()<<item->url;
		if (url==item->url)
		{
			item->m_SubWebView->showMaximized();
			item->m_SubWebView->OnRefressMessage();
			DEF_EVENT_PARAM(arg);
			EP_ADD_PARAM(arg,"state","hide");
			EventProcCall("IndexPageState",arg);
			return;
		}
	}
	tagViewPage m_tagViewPage;
	m_tagViewPage.m_SubWebView=new SubWebView(url);
	if (NULL==m_tagViewPage.m_SubWebView)
	{
		return;
	}
	connect(m_tagViewPage.m_SubWebView,SIGNAL(LoadOrChangeUrl(const QString &)),this,SLOT(LoadNewPageFromViewSignal(const QString &)));
	connect(m_tagViewPage.m_SubWebView,SIGNAL(CloseAllPage()),this,SLOT(CloseAllPage()));
	m_tagViewPage.m_SubWebView->showMaximized();
	DEF_EVENT_PARAM(arg);
	EP_ADD_PARAM(arg,"state","hide");
	EventProcCall("IndexPageState",arg);
	m_tagViewPage.url = url;
	m_ViewPageList.append(m_tagViewPage);

	return;
}

void QtQWebView::LoadNewPageFromViewSignal( const QString &text )
{
	qDebug()<<text;
	QDomDocument ConFile;
	ConFile.setContent(text);
	QDomNode pageaction=ConFile.elementsByTagName("pageaction").at(0);
	QString SrcUrl=pageaction.toElement().attribute("SrcUrl");
	QString SrcAct=pageaction.toElement().attribute("SrcAct");
	QString DstUrl=pageaction.toElement().attribute("DstUrl");
	QString DstAct=pageaction.toElement().attribute("DstAct");

	//��ת����ҳ
	if ("index"==SrcAct)
	{
		QList<tagViewPage>::const_iterator it;
		for(it=m_ViewPageList.constBegin();it!=m_ViewPageList.constEnd();++it){
			it->m_SubWebView->hide();
		}
		DEF_EVENT_PARAM(arg);
		EP_ADD_PARAM(arg,"state","show");
		EventProcCall("IndexPageState",arg);
		return;
	}

	//�رյ�ǰҳ��
	QList<tagViewPage>::const_iterator item;
	for (item=m_ViewPageList.constBegin();item!=m_ViewPageList.constEnd();++item)
	{
		qDebug()<<item->url;
		//if (SrcUrl==item->url)
		//{
		//	item->m_SubWebView->hide();
		//}
		if (SrcUrl==item->url&&SrcAct=="close")
		{
			item->m_SubWebView->close();
			DEF_EVENT_PARAM(arg);
			EP_ADD_PARAM(arg,"state","show");
			EventProcCall("IndexPageState",arg);
			return;
		}
	}
	//��ת��Ŀ��ҳ��
	if ("new"==DstAct)
	{
		bool bExit=false;

		QList<tagViewPage>::const_iterator it;
		for (it=m_ViewPageList.constBegin();it!=m_ViewPageList.constEnd();++it)
		{
			if (DstUrl==it->url)
			{
				it->m_SubWebView->showMaximized();
				it->m_SubWebView->OnRefressMessage();
				bExit=true;
				break;
			}
		}
		QList<tagViewPage>::const_iterator ite;
		for(ite=m_ViewPageList.constBegin();ite!=m_ViewPageList.constEnd();++ite){
			if (DstUrl!=ite->url)
			{
				ite->m_SubWebView->hide();
			}
		}
		if (false==bExit)
		{
			tagViewPage m_tagViewPage;
			m_tagViewPage.m_SubWebView=new SubWebView(DstUrl);
			if (NULL!=m_tagViewPage.m_SubWebView)
			{
				connect(m_tagViewPage.m_SubWebView,SIGNAL(LoadOrChangeUrl(const QString &)),this,SLOT(LoadNewPageFromViewSignal(const QString &)));
				connect(m_tagViewPage.m_SubWebView,SIGNAL(CloseAllPage()),this,SLOT(CloseAllPage()));
				m_tagViewPage.m_SubWebView->showMaximized();
				m_tagViewPage.url.append(DstUrl);
				m_ViewPageList.append(m_tagViewPage);
			}
		}
		return;
	}
	//��ǰҳ�����¼���Ŀ��ҳ��
	if ("reload"==DstAct)
	{
		QList<tagViewPage>::iterator it;
		for(it=m_ViewPageList.begin();it!=m_ViewPageList.end();++it){
			if (it->url==SrcUrl)
			{
				QString temp=QCoreApplication::applicationDirPath();
				temp.append(DstUrl);
				it->m_SubWebView->load(temp);
				it->url.clear();
				it->url.append(DstUrl);
				it->m_SubWebView->showMaximized();
			}
		}
	}
	return;
}

void QtQWebView::CloseAllPage()
{
	DEF_EVENT_PARAM(arg);
	EP_ADD_PARAM(arg,"state","close");
	EventProcCall("IndexPageState",arg);
}

