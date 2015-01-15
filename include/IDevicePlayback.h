#ifndef __IDEVICEREMOTEPLAYBACK_HEAD_FILE_DY3SEX78DFG42LMGK73V__
#define __IDEVICEREMOTEPLAYBACK_HEAD_FILE_DY3SEX78DFG42LMGK73V__
#include <libpcom.h>
#include <QtGui/QWidget>
#include <QtCore/QDateTime>

interface IDeviceRemotePlayback : IPComBase
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

	// ͨ��¼�����ֹʱ������ȡ����
	// nChannel:���λΪ0ͨ�������λΪ31ͨ����ÿһλ��ʾһ��ͨ��
	// nTypes:¼�����ͣ���λ���㣬��0λ��ʾ��ʱ¼�񣬵�1λ��ʾ�ƶ����¼�񣬵�2λ��ʾ����¼�񣬵�3λ��ʾ�ֶ�¼��
	// startTime:��ʼʱ��
	// endTime:����ʱ��
	// ����ֵ:
	//	0:���óɹ�
	//	1:����ʧ��
	//	2:��������
	virtual int getPlaybackStreamByTime(int nChannel,int nTypes,const QDateTime & startTime,const QDateTime & endTime) = 0;

	// ֪ͨ��ͣ�ط�����
	// bPause:true-��ͣ������false-�ָ�����
	// ����ֵ:
	//	0:���óɹ�
	//	1:����ʧ��
	virtual int pausePlaybackStream(bool bPause) = 0;

	// ֹͣ��ǰ�ط�����
	// ����ֵ:
	//	0:���óɹ�
	//	1:����ʧ��
	virtual int stopPlaybackStream() = 0;
};

#endif