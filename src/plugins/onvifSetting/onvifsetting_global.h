#ifndef ONVIFSETTING_GLOBAL_H
#define ONVIFSETTING_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QString>
typedef enum __tagOnvifSettingStepCode{
	GETNETWORKINFO,//��ȡ������Ϣ
	SETNETWORKINFO,//����������Ϣ
	GETENCODERINFO,//��ȡ������Ϣ
	SETENCODERINFO,//���ñ�����Ϣ
	GETDEVICEINFO,//��ȡ�豸��Ϣ
}tagOnvifSettingStepCode;
typedef struct __tagOnvifDeviceInfo{
	QString sIp;
	QString sPort;
	QString sUserName;
	QString sPassword;
	QString sSetIp;
	QString sSetMac;
	QString sSetGateway;
	QString sSetMask;
	QString sSetDns;
	int nIndex;
	int nWidth;
	int nHeight;
	QString sEnc_fps;
	QString sEnc_bps;
	QString sCodeFormat;
	QString sEncInterval;
	QString sEncProfile;
}tagOnvifDeviceInfo;
#endif // ONVIFSETTING_GLOBAL_H
