#ifndef __ILOGINDEVICE_HEAD_FILE_ASDNVG8Y9ASDF__
#define __ILOGINDEVICE_HEAD_FILE_ASDNVG8Y9ASDF__
#include <libpcom.h>
#include <QtCore/QVariantMap>
#include <QtCore/QString>

interface IDeviceConnection : public IPComBase
{
	//����host
	//����ֵ
	//0�����óɹ�
	//1������ʧ��
	virtual int setDeviceHost(const QString & sAddr) = 0;
	//����Ports
	//����ֵ
	//0�����óɹ�
	//1������ʧ��
	virtual int setDevicePorts(const QVariantMap & ports) = 0;
	//����Id
	//����ֵ
	//0�����óɹ�
	//1������ʧ��
	virtual int setDeviceId(const QString & sAddress) = 0;
	//����Authority
	//����ֵ
	//0�����óɹ�
	//1������ʧ��
	virtual int setDeviceAuthorityInfomation(QString username,QString password) = 0;
	//�����豸
	//����ֵ
	//0�����ӳɹ�
	//1������ʧ��
	virtual int connectToDevice() = 0;
	// У���û�������
	// ����ֵ:
	//	0:У��ɹ�
	//	1:У��ʧ��
	virtual int authority() = 0;
	//�Ͽ�����
	//����ֵ
	//0���Ͽ��ɹ�
	//1���Ͽ�ʧ��
	virtual int disconnect() = 0;
	virtual int getCurrentStatus() = 0;
	virtual QString getDeviceHost() = 0;
	virtual QString getDeviceid() = 0;
	virtual QVariantMap getDevicePorts() = 0;
	enum _enConnectionStatus{
		CS_Disconnected,
		CS_Connectting,
		CS_Connected,
		CS_Disconnecting
	};

};

/*
setDevicePorts:parameter
	ports:
	"media":�������ݶ˿�
*/


#endif