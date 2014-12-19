#ifndef AUTOSEARCHDEVICE_H
#define AUTOSEARCHDEVICE_H

#include "autosearchdevice_global.h"
#include "IDeviceManager.h"
#include "IAutoSearchDevice.h"
#include <IUserManagerEx.h>
#include "guid.h"
#include <qwfw.h>
#include <autoSearchDeviceWindow.h>
#include <QTimer>
#include <QList>

class  autoSearchDevice:public QWidget,
	public QWebPluginFWBase
{
	Q_OBJECT
public:
	autoSearchDevice();
	~autoSearchDevice();
public:
	void autoSearchDeviceCb(QVariantMap tItem);
public slots:
		void AddEventProc( const QString sEvent,QString sProc ){m_mapEventProc.insertMulti(sEvent,sProc);}
		//�Զ����� �豸�ӿ�
		void startAutoSearchDevice(int nTime,int nWidth,int nHeight);
		void cancelSearch();
		void autoSearchDeviceTimeout();

		//�û���¼�ӿ�
		void showUserLoginUi(int nWidth,int nHeight);
		int checkUserLimit(quint64 uiCode,quint64 uiSubCode);//0:��֤ͨ�� 1���û�û�е�¼ 2���û�Ȩ�޲���
		void cancelLoginUI();//�˳��û���¼����
		int login(QString sUserName,QString sPassword,int nCode);//0:�����ɹ� 1������ʧ�ܣ�nCode:0(�û���¼)��1���û�ע����
		QStringList getUserList();
		QVariantMap getUserLimit();
private:
	autoSearchDeviceWindow m_tAutoSearchDeviceWindow;
	IAutoSearchDevice *m_pDeviceSearch;
	IUserManagerEx *m_pUserMangerEx;
	QList<QVariantMap> m_tDeviceList;
};

#endif // AUTOSEARCHDEVICE_H
