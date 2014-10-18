#ifndef __ISETRECORDTIME_HEAD_FILE__0NSDV9FNP9QUAB89A__
#define __ISETRECORDTIME_HEAD_FILE__0NSDV9FNP9QUAB89A__


#include "libpcom.h"
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>

interface ISetRecordTime : public IPComBase
{
	//�޸��豸¼��ʱ��
	//���������
	//	recordtime_id��ָ�����޸ĵ�¼��ʱ���id��
	//	starttime����ʼ¼��ʱ��
	//	endtime������¼��ʱ��
	//	enable����λ���㣬00��û��¼��01����ʱ¼��10���ƶ�¼��11����ʱ���ƶ�¼��
	//����ֵ��
	//	0���޸ĳɹ�
	//	1���޸�ʧ��
	virtual int ModifyRecordTime(int recordtime_id,QString starttime,QString endtime,int enable)=0;

	//��ѯָ��ͨ���µ�¼��ʱ���¼
	//���������
	//	chl_id��ͨ����
	//����ֵ��
	//	¼��ʱ���¼��id���б�
	virtual QStringList GetRecordTimeBydevId(int chl_id)=0;
	//��ѯָ��¼��ʱ���¼�����ֶ�ֵ
	//���������
	//	recordtime_id��¼��ʱ���¼id
	//����ֵ��
	//	¼��ʱ����ֶ�ֵ��
	//		chl_id:ͨ����
	//		schedle_id��ʱ���id
	//		weekday������ѡֵ 0����һ����1���ܶ�����2����������3�����ģ���4�����壩��5����������6�������죩��
	//		starttime����ʼʱ��
	//		endtime������ʱ��
	//		enable����λ���㣬00��û��¼��01����ʱ¼��10���ƶ�¼��11����ʱ���ƶ�¼��
	virtual QVariantMap GetRecordTimeInfo(int recordtime_id)=0;


	enum _enError{
		OK,
		E_SYSTEM_FAILED,
	};
};


#endif