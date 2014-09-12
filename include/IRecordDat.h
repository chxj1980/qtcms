#ifndef __IRECORDDAT_HEAD_FILE__
#define __IRECORDDAT_HEAD_FILE__
#include "libpcom.h"
#include <QtCore/QVariantMap>
//��ϸ�ӿ�˵���μ�doc\Interface\IRecordDat.html
interface IRecordDat:public IPComBase{
	//¼���ʼ��
	virtual bool init(int nWnd)=0;
	//�˳�¼��
	virtual bool deinit()=0;
	//���� ¼������
	virtual int  inputFrame(QVariantMap &tFrameInfo)=0;

	//�ֶ�¼��
	//�����ֶ�¼��
	virtual bool manualRecordStart()=0;
	//�ر��ֶ�¼��
	virtual bool manualRecordStop()=0;

	//�ƶ�¼��
	//�����ƶ�¼��
	//nTime:�ƶ�¼���źŵĳ���ʱ��
	virtual bool motionRecordStart(int nTime)=0;

	//��ȡ¼��״̬
	//����ֵ��λ���㣺�ƶ���� �ֶ�¼�� ��ʱ¼�񣬸�λ���ȼ����ڵ�λ������ 110��ʾ �ƶ���⣬010 ��ʾ �ֶ�¼��
	virtual int getRecordStatus()=0;

	//����¼���ճ̱�
	//nChannelId:ͨ�����ճ̱���id��
	virtual bool updateRecordSchedule(int nChannelId)=0;
	//����ϵͳ���ݿ�
	virtual bool upDateSystemDatabase()=0;
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
	"winid":���ں�
	"width":��Ƶ֡�Ŀ���λ���أ��������Ƶ֡�������ݸò���
	"height":��Ƶ֡�ĸߣ���λ���أ��������Ƶ֡�������ݸò���
	"vcodec":��Ƶ֡�ı����ʽ����ǰ����ֵ:"H264"���������Ƶ֡�������ݸò���
	
	"samplerate":��Ƶ�����ʣ��������Ƶ֡�������ݸò���
	"samplewidth":��Ƶ����λ���������Ƶ֡�������ݸò���
	"audiochannel":��Ƶ�Ĳ���ͨ�������������Ƶ֡�������ݸò���
	"acodec":��Ƶ�����ʽ����ǰ����ֵ��"G711"���������Ƶ֡�������ݸò���
*/
#endif