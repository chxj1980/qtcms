#ifndef __IEVENTREGISTER_HEAD_FILE_9VH988YASDGVG7AV__
#define __IEVENTREGISTER_HEAD_FILE_9VH988YASDGVG7AV__
#include <libpcom.h>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

interface IEventRegister : public IPComBase
{
	//�����¼��б�
	virtual QStringList eventList() = 0;
	//��ѯ�¼�����
	//eventName�����󷵻��¼�����������
	//eventParams�������¼��������б�
	//����ֵ��
	//OK���ɹ�
	//E_EVENT_NOT_SUPPORT���޴��¼�
	//E_INVALID_PARAM��ϵͳ����
	virtual int queryEvent(QString eventName,QStringList &eventParams) = 0;
	//ע���¼�
	//eventName��ע����¼�����
	//proc��ע���¼��Ļص�����
	//pUser��ע���¼��ĸ�ָ��
	//����ֵ��
	//OK���ɹ�
	//E_EVENT_NOT_SUPPORT���޴��¼�
	//E_INVALID_PARAM��ϵͳ����
	virtual int registerEvent(QString eventName,int (__cdecl *proc)(QString,QVariantMap,void *),void *pUser) = 0;

	enum _enErrorCode{
		OK,
		E_EVENT_NOT_SUPPORT,
		E_INVALID_PARAM
	};
};

#endif