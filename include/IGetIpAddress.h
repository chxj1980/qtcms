#ifndef __ILOGINDEVICE_HEAD_FILE_ASSDFDDNVG8Y9ASDF__
#define __ILOGINDEVICE_HEAD_FILE_ASSDFDDNVG8Y9ASDF__
#include <libpcom.h>
#include <QtCore/QVariantMap>
#include <QtCore/QString>

interface IGetIpAddress : public IPComBase
{
	//ͨ���豸ID��ȡ�豸��ͨ�ŵ�ַ�Ͷ˿�
	//������
	//sId:�豸id
	//sIp�����ص��豸ip
	//sPort�����ص��豸�˿�
	//����ֵ
	//true���ɹ�
	//flase��ʧ��
	virtual bool getIpAddressEx(const QString sId,QString &sIp,QString &sPort,QString &sHttp) = 0;
};


#endif