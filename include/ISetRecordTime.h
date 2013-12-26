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
	//	enable��trueΪ��ʼ��falseΪ�رմ�¼��ʱ��
	//����ֵ��
	//	0���޸ĳɹ�
	//	1���޸�ʧ��
	virtual int ModifyRecordTime(int recordtime_id,QString starttime,QString endtime,bool enable)=0;

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
	//		enable���Ƿ�ʹ�ã�0��true����1(false)��
	virtual QVariantMap GetRecordTimeInfo(int recordtime_id)=0;


	enum _enError{
		OK,
		E_SYSTEM_FAILED,
	};
};


#endif