#ifndef __IREMOTEBACKUP_HEAD_FILE_OASDF892389HUDSAF80BAS__
#define __IREMOTEBACKUP_HEAD_FILE_OASDF892389HUDSAF80BAS__
#include "libpcom.h"
#include <QtCore/QString>
#include <QtCore/QDateTime>

interface IRemoteBackup : public IPComBase
{

	// ���ñ��ݲ���
	// sAddr:�豸ip
	// uiPort:�豸�˿�
	// sEseeId:�豸id
	//sDeviceName:�豸����
	// nChannel:���λΪ0ͨ�������λΪ31ͨ����ÿһλ��ʾһ��ͨ��
	// nTypes:¼�����ͣ���λ���㣬��0λ��ʾ��ʱ¼�񣬵�1λ��ʾ�ƶ����¼�񣬵�2λ��ʾ����¼�񣬵�3λ��ʾ�ֶ�¼��
	// startTime:��ʼʱ��
	// endTime:����ʱ��
	// sbkpath:����·��

	virtual int startBackup(const QString &sAddr,unsigned int uiPort,const QString &sEseeId,
							int nChannel,
							int nTypes,
							const QString &sDeviceName,
							const QDateTime & startTime,
							const QDateTime & endTime,
							const QString & sbkpath) = 0;

	virtual int stopBackup() = 0;

	//��ȡ���ݽ��� ��0.0~1.0��
	virtual float getProgress() = 0;

	enum _emError{
		OK = 0,          //�ɹ�
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};
};

// event
// 	@1 name "backupEvent"
// parameters:
// 	"types":"startBackup"��ʼ���б��� "stopBackup"ֹͣ���� "diskfull"�������� "backupFinished"������� "noStream"��ȡ��������
// @2 name "progress" ���ݽ���
// parameters:
// "parm":" ��ΧΪ��0-100��

#endif