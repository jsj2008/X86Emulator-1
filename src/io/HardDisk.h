#ifndef HARDDISK_H_INCLUDED
#define HARDDISK_H_INCLUDED

class HardDisk{
public:

    void IO_Write_03F6(WORD data);	//��������ͨ��ջ���ݵģ�����Ҫ��WORD
    void IO_Write_03F7(WORD data);
    unsigned int getPoint();
    void setPoint(unsigned int point);

private:
    unsigned int HardDiskIOPoint;		//��ǰ��д�����ݵ�ָ�룬��DMAʹ��
    // �ڲ��Ĵ���
    unsigned int HdReg[4];		//��ǰ��д������ָ�롣  ��������ͷ�����桢������
    char HdCnt[4][3]={
        {0,16,63},
        {0,16,63},
        {0,16,63},
        {0,16,63},
    };		//Ӳ��������16�����棨��ͷ����63������������������Ӳ������������
}
#endif // HARDDISK_H_INCLUDED
