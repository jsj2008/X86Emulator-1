#include "DMA.h"

DMA::DMA(IOPortList ioPortList)
{
    HardDisk hardDisk=new HardDisk();
    Floppy floppy=new Floppy();
    ioPortList.add2PortList(02h,this);

}

//��дһ���˿�0,����ѡ��ͨ��
//��д�ĸ��˿�1,�����趨�ڴ�����ʼ��ַ
//��д�ĸ��˿�1,�����趨Ҫ������ֽ���
void IO_Write_0000(WORD data)
{
	CurChn=&(ChnReg[(BYTE)data][0][0]);
}

void IO_Write_0001(WORD data)
{
	*CurChn=(BYTE)data;
	CurChn++;
}

void IO_Write_0002(WORD data)
{
	ModReg=(BYTE)data;
	char tChn=ModReg & 0x03;
	char tReadWrite=(ModReg >> 2) & 0x03;
	char tDec=(ModReg >> 5) & 0x01;
	char tMode=(ModReg >> 6) & 0x03;

	switch(tChn)
	{
	case 2:
		tAd=floppy.getPoint();		//����ռ��DMAͨ��2
		break;
	case 3:
		tAd=hardDisk.getPoint();	//Ӳ��ռ��DMAͨ��3
		break;
	default:
		return;
	}

	switch(tReadWrite)
	{
	case 1:
		Des=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][2]);
		Des<<=4;														//��ȡ�λ���
		Des+=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][0]);		//��ȡ����ƫ��
		Des+=MemoryStart;
		Src=tAd;
		break;
	case 2:
		Src=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][2]);
		Src<<=4;														//��ȡ�λ���
		Src+=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][0]);		//��ȡ����ƫ��
		Src+=MemoryStart;
		Des=tAd;
		break;
	}

	memcpy((void *)Des,(const void *)Src,*(unsigned int *)(&ChnReg[tChn][1][0]));

}



