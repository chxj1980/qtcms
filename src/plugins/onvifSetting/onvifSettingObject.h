#pragma once

#include <qwfw.h>
#include <QVariantMap>
#include <IOnvifRemoteInfo.h>
#include <onvifSettingRun.h>
#include <QList>
#include <QMutex>
#include <QDateTime>
#define THREADNUM 10
#define OPERATIONINTERVAL 1000
typedef struct __tagOnvifOperationItem{
	int nThreadId;
	quint64 uiStartTime;
	tagOnvifSettingStepCode tOperationType;
	QString sDeviceIp;
}tagOnvifOperationItem;
class onvifSettingObject:public QWidget,
	public QWebPluginFWBase
{
	Q_OBJECT
public:
	onvifSettingObject();
	~onvifSettingObject();
public slots:
	void AddEventProc( const QString sEvent,QString sProc ){m_mapEventProc.insertMulti(sEvent,sProc);};
	bool setOnvifDeviceParam(QString sIp,QString sPort,QString sUserName,QString sPassword);
	void getOnvifDeviceNetworkInfo();
	void getOnvifDeviceBaseInfo();
	bool setOnvifDeviceNetWorkInfo(QString sSetIp,QString sSetMac,QString sSetGateway,QString sSetMask,QString sSetDns);
	void getOnvifDeviceEncoderInfo();
	bool setOnvifDeviceEncoderInfo(int nIndex,int nWidth,int nHeight,QString sEnc_fps,QString sEnc_bps,QString sCodeFormat,QString sEncInterval,QString sEncProfile);
public slots:
	void slThreadStart(QVariantMap tMap);//�̺߳ţ�ip��ַ����������
	void slThreadEnd(QVariantMap tMap);//�̺߳ţ�ip��ַ����������
	void slThreadInfo(QVariantMap tMap);//�̺߳ţ�ip ��ַ���������ͣ�״̬��0���ɹ���1��ʧ�ܣ�,�������ַ�����xml��
private:
	void saveOperationItem(QString sDeviceIp,tagOnvifSettingStepCode tOperationType,int nThreadId);
	bool getFreeThreadId(QString sDeviceIp,tagOnvifSettingStepCode tOperationType,int &nThreadId);
	void removeOperationItem(QString sDevice,tagOnvifSettingStepCode tOperationType,int nThreadId);
private:
	QList<tagOnvifOperationItem> m_tOperationList;
	QList<onvifSettingRun*> m_tOnvifSettingRunList;
	QMutex m_tOperationListMutex;
	tagOnvifDeviceInfo m_tOnvifDeviceInfo;
	QMutex m_tThreadMutex;
};

