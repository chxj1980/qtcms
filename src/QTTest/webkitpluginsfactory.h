#ifndef WEBKITPLUGINSFACTORY_H
#define WEBKITPLUGINSFACTORY_H

#include <QWebPluginFactory>

class WebkitPluginsFactory : public QWebPluginFactory
{
	Q_OBJECT

public:
	WebkitPluginsFactory(QObject *parent);
	~WebkitPluginsFactory();

	virtual QList<QWebPluginFactory::Plugin> plugins() const;

	virtual QObject * create( const QString& mimeType, const QUrl&, const QStringList& argumentNames, const QStringList& argumentValues ) const;
private:
	// ����б�

	mutable QList<QList<QWebPluginFactory::Plugin> > pluginslist;

	//����ӿڣ�����ӿ��������Զ���Ĳ����ͬ��ӿڡ�

	//����ӿ��ں���ὲ����

	mutable QList<WebKitPluginInterface *> interfaces;
};

#endif // WEBKITPLUGINSFACTORY_H
