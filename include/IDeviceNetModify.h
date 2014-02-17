#ifndef __IDEVICENETMODIFY_HEAD_FILE_HVG4S5HFGH832A__
#define __IDEVICENETMODIFY_HEAD_FILE_HVG4S5HFGH832A__
#include <libpcom.h>
#include <IEventRegister.h>

interface IDeviceNetModify : public IPComBase
{
	//�����������
	//sDeviceID:�豸ID
	//sAddress��IP��ַ
	//sMask����������
	//sGateway������
	//sMac��MAC��ַ
	//uiPort:�˿ں�
	//sUsername���û���
	//sPassword:����
	virtual int SetNetworkInfo(const QString &sDeviceID,
		const QString &sAddress,
		const QString &sMask,
		const QString &sGateway,
		const QString &sMac,
		const QString &sPort,
		const QString &sUsername,
		const QString &sPassword
		) = 0;

	//��ѯ�¼�ע����
	virtual IEventRegister * QueryEventRegister() = 0;

	enum _enErrorCode{
		OK,
		E_INVALID_PARAM,
		E_SYSTEM_FAILED,
	};
};

/*
Event:
@1	name: "SettingStatus"
	parameters:
		"Status":���õ�״̬,��ѡ��set info success��,��set port success������Unauthorized��
*/

#endif