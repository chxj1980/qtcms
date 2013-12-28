#ifndef __IRECORDER_HEAD_FILE_ASDNVG8Y9ASDF__
#define __IRECORDER_HEAD_FILE_ASDNVG8Y9ASDF__
#include <libpcom.h>

//��ϸ�ӿ�˵���μ�doc\Interface\IRecorder.html

interface IRecorder : public IPComBase
{
	virtual int Start() = 0;

	virtual int Stop() = 0;

	virtual int InputFrame(int type,char *cbuf,int buffersize) = 0;

	enum _emError{
		OK = 0,          //�ɹ�
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};

};

/*
	����type���ͣ�
	REC_SYS_DATA 0x11 Ϊ��Ϣ����
	0x1��0x2Ϊ��Ƶ֡

	�¼�����
*/


#endif //__IRECORDER_HEAD_FILE_ASDNVG8Y9ASDF__