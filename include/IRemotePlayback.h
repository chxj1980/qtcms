#ifndef __IREMOTEPLAYBACK_HEAD_FILE_OASDF892389HUDSAF80BAS__
#define __IREMOTEPLAYBACK_HEAD_FILE_OASDF892389HUDSAF80BAS__
#include <libpcom.h>

interface IRemotePlayback : public IPComBase
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

	// ͨ��¼���ļ�������ȡ����
	// nChannel:ͨ����
	// sFileName:¼���ļ���
	// ����ֵ
	//	0:���óɹ�
	//	1:����ʧ��
	//	2:��������
	virtual int getPlaybackStreamByFileName(int nChannel,const QString &sFileName) = 0;

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

// 	@3 name "recStream"
// parameters:
// 	"length":���������ݳ���
// 	"frametype":֡���ͣ�0Ϊ��Ƶ��1Ϊ��ƵI֡��2Ϊ��ƵP֡
// 	"channel":��ǰ����֡��ͨ���ţ���0��ʼ����
// 	"width":��Ƶ֡�Ŀ��������Ƶ֡����Ըò���
// 	"height":��Ƶ֡�ĸߣ��������Ƶ֡����Ըò���
// 	"framerate":��Ƶ֡��֡�ʣ��������Ƶ֡����Ըò���
// 	"audioSampleRate":��Ƶ֡�Ĳ����ʣ��������Ƶ֡����Ըò���
// 	"audioFormat":��Ƶ֡�ı����ʽ�������ַ�����ʾ����"g711"��
// 	"audioDataWidth":��Ƶ֡�Ĳ���λ���������Ƶ֡����Ըò���
// 	"pts":64λʱ�������ȷ��΢��
// 	"gentime":֡�Ĳ����¼�����λΪ�룬ΪGMTʱ��
// 	"data":�������ݵĵ�ַ


#endif