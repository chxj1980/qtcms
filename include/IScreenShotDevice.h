#ifndef __IPROTOCOLPTZ_HEAD_FILE_D9WPC23AJ8sdfsdgsdgKW54EG0SYE__
#define __IPROTOCOLPTZ_HEAD_FILE_D9WPC23AJ8sdfsdgsdgKW54EG0SYE__

#include "libpcom.h"


interface IScreenShotDevice : public IPComBase
{
	//user:��ǰ�û���nType:�������ͣ�0��Ԥ����1�����أ�2��Զ�̣�,nChl:�������ں�
	virtual void screenShot(QString sUser,int nType,int nChl)=0;

};

#endif