//�²⣬�жϷ���������Flags�Ĵ������޸�Ӧ�����޸�ջ���flags
void write2Port(u32 value,Memory& memory,RegisterFile& registerFile)
{
	int t;
	switch(registerFile.getGPR8BitsHigh(RAX)) //AH
	{
	case 0:
		(*(unsigned short*)((t=registerFile.getSR(SS),t<<=4)+registerFile.getGPR16Bits(RSP)+4+memory.getMemoryAddress())) &= ~CF;
        registerFile.setGPR8BitsHigh(RAX,0);     //AH=0;
		break;
	case 1:
		if (registerFile.getGPR8BitsLow(RDX)<0x80)   //DL
			registerFile.setGPR8BitsHigh(RAX,0);    //AH=0;
		else
			registerFile.setGPR8BitsHigh(RAX,0x80); //AH=0x80;
		break;
	case 2:
	case 3:
		if (floppy.isInsert(registerFile.getGPR8BitsLow(RDX)])&& registerFile.getGPR8BitsLow(RDX) <0x80 || registerFile.getGPR8BitsLow(RDX) >=0x80)
		//if (1)
		{
			if (registerFile.getGPR8BitsLow(RDX)<0x80)
			{
				floppy.IO_Write_03F0(0);
				floppy.IO_Write_03F1(registerFile.getGPR8BitsLow(RCX)&0x3f);    //CL
				floppy.IO_Write_03F0(1);
				floppy.IO_Write_03F1(registerFile.getGPR8BitsHigh(RDX)); //DH
				floppy.IO_Write_03F0(2);
				floppy.IO_Write_03F1(registerFile.getGPR8BitsHigh(RCX)+((registerFile.getGPR8BitsLow(RCX)&0xc0)<<2));
				floppy.IO_Write_03F0(3);
				floppy.IO_Write_03F1(registerFile.getGPR8BitsLow(RDX)); //DL
				IO_Write_0000(2);					//ѡ��ͨ��
			}
			else
			{
				hardDisk.IO_Write_03F6(0);
				hardDisk.IO_Write_03F7(registerFile.getGPR8BitsLow(RCX)&0x3f);    //CL
				hardDisk.IO_Write_03F6(1);
				hardDisk.IO_Write_03F7(registerFile.getGPR8BitsHigh(RDX)); //DH
				hardDisk.IO_Write_03F6(2);
				hardDisk.IO_Write_03F7(registerFile.getGPR8BitsHigh(RCX)+((registerFile.getGPR8BitsLow(RCX)&0xc0)<<2));
				hardDisk.IO_Write_03F6(3);
				hardDisk. IO_Write_03F7(registerFile.getGPR8BitsLow(RDX)-0x80);    //DL
				IO_Write_0000(3);					//ѡ��ͨ��
			}
			IO_Write_0001(registerFile.getGPR8BitsLow(RBX));				//BL    �ڴ����ʼ��ַ
			IO_Write_0001(registerFile.getGPR8BitsHigh(RBX)); //BH
			IO_Write_0001(registerFile.getSR(ES)&0xff);    //ES
			IO_Write_0001((registerFile.getSR(ES)&0xff00)>>8);
			IO_Write_0001(0);					//������ֽ�������������*200h
			IO_Write_0001(registerFile.getGPR8BitsLow(RAX)*2);   //AL
			IO_Write_0001((registerFile.getGPR8BitsLow(RAX)&0x80)>>7);   //AL
			IO_Write_0001(0);
			if (registerFile.getGPR8BitsLow(RDX)<0x80)   //DL
			{										//����
				if (registerFile.getGPR8BitsHigh(RAX)) //AH
					IO_Write_0002(0x96);			//������
				else
					IO_Write_0002(0x9a);			//д����
			}
			else
			{										//Ӳ��
				if (registerFile.getGPR8BitsHigh(RAX)) //AH
					IO_Write_0002(0x97);			//������
				else
					IO_Write_0002(0x9b);			//д����
			}
		}
		else
		{
			(*(unsigned short*)((t=registerFile.getSR(SS),t<<=4)+registerFile.getGPR16Bits(RSP)+4+memory.getMemoryAddress()))|=CF;
			registerFile.setGPR8BitsHigh(RAX)=0x80;   //AH
		}
		break;
	case 4:
		break;
	case 8:
		if (registerFile.getGPR8BitsLow(RDX)<0x80)   //DL
		{
			(*(unsigned short*)((t=registerFile.getSR(SS),t<<=4)+registerFile.getGPR16Bits(RSP)+4+memory.getMemoryAddress()))&=~CF;
			registerFile.setGPR8BitsHigh(RAX,0);  //AH
			registerFile.setGPR8BitsLow(RBX,4);  //BL
			registerFile.setGPR16Bits(RCX,0x4f12); //CX
			registerFile.setGPR16Bits(RDX,0x0102);  //DX
			eCPU.es=*(unsigned short*)(memory.getMemoryAddress()+0x1e*4+2);	//INT 1Eh,���̲�����
			eCPU.di=*(unsigned short*)(memory.getMemoryAddress()+0x1e*4);
		}
		else
		{
			if (*(unsigned char *)(memory.getMemoryAddress()+0x475))
			{												//��Ӳ��
				(*(unsigned short*)((t=registerFile.getSR(SS),t<<=4)+registerFile.getGPR16Bits(RSP)+4+memory.getMemoryAddress()))&=~CF;
				registerFile.setGPR8BitsHigh(RAX,0);  //AH
				registerFile.setGPR8BitsHigh(RCX,(HdCnt[registerFile.getGPR8BitsLow(RDX)-0x80][0]&0xff)-2);    //CH
				registerFile.setGPR8BitsLow(RCX,63+((HdCnt[registerFile.getGPR8BitsLow(RDX)-0x80][0]&0x300)>>2)); //CL
				registerFile.setGPR8BitsHigh(RDX,HdCnt[registerFile.getGPR8BitsLow(RDX)-0x80][1]-1);
				registerFile.setGPR8BitsLow(RDX,*(unsigned char *)(memory.getMemoryAddress()+0x475));
// 				eCPU.es=*(unsigned short*)(MemoryStart+0x1e*4+2);	//INT 1Fh,���̲�����
// 				eCPU.di=*(unsigned short*)(MemoryStart+0x1e*4);
			}
			else
			{												//��Ӳ��
				(*(unsigned short*)((t=registerFile.getSR(SS),t<<=4)+registerFile.getGPR16Bits(RSP)+4+memory.getMemoryAddress()))|=CF;
				registerFile.setGPR8BitsHigh(RAX,0x1);    //AH
			}
		}
		break;
	case 0x10:
		break;
	case 0x15:
		if (registerFile.getGPR8BitsLow(RDX)<0x02)		//��ʱֻ֧����������
		{
			(*(unsigned short*)((t=registerFile.getSR(SS),t<<=4)+registerFile.getGPR16Bits(RSP)+4+memory.getMemoryAddress()))&=~CF;
			registerFile.setGPR8BitsHigh(RAX,1); //AH
		}
		else
		{
			(*(unsigned short*)((t=registerFile.getSR(SS),t<<=4)+registerFile.getGPR16Bits(RSP)+4+memory.getMemoryAddress()))&=~CF;
			registerFile.setGPR8BitsHigh(RAX,0);  //AH
		}
		break;
	}
}

