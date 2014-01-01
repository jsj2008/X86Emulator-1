#include "HardDisk.h"
#include <stdlib.h>

HardDisk::HardDisk()
{

	char t[80];
	char iniRet[80];
	DWORD tDword;
	GetCurrentDirectory(sizeof(t),t);
	strcat(t,"\\setup.ini");
	GetPrivateProfileString("IMG","C","",iniRet,sizeof(iniRet),t);
	tDword=GetLastError();
	if (iniRet[0]!=0)
	{
		hHardDisk[0]=CreateFile(iniRet, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		SizeHardDisk[0]=GetFileSize(hHardDisk[0],&tDword);
		HdhMap[0]=CreateFileMapping(hHardDisk[0],NULL,PAGE_READWRITE,0,SizeHardDisk[0],"HardDiskAMapping");
		pBufHardDisk[0]=(char *)MapViewOfFile(HdhMap[0],FILE_MAP_ALL_ACCESS,0,0,SizeHardDisk[0]);
		//HdCnt[0][0]=SizeHardDisk[0]/512/63/16;
		*(unsigned char *)(MemoryStart+0x474)=0;			// Ӳ����һ�β���״̬��Ĭ����1��
		*(unsigned char *)(MemoryStart+0x475)=1;			// Ӳ�̸���+1
	}
	GetPrivateProfileString("IMG","spt","",iniRet,sizeof(iniRet),t);
	HdCnt[0][2]=atoi(iniRet);
	GetPrivateProfileString("IMG","heads","",iniRet,sizeof(iniRet),t);
	HdCnt[0][1]=atoi(iniRet);
	GetPrivateProfileString("IMG","cylinders","",iniRet,sizeof(iniRet),t);
	HdCnt[0][0]=atoi(iniRet);

}

unsigned int getPoint()
{
    return HardDiskIOPoint;
}

void setPoint(unsigned int point)
{
    HardDiskIOPoint=point;
}

void IO_Write_03F6(WORD data)	//��������ͨ��ջ���ݵģ�����Ҫ��WORD
{
	HdRegPointer=(BYTE)data;
}

void IO_Write_03F7(WORD data)
{
	HdReg[HdRegPointer]=(BYTE)data;
	HardDiskIOPoint=0;
	for (int i=0;i<3;i++)
	{
		HardDiskIOPoint*=HdCnt[HdReg[3]][i];		//HdCnt[][0]���Ǻ�0��˵�
		HardDiskIOPoint+=HdReg[2-i];
	}
	HardDiskIOPoint-=1;					//������1��ʼ��ţ����Լ���ƫ�Ƶ�ʱ��Ҫ�ȼ�1
	HardDiskIOPoint*=512;					//ÿ������512���ֽ�
	HardDiskIOPoint+=(unsigned int)pBufHardDisk[HdReg[3]];
}


