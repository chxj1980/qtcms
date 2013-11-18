#ifndef __IPREVIEWWINDOW_HEAD_FILE_NASV9123400SDV__
#define __IPREVIEWWINDOW_HEAD_FILE_NASV9123400SDV__
#include <libpcom.h>

interface IVideoDecoder : public IPComBase
{
	virtual int init(int nWidth,int nHeight) = 0;
	virtual int deinit() = 0;
	virtual int decode(char * pData,unsigned int nDataLength) = 0;
	virtual int flushDecoderBuffer() = 0;
};

/*
Event:
@1	name: "DecodedFrame"
	parameters:
		"Ydata":Y����
		"Udata":U����
		"Vdata":V����
		"width":��Ƶ֡��ȣ���λ����
		"height":��Ƶ֡�߶ȣ���λ����
		"YStride":Y�����п���λ�ֽ�
		"UVStride":UV�����п���λ�ֽ�
*/

#endif