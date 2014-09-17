#ifndef RECORDDAT_GLOBAL_H
#define RECORDDAT_GLOBAL_H

#include <vld.h>
#include <QtCore/qglobal.h>
#define MANUALRECORD 2
#define MOTIONRECORD 4
#define TIMERECORD 1
#define  WNDMAXSIZE 64
#define  IFRAME 0x01
#define  PFRAME 0x02
#define  AFRMAE 0x00
#define  BUFFERSIZE 120//��λ��M
typedef struct __tagFrameHead{
	unsigned int uiType;
	unsigned int uiLength;
	unsigned int uiChannel;//���ں�
	unsigned int uiPts;
	unsigned int uiGentime;
	unsigned int uiRecType;
	unsigned int uiExtension;
	unsigned int uiSessionId;
	char *pBuffer;
}tagFrameHead;

typedef struct __tagVideoConfigFrame{
	unsigned int uiWidth;
	unsigned int uiHeight;
	unsigned char ucVideoDec[4];
	unsigned char ucReversed[4];
}tagVideoConfigFrame;
typedef struct __tagAudioConfigFrame{
	unsigned int uiSamplebit;
	unsigned int uiSamplerate;
	unsigned int uiChannels;
	unsigned char ucAudioDec[4];
}tagAudioConfigFrame;
typedef struct __tagIFrameIndex{
	unsigned int uiFirstIFrame[64];//I֡������֡λ��
}tagIFrameIndex;
typedef struct __tagFileHead{
	unsigned char ucMagic[4];//JUAN
	unsigned int uiVersion;
	unsigned int uiChannels[2];
	unsigned int uiStart;
	unsigned int uiEnd;
	unsigned int uiIndex;
	tagIFrameIndex tIFrameIndex;
}tagFileHead;
typedef struct __tagPerFrameIndex{
	unsigned int uiPreFrame;
	unsigned int uiPreIFrame;//I֡������֡λ��
	unsigned int uiNextFrame;
	unsigned int uiNextIFrame;//I֡������֡λ��
}tagPerFrameIndex;
typedef struct __tagFileFrameHead{
	tagPerFrameIndex tPerFrameIndex;
	tagFrameHead tFrameHead;
}tagFileFrameHead;
typedef enum __tagFrameType{
	FT_Audio,//00
	FT_IFrame,//01
	FT_PFrame,//02
	FT_BFrame,
	FT_AudioConfig,
	FT_VideoConfig
}tagFrameType;
typedef enum __tagObtainFilePathStepCode{
	obtainFilePath_getDrive,//��ȡ��¼���̷�
	obtainFilePath_diskUsable,//��ʣ��ռ�Ŀ�¼����̷�
	obtainFilePath_diskFull,//ÿ���̷����Ѿ�¼��
	obtainFilePath_createFile,//����ļ������ڣ��򴴽��ļ�
	obtainFilePath_success,//��ȡ¼���ļ�·���ɹ�
	obtainFilePath_fail,//��ȡ¼���ļ�·��ʧ��
	obtainFilePath_end//����
}tagObtainFilePathStepCode;
#endif // RECORDDAT_GLOBAL_H
