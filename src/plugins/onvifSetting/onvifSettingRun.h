#pragma once
#include <QThread>
#include "onvifsetting_global.h"
#include <QVariantMap>
#include <IOnvifRemoteInfo.h>
class onvifSettingRun:public QThread
{
	Q_OBJECT
public:
	onvifSettingRun(int nThreadId);
	~onvifSettingRun(void);
public:
	bool setOnvifDeviceParam(QString sIp,QString sPort,QString sUserName,QString sPassword);
	bool getOnvifDeviceNetworkInfo();
	bool getOnvifDevcieBaseInfo();
	bool setOnvifDeviceNetWorkInfo(QString sSetIp,QString sSetMac,QString sSetGateway,QString sSetMask,QString sSetDns);
	bool getOnvifDeviceEncoderInfo();
	bool setOnvifDeviceEncoderInfo(int nIndex,int nWidth,int nHeight,QString sEnc_fps,QString sEnc_bps,QString sCodeFormat,QString sEncInterval,QString sEncProfile);
signals:
	void sgThreadStart(QVariantMap tMap);//�̺߳ţ�ip��ַ����������
	void sgThreadEnd(QVariantMap tMap);//�̺߳ţ�ip��ַ����������
	void sgThreadInfo(QVariantMap tMap);//�̺߳ţ�ip ��ַ���������ͣ�״̬��0���ɹ���1��ʧ�ܣ�,�������ַ�����xml��
protected:
	void run();
private:
	int m_tStepCode;
	tagOnvifDeviceInfo m_tOnvifDeviceInfo;
	int m_nThreadId;
	IOnvifRemoteInfo *m_pOnvifReoteInfo;
};

