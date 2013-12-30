
typedef struct _tagDiskInfo{
	char cDiskName;
	unsigned long long ullFreeBytesToCaller;
	unsigned long long ullTotalBytes;
	unsigned long long ullFreeBytes;
}DiskInfo;

extern int g_diskNum;
extern DiskInfo g_allDiskInof[30];

//��ȡ�߼��̵������̷�
//dvrStrΪ����ַ���
//��ȡ�ɹ������ַ������ȣ����򷵻�0
int getLogicalDriveStrings(char *pstrDriveStr);

//��ȡָ������Ϣ�������ܴ�С�����ÿռ�
//pstrDiskΪ�����̷����磺��D:\��
//ullFreeBytesToCallerΪ�ɻ�õĿ��пռ䣨�ֽڣ�
//ullTotalBytesΪ�������������ֽڣ�
//ullFreeBytesΪ"���пռ䣨�ֽڣ�
//��ȡ�ɹ�����true������Ϊfalse
bool getDiskFreeSpaceEx(const char *pstrDisk, 
						unsigned long long &ullFreeBytesToCaller,
						unsigned long long &ullTotalBytes,
						unsigned long long &ullFreeBytes);

//���������̵���Ϣ�����浽g_allDiskInof��
//����ɹ�����true�����򷵻�false
bool saveAllDistInfo();

//��ȡ�����̵���Ϣ�����浽g_allDiskInof��
//��ȡ�ɹ������̵ĸ����������diskInfo���������򷵻�0
int getAllDiskInfo(DiskInfo *diskInfo);