#ifndef __IDEVICESEARCHRECORD_HEAD_FILE_ASD89A8SDBV07SADY__
#define __IDEVICESEARCHRECORD_HEAD_FILE_ASD89A8SDBV07SADY__
#include <libpcom.h>

interface IDeviceSearchRecord : IPComBase
{
	// ��ʼ����¼���ļ�
	// nChannel:���λΪ0ͨ�������λΪ31ͨ����ÿһλ��ʾһ��ͨ��
	// nTypes:¼�����ͣ���λ���㣬��0λ��ʾ��ʱ¼�񣬵�1λ��ʾ�ƶ����¼�񣬵�2λ��ʾ����¼�񣬵�3λ��ʾ�ֶ�¼��
	// startTime:��ʼʱ��
	// endTime:����ʱ��
	// ����ֵ:
	//	0:���óɹ�
	//	1:����ʧ��
	//	2:��������
	virtual int startSearchRecFile(int nChannel,int nTypes,const QDateTime & startTime,const QDateTime & endTime) = 0;

};

// event
// 	@1 name "foundFile"
// parameters:
// 	"channel":¼��������ͨ����
// 	"types":¼�����ͣ���λ���㣬��0λ��ʾ��ʱ¼�񣬵�1λ��ʾ�ƶ����¼�񣬵�2λ��ʾ����¼�񣬵�3λ��ʾ�ֶ�¼��
// 	"start":¼��ʼʱ�䣬��ʽΪ"YYYY-MM-DD hh:mm:ss"
// 	"end":¼�����ʱ�䣬��ʽΪ"YYYY-MM-DD hh:mm:ss"
// 	"filename":¼����ļ���
// 
// 	@2 name "recFileSearchFinished"
// parameters:
// 	"total":�ܹ���¼���¼��Ŀ��

#endif