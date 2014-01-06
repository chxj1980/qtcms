// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� NETLIB_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// NETLIB_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifndef NETLIB_H
#define NETLIB_H

#define AF_INET         2  /* internetwork: UDP, TCP, etc. */

extern "C"{
	unsigned short htonsQ(unsigned short hostshort);
	unsigned long htonlQ(unsigned long hostlong);

	void SleepQ(unsigned long hostshort);
	unsigned long GetTickCountQ();

	unsigned long GetServerAddr(const char* hostname);

	bool GetDiskFreeSpaceExQ(char* diskname,
					unsigned long long* lpFreeBytesAvailableToCaller,
					unsigned long long* lpTotalNumberOfBytes,
					unsigned long long* lpTotalNumberOfFreeBytes);
};

#endif