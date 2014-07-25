#pragma once
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QStringList>
#include <QVariantMap>
#include <QMultiMap>
#include <QtNetwork/QTcpSocket>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QByteArray>
#include <QDateTime>
typedef int (__cdecl *setAutoSycTimeEventCb)(QString eventName,QVariantMap tInfo,void *pUser);
typedef struct __tagSetAutoSycTimeProInfo{
	setAutoSycTimeEventCb proc;
	void *pUser;
}tagSetAutoSycTimeProInfo;

typedef struct __tagAutoSycTimeDeviceInfo{
	QString sAddr;
	quint16 uiPort;
	QString sUserName;
	QString sPassWord;
}tagAutoSycTimeDeviceInfo;
typedef enum __tagSetAutoSycTimeStepCode{
	SYC_CONNECT,
	SYC_GETVESIONINFO,
	SYS_SYCTIMETOOLDVERSION,
	SYS_GETLOCALTIME,
	SYS_SYCTIMETONEWVERSION,
	SYC_REVICEDATA,
	SYC_FAIL,
	SYC_SUCCESS,
	SYC_END,
}tagSetAutoSycTimeStepCode;
typedef enum __tagReceiveStepCode{
	RECV_VERSERINFO,//���հ汾��Ϣ
	RECV_OLDVERSER,//����1.1.3��ǰ�汾�ظ�����Ϣ
	RECV_LOCALSYSTEMTIME,//���յ���ʱ����Ϣ
	RECV_NEWVERSER,//����1.1.3�Ժ�汾�Ļظ���Ϣ
}tagReceiveStepCode;
class SetAutoSycTime:public QThread
{
	Q_OBJECT
public:
	SetAutoSycTime(void);
	~SetAutoSycTime(void);
public:
	void setAutoSycTime(QString sAddr,quint16 uiPort,QString sUserName,QString sPassWord);
	void registerEvent(QString eventName,int (__cdecl *proc)(QString ,QVariantMap,void*),void *pUser);

protected:
	void run();
private:
	void sleepEx(int time);
	void eventCallBack(QString sEventName,QVariantMap evMap);
private slots:
	void slCheckBlock();
private:
	int m_nSleepSwitch;
	tagAutoSycTimeDeviceInfo m_tDeviceInfo;
	bool m_bIsBlock;
	int m_nPosition;
	QMultiMap<QString,tagSetAutoSycTimeProInfo> m_tEventMap;
	QStringList m_sEventList;
	bool m_bStop;
	QTimer m_tCheckTimer;
};

