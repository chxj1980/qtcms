#ifndef __ISWITCHSTREAM_HEAD_FILE_V898UY19ASYVB__
#define __ISWITCHSTREAM_HEAD_FILE_V898UY19ASYVB__
#include <libpcom.h>

interface ISwitchStream : public IPComBase
{

	//�л�����
	//StreamNum:�����ţ�1�������������0������������
	//����ֵ��
	//0:�л��ɹ�
	//1:�л�ʧ��
	virtual int SwitchStream(int StreamNum)=0;

};

#endif