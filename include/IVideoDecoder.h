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
		��flags��:���ò�����־����λ���㣬����Ӧλ��λʱ����ʾ�ò������ã��������������
				0x00000001			DF_MASK_DATA
				0x00000002			DF_MASK_YDATA
				0x00000004			DF_MASK_UDATA
				0x00000008			DF_MASK_VDATA
				0x00000010			DF_MASK_WIDTH
				0x00000020			DF_MASK_HEIGHT
				0x00000040			DF_MASK_YSTRIDE
				0x00000080			DF_MASK_UVSTRIDE
				0x00000100			DF_MASK_LINESTRIDE
				0x00000200			DF_MASK_PIXELFOMAT
		"data":���ݵ�ַ
		"Ydata":��ƵY���ݵ�ַ
		"Udata":��ƵU���ݵ�ַ
		��Vdata":��ƵV���ݵ�ַ
		"width":��Ƶ֡��ȣ���λ����
		"height":��Ƶ֡�߶ȣ���λ����
		"YStride":Y�����п���λ�ֽ�
		"UVStride":UV�����п���λ�ֽ�
		"lineStride":�п������ظ�ʽΪRGB����YUV�����ʽʱʹ��
		"pixelFormat":���ظ�ʽ��YUV420_P-"YV12"
*/

#endif