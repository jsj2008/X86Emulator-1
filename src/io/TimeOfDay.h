#ifndef TIMEOFDAY_H_INCLUDED
#define TIMEOFDAY_H_INCLUDED

#include "ioport.h"
#define TICK 54.9255

Class TimeOfDay:IOPort
{
public
    void write2Port(u32 value,Memory& memory,RegisterFile& registerFile);
    u32 readFromPort(Memory& memory,RegisterFile& registerFile);

private
    //������ ���롢�롢�֡�ʱ���ա��¡��ꡢ����
    unsigned char rTime[8];		//ʮ�����ƴ�ţ���������
    unsigned char BCDTIme[8];	//BDC����
    unsigned int *RtcTick;
    unsigned char *RtcMidNight;
    char carry[7]={100,60,60,24,31,12,100};		//�˸��Ĵ���֮��Ľ�λ��ϵ

    unsigned int RWpnt;
   // bool A20Gate;								//A20��ַ�ߣ�ֻ��Ϊ��92�˿��и���������ʵ�����Ǵ򿪵ġ�
    int t;
}




#endif // TIMEOFDAY_H_INCLUDED
