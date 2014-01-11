#ifndef __IRECORDER_HEAD_FILE_ASDNVG8Y9ASDF__
#define __IRECORDER_HEAD_FILE_ASDNVG8Y9ASDF__
#include <libpcom.h>
#include <QtCore/QVariantMap>

//��ϸ�ӿ�˵���μ�doc\Interface\IRecorder.html

interface IRecorder : public IPComBase
{
	//typedef struct _tagFrameInfo{
	//	int type;
	//	char * pData;
	//	unsigned int uiDataSize;
	//	unsigned int uiTimeStamp;
	//}FrameInfo;

	virtual int Start() = 0;

	virtual int Stop() = 0;

	virtual int InputFrame(QVariantMap& frameinfo) = 0;

	virtual int SetDevInfo(const QString& devname,int nChannelNum) = 0;

	enum _emError{
		OK = 0,          //�ɹ�
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};

};

/*
	frameinfo������
	"frametype":֡���ͣ�ȡֵ'I'0x01,'P'0x02,'B','A'0x00
	"data":����ָ��
	"length":���ݳ���
	"pts":��ǰ֡64λʱ�������ȷ��΢��
	"gentime":֡�Ĳ����¼�����λΪ�룬ΪGMTʱ��
	
	"channel":��ǰ֡��ͨ���ţ��������Ƶ֡�������ݸò���
	"width":��Ƶ֡�Ŀ���λ���أ��������Ƶ֡�������ݸò���
	"height":��Ƶ֡�ĸߣ���λ���أ��������Ƶ֡�������ݸò���
	"vcodec":��Ƶ֡�ı����ʽ����ǰ����ֵ:"H264"���������Ƶ֡�������ݸò���
	
	"samplerate":��Ƶ�����ʣ��������Ƶ֡�������ݸò���
	"samplewidth":��Ƶ����λ���������Ƶ֡�������ݸò���
	"audiochannel":��Ƶ�Ĳ���ͨ�������������Ƶ֡�������ݸò���
	"acodec":��Ƶ�����ʽ����ǰ����ֵ��"G711"���������Ƶ֡�������ݸò���
*/


#endif //__IRECORDER_HEAD_FILE_ASDNVG8Y9ASDF__