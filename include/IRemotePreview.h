#ifndef __IPREVIEWCAM_HEAD_FILE_NS89VY9ASUD8__
#define __IPREVIEWCAM_HEAD_FILE_NS89VY9ASUD8__
#include <libpcom.h>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

interface IRemotePreview : public IPComBase
{
	virtual int getLiveStream(int nChannel, int nStream) = 0;
	virtual int stopStream() = 0;
	virtual int pauseStream(bool bPaused) = 0;
	virtual int getStreamCount() = 0;
	virtual int getStreamInfo(int nStreamId,QVariantMap &streamInfo) = 0;
};

/*
Event:
@1	name: "LiveStream"
	parameters:
		"channel":��ǰ֡��ͨ����
		"pts":��ǰ֡ʱ�������λΪ΢��
		"length":���ݳ���
		"data":����ָ��
		"frametype":֡���ͣ�ȡֵ'I','P','B','A'
		"width":��Ƶ֡�Ŀ���λ���أ��������Ƶ֡�������ݸò���
		"height":��Ƶ֡�ĸߣ���λ���أ��������Ƶ֡�������ݸò���
		"vcodec":��Ƶ֡�ı����ʽ����ǰ����ֵ:"H264"���������Ƶ֡�������ݸò���
		"samplerate":��Ƶ�����ʣ��������Ƶ֡�������ݸò���
		"samplewidth":��Ƶ����λ���������Ƶ֡�������ݸò���
		"audiochannel":��Ƶ�Ĳ���ͨ�������������Ƶ֡�������ݸò���
		"acodec":��Ƶ�����ʽ����ǰ����ֵ��"G711"���������Ƶ֡�������ݸò���
*/


#endif