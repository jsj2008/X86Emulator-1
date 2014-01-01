#ifndef DMA_H_INCLUDED
#define DMA_H_INCLUDED

#include "memory/memory.h"
#include "Floppy.h"
#include "HardDisk.h"
#include "ioport.h"
#include <memory.h>

class DMA:IOPort{
public
    void write2Port(u32 value,Memory& memory,RegisterFile& registerFile);
    u32 readFromPort(Memory& memory,RegisterFile& registerFile);

public
    void IO_Write_0000(WORD data);
    void IO_Write_0001(WORD data);
    void IO_Write_0002(WORD data);

private
    //	I/O
    //	0000(byte)	--	ѡ��ͨ��
    //	0001(byte)	--	����д�ĸ��ֽڵ���ʼ��ַ��������д�ĸ��ֽڵ��ֽ������ӵ�λ����λ
    //	0002(byte)	--	ģʽ�Ĵ������ɶ�д��������д���ݺ�������ʼ���͡�
    // �ڲ��Ĵ���
    char ChnReg[4][2][4];		//��ͨ������ʼ��ַ���ֽ��������ֽڵļĴ���
    char CmdReg;				//����Ĵ���
    char ModReg;				//ģʽ�Ĵ���
    char *CurChn;				//I/O 1ͨ�� Ҫд�ļĴ�����ָ��
    unsigned int Src,Des,tAd;				//���ݸ��Ƶ���ʼ��ַ���ڲ�ʹ��
    unsigned int BlockLen;		//���ݸ��Ƶ��ֽ������ڲ�ʹ��
    HardDisk hardDisk;
    Floppy floppy;


};


#endif // DMA_H_INCLUDED
