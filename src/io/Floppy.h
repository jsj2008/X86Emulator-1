#ifndef FLOPPY_H_INCLUDED
#define FLOPPY_H_INCLUDED

class Floppy{
public
    void IO_Write_03F0(WORD data);
    void IO_Write_03F1(WORD data);
    unsigned int getPoint();
    void setPoint(unsigned int point);
    bool isInsert(int num);

private
    unsigned int FloppyIOPoint; //��ǰ��д���ݵ�ָ��

    char FlpReg[4]; //��ǰ��д������ָ��
    char FlpCnt[3]={80,2,18};   //1.44MB��������80�����棬2�����棨��ͷ����18������
    char RegPointer=0;

    HANDLE hFloppy[4];				//һ�����̿������������4��������
    HANDLE hMap[4];
    unsigned int SizeFloppy[4];
    char *pBufFloppy[4];
    bool FloppyInserted[4];
};



#endif // FLOPPY_H_INCLUDED
