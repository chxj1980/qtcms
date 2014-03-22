#ifndef __IDEVICEREMOTEPLAYBACK_HEAD_FILE_SADVN98ASD7YASGV__
#define __IDEVICEREMOTEPLAYBACK_HEAD_FILE_SADVN98ASD7YASGV__
#include <libpcom.h>
#include <QtGui/QWidget>
#include <QtCore/QDateTime>
// IDeviceGroupRemotePlayback�ӿ����ڿ���һ��ͨ��ͬ������

interface IDeviceGroupRemotePlayback : IPComBase
{
	// ��ͨ����ӵ�ͬ���飬����ָ��ͨ��nChannel�ڴ���wnd�ڲ���
	// ����ֵ:
	//	0:��ӳɹ�
	//	1:���������޷���Ӹ���ͨ��
	virtual int AddChannelIntoPlayGroup(int nChannel,QWidget * wnd) = 0;

	// �����ط�
	// nTypes:¼�����ͣ���λ���㣬��0λ��ʾ��ʱ¼�񣬵�1λ��ʾ�ƶ����¼�񣬵�2λ��ʾ����¼�񣬵�3λ��ʾ�ֶ�¼��
	// startTime:��ʼʱ��
	// endTime:����ʱ��
	// ����ֵ:
	//	0:���óɹ�
	//	1:�����ж�
	//	2:��������
	virtual int GroupPlay(int nTypes,const QDateTime & start,const QDateTime & end) = 0;

	// ��ȡ��ǰ����ʱ���
	virtual QDateTime GroupGetPlayedTime() = 0;

	// ��ͣ����
	virtual int GroupPause() = 0;

	// ��������
	virtual int GroupContinue() = 0;

	// ֹͣ����
	virtual int GroupStop() = 0;

	// ����/�ر���Ƶ
	// ����ֵ������֮ǰ����Ƶ����״̬
	virtual bool GroupEnableAudio(bool bEnable) = 0;

	//�趨ָ�����ڵ�����
	virtual int GroupSetVolume(unsigned int uiPersent, QWidget* pWnd) = 0;

	// ���
	virtual int GroupSpeedFast() = 0;

	// ����
	virtual int GroupSpeedSlow() = 0;

	// �ָ������ٶȲ���
	virtual int GroupSpeedNormal() = 0;
};

#endif