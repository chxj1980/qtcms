#ifndef __DEVICE_CLIENT_HEAD_FILE_V898UY19ASYVB__
#define __DEVICE_CLIENT_HEAD_FILE_V898UY19ASYVB__
#include <libpcom.h>

interface IDeviceClient : public IPComBase
{
	// ���ӵ��豸
	// sAddress:�豸ip��ַ
	// uiPort:�豸�˿�
	// sEseeId:�豸id
	// ����ֵ:
	//	0:���ӳɹ�
	//  1:����ʧ��
	virtual int connectToDevice(const QString & sAddr,unsigned int uiPort,const QString & sEseeId) = 0;

	// У���û�������
	// sUsername:�û���
	// sPassword:����
	// ����ֵ:
	//	0:У��ɹ�
	//	1:У��ʧ��
	virtual int checkUser(const QString & sUsername,const QString & sPassword) = 0;

	// ���õ�ǰ������ͨ��������
	// sChannelName:ͨ������
	// ����ֵ:
	//	0:���óɹ�
	//	1:����ʧ��
	virtual int setChannelName(const QString & sChannelName) = 0;

	// ����ʵʱ����
	// nChannel:ͨ����
	// nStream:�������
	// bOpen:Ϊtrueʱ��ͨ��Ԥ����Ϊfalseʱ���ر�ͨ��Ԥ��
	// ����ֵ:
	//	0:����ɹ�
	//	1:����ʧ��
	virtual int liveStreamRequire(int nChannel,int nStream,bool bOpen) = 0;

	// �ر����ӣ���������Դ
	// ����ֵ:
	//	0:�رճɹ�
	//	1:�ر�ʧ��
	virtual int closeAll() = 0;

	// ��ȡ��ǰ�����Vendor���ƣ���ǰ����ֵΪ:"JUAN DVR"��"JUAN IPC","ONVIF"����
	virtual QString getVendor() = 0;

	// ��ȡ��ǰ������״̬
	// ����ֵ:
	//	0:δ����
	//	1:������
	//	2:��������
	//	3:���ڶϿ�
	virtual int getConnectStatus() = 0;
};

// Event
// @1 name:"PreviewStream"
// parameters��
// 	"channel":��ǰ֡��ͨ����
// 	"pts":��ǰ֡ʱ�������λΪ΢��
// 	"length":���ݳ���
// 	"data":����ָ��
// 	"frametype":֡���ͣ�ȡֵ'I','P','B','A'
// 	"width":��Ƶ֡�Ŀ���λ���أ��������Ƶ֡�������ݸò���
// 	"height":��Ƶ֡�ĸߣ���λ���أ��������Ƶ֡�������ݸò���
// 	"vcodec":��Ƶ֡�ı����ʽ����ǰ����ֵ:"H264"���������Ƶ֡�������ݸò���
// 	"samplerate":��Ƶ�����ʣ��������Ƶ֡�������ݸò���
// 	"samplewidth":��Ƶ����λ���������Ƶ֡�������ݸò���
// 	"audiochannel":��Ƶ�Ĳ���ͨ�������������Ƶ֡�������ݸò���
// 	"acodec":��Ƶ�����ʽ����ǰ����ֵ��"G711"���������Ƶ֡�������ݸò���
// @2 name:"RecordStream"
// parameters:
// "channel":��ǰ֡��ͨ����
// 	"pts":��ǰ֡ʱ�������λΪ΢��
// 	"length":���ݳ���
// 	"data":����ָ��
// 	"frametype":֡���ͣ�ȡֵ'I','P','B','A'
// 	"width":��Ƶ֡�Ŀ���λ���أ��������Ƶ֡�������ݸò���
// 	"height":��Ƶ֡�ĸߣ���λ���أ��������Ƶ֡�������ݸò���
// 	"vcodec":��Ƶ֡�ı����ʽ����ǰ����ֵ:"H264"���������Ƶ֡�������ݸò���
// 	"samplerate":��Ƶ�����ʣ��������Ƶ֡�������ݸò���
// 	"samplewidth":��Ƶ����λ���������Ƶ֡�������ݸò���
// 	"audiochannel":��Ƶ�Ĳ���ͨ�������������Ƶ֡�������ݸò���
// 	"acodec":��Ƶ�����ʽ����ǰ����ֵ��"G711"���������Ƶ֡�������ݸò���

#endif