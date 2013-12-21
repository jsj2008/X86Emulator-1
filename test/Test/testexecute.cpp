#include "testexecute.h"
#include "cpu/executer/operand/execgpregisteroperand.h"
#include "cpu/executer/operand/execimmediateoperand.h"
#include "cpu/executer/operand/execmemoryoperand.h"
#include "cpu/executer/operand/execsegmentregisteroperand.h"

TestExecute::TestExecute(QObject *parent) :
    QObject(parent)
{
}

void TestExecute::testADD()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(0x11,DATA_SIZE_BYTE);
        executeADD(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(int(registerFile.getGPR16Bits(RAX)),0x11);
    }
    {
        fillRegisterFile(registerFile);
        //signed extent case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(0x81,DATA_SIZE_BYTE);
        executeADD(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(int(registerFile.getGPR16Bits(RAX)),0xff81);
    }
    {
        fillRegisterFile(registerFile);
        //flags case 1.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x8123);
        ExecImmediateOperand src(0x8123,DATA_SIZE_WORD);
        executeADD(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(int(registerFile.getGPR16Bits(RAX)),0x0246);
        QCOMPARE(registerFile.getFlagsBits().CF,(unsigned int)(1));
        QCOMPARE(registerFile.getFlagsBits().PF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().AF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().ZF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().SF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().OF,(unsigned int)(1));
    }
    {
        fillRegisterFile(registerFile);
        //flags case 2.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x7f23);
        ExecImmediateOperand src(0x8123,DATA_SIZE_WORD);
        executeADD(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(int(registerFile.getGPR16Bits(RAX)),0x0046);
        QCOMPARE(registerFile.getFlagsBits().CF,(unsigned int)(1));
        QCOMPARE(registerFile.getFlagsBits().PF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().AF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().ZF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().SF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().OF,(unsigned int)(0));
    }
    {
        fillRegisterFile(registerFile);
        //flags case 3.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x4123);
        ExecImmediateOperand src(0x4123,DATA_SIZE_WORD);
        executeADD(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(int(registerFile.getGPR16Bits(RAX)),0x8246);
        QCOMPARE(registerFile.getFlagsBits().CF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().PF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().AF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().ZF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().SF,(unsigned int)(1));
        QCOMPARE(registerFile.getFlagsBits().OF,(unsigned int)(1));
    }
    {
        fillRegisterFile(registerFile);
        //flags case 4.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x8000);
        ExecImmediateOperand src(0x8000,DATA_SIZE_WORD);
        executeADD(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(int(registerFile.getGPR16Bits(RAX)),0x0000);
        QCOMPARE(registerFile.getFlagsBits().CF,(unsigned int)(1));
        QCOMPARE(registerFile.getFlagsBits().PF,(unsigned int)(1));
        QCOMPARE(registerFile.getFlagsBits().AF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().ZF,(unsigned int)(1));
        QCOMPARE(registerFile.getFlagsBits().SF,(unsigned int)(0));
        QCOMPARE(registerFile.getFlagsBits().OF,(unsigned int)(1));
    }

}

void TestExecute::testPOP()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //16bits data and 16bits address case pop to rax.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        memory.startAccess(memory.DEBUG_ACCESS);
        memory.set16Bits(registerFile.getSSR(SS)+registerFile.getGPR16Bits(RSP),0x1234);
        memory.endAccess();
        executePOP(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);
        QCOMPARE(u32(registerFile.getGPR16Bits(RAX)),u32(0x1234));
        QCOMPARE(u32(registerFile.getGPR16Bits(RSP)),u32(0x4446));
    }
    {
        fillRegisterFile(registerFile);
        //16bits data and 16bits address case pop to ds

        ExecSegmentRegisterOperand dest(registerFile,DS);
        memory.startAccess(memory.DEBUG_ACCESS);
        memory.set16Bits(registerFile.getSSR(SS)+registerFile.getGPR16Bits(RSP),0x1234);
        memory.endAccess();
        executePOP(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);
        QCOMPARE(u32(registerFile.getSR(DS)),u32(0x1234));
        QCOMPARE(u32(registerFile.getSSR(DS)),u32(0x12340));
        QCOMPARE(u32(registerFile.getGPR16Bits(RSP)),u32(0x4446));
    }
    {
        fillRegisterFile(registerFile);
        //32bits data and 16bits address case pop to ds
        ExecGPRegisterOperand dest(registerFile,RDX,DATA_SIZE_DWORD,false);

        memory.startAccess(memory.DEBUG_ACCESS);
        memory.set32Bits(registerFile.getSSR(SS)+registerFile.getGPR16Bits(RSP),0x12345678);
        memory.endAccess();
        executePOP(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(registerFile.getGPR32Bits(RDX)),u32(0x12345678));
        QCOMPARE(u32(registerFile.getGPR16Bits(RSP)),u32(0x4448));
    }
}

void TestExecute::testPUSH()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //16bits data and 16bits address case push from rax.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x1234);

        executePUSH(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(registerFile.getGPR16Bits(RSP)),u32(0x4442));

        memory.startAccess(memory.DEBUG_ACCESS);
        QCOMPARE((u32)memory.get16Bits(registerFile.getSSR(SS)+registerFile.getGPR16Bits(RSP)),
                 u32(0x1234));
        memory.endAccess();

    }
    {
        fillRegisterFile(registerFile);
        //16bits data and 16bits address case push from es.
        ExecSegmentRegisterOperand dest(registerFile,ES);

        executePUSH(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(registerFile.getGPR16Bits(RSP)),u32(0x4442));

        memory.startAccess(memory.DEBUG_ACCESS);
        QCOMPARE((u32)memory.get16Bits(registerFile.getSSR(SS)+registerFile.getGPR16Bits(RSP)),
                 u32(0x0000));
        memory.endAccess();

    }
    {
        fillRegisterFile(registerFile);
        //32bits data and 16bits address case push from es.
        ExecGPRegisterOperand dest(registerFile,RBX,DATA_SIZE_DWORD,false);

        executePUSH(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(registerFile.getGPR16Bits(RSP)),u32(0x4440));

        memory.startAccess(memory.DEBUG_ACCESS);
        QCOMPARE((u32)memory.get32Bits(registerFile.getSSR(SS)+registerFile.getGPR16Bits(RSP)),
                 u32(0x33333333));
        memory.endAccess();

    }
}

void TestExecute::testOR()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(0x11,DATA_SIZE_BYTE);
        executeOR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(int(registerFile.getGPR16Bits(RAX)),0x11);
    }
    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RSP,DATA_SIZE_DWORD,false);
        ExecImmediateOperand src(0x81,DATA_SIZE_BYTE);
        executeOR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(u32(registerFile.getGPR32Bits(RSP)),u32(0xffffffc5));
    }
}

void TestExecute::testAND()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(0x11,DATA_SIZE_BYTE);
        executeAND(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(int(registerFile.getGPR16Bits(RAX)),0x0);
    }
    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RDI,DATA_SIZE_DWORD,false);
        ExecImmediateOperand src(0x81,DATA_SIZE_BYTE);
        executeAND(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(u32(registerFile.getGPR32Bits(RDI)),u32(0x77777701));
    }
}

void TestExecute::testXOR()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(0x11,DATA_SIZE_BYTE);
        executeXOR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(int(registerFile.getGPR16Bits(RAX)),0x11);
    }
    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RDI,DATA_SIZE_DWORD,false);
        ExecImmediateOperand src(0x81,DATA_SIZE_BYTE);
        executeXOR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);
        QCOMPARE(u32(registerFile.getGPR32Bits(RDI)),u32(0x888888f6));
    }
}

void TestExecute::testADC()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0xffee);
        ExecImmediateOperand src(0x11,DATA_SIZE_WORD);

        executeADC(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x7fee);
        ExecImmediateOperand src(0x7f11,DATA_SIZE_WORD);

        executeADC(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xff00);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)1);
    }
}

void TestExecute::testSBB()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0xffee);
        ExecImmediateOperand src(0x11,DATA_SIZE_WORD);

        executeSBB(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xffdc);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x7f11);
        ExecImmediateOperand src(0x7fee,DATA_SIZE_WORD);

        executeSBB(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xff22);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x4000);
        ExecImmediateOperand src(0xc000,DATA_SIZE_WORD);

        executeSBB(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0x7fff);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testSUB()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
//        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0xffee);
        ExecImmediateOperand src(0x11,DATA_SIZE_WORD);

        executeSUB(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xffdd);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
//        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x7f11);
        ExecImmediateOperand src(0x7fee,DATA_SIZE_WORD);

        executeSUB(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xff23);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
//        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0x4000);
        ExecImmediateOperand src(0xc000,DATA_SIZE_WORD);

        executeSUB(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0x8000);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)1);
    }
}

void TestExecute::testCMP()
{

}

void TestExecute::testINC()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
//        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0xffee);

        executeINC(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xffef);
//        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testDEC()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
//        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0xffee);

        executeDEC(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xffed);
//        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testNOT()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0xffee);

        executeNOT(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0x0011);
    }
}

void TestExecute::testNEG()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        dest.setU16(0xffee);

        executeNEG(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0x0012);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().PF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().AF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().ZF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().SF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testROL()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(1,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeROL(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xffdd);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(4,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeROL(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xfeef);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
//        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testROR()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(1,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeROR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0x7ff7);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)1);
    }
    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(4,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeROR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xeffe);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
//        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testRCL()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(1,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeRCL(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xffdd);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=0;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(4,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeRCL(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xfee7);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
//        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testRCR()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(1,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeRCR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xfff7);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(4,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeRCR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xdffe);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
//        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testSHR()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(1,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeSHR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0x7ff7);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)1);
    }
    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(4,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeSHR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0x0ffe);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
//        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testSHL_SAL()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(1,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeSHL_SAL(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xffdc);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(4,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeSHL_SAL(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xfee0);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
//        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testSAR()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(1,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeSAR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xfff7);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)0);
        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
    {
        fillRegisterFile(registerFile);
        //normal case.
        ExecGPRegisterOperand dest(registerFile,RAX,DATA_SIZE_WORD,false);
        ExecImmediateOperand src(4,DATA_SIZE_BYTE);
        dest.setU16(0xffee);

        executeSAR(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,&src,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(dest.getU16()),(u32)0xfffe);
        QCOMPARE((u32)registerFile.getFlagsBits().CF,(u32)1);
//        QCOMPARE((u32)registerFile.getFlagsBits().OF,(u32)0);
    }
}

void TestExecute::testJx()
{
    RegisterFile registerFile;
    DebugMemory memory;
    IOPortList ioPortList(memory,registerFile);

    {
        fillRegisterFile(registerFile);
        registerFile.getFlagsBits().CF=1;
        //normal case.
        ExecImmediateOperand dest(1,DATA_SIZE_BYTE);

        executeJBE(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(registerFile.getIP()),(u32)0x1235);
    }
    {
        fillRegisterFile(registerFile);
        registerFile.setIP(0x1122ffff);
        registerFile.getFlagsBits().ZF=1;
        //normal case.
        ExecImmediateOperand dest(1,DATA_SIZE_BYTE);

        executeJBE(ENV_16_BITS,EFFECTIVE_16_BITS,&dest,NULL,NULL,registerFile,memory,ioPortList);

        QCOMPARE(u32(registerFile.getIP()),(u32)0x11220000);
    }
}

void TestExecute::fillRegisterFile(RegisterFile &registerFile)
{
    registerFile.setFlags32Bits(0);
    //RAX=0,RCX,RDX,RBX,RSP,RBP,RSI,RDI,R8,R9,R10,R11,R12,R13,R14,R15
    registerFile.setGPR64Bits(RAX,0x0000000000000000L);
    registerFile.setGPR64Bits(RCX,0x1111111111111111L);
    registerFile.setGPR64Bits(RDX,0x2222222222222222L);
    registerFile.setGPR64Bits(RBX,0x3333333333333333L);
    registerFile.setGPR64Bits(RSP,0x4444444444444444L);
    registerFile.setGPR64Bits(RBP,0x5555555555555555L);
    registerFile.setGPR64Bits(RSI,0x6666666666666666L);
    registerFile.setGPR64Bits(RDI,0x7777777777777777L);
    registerFile.setGPR64Bits(R8,0x8888888888888888L);
    registerFile.setGPR64Bits(R9,0x9999999999999999L);
    registerFile.setGPR64Bits(R10,0xaaaaaaaaaaaaaaaaL);
    registerFile.setGPR64Bits(R11,0xbbbbbbbbbbbbbbbbL);
    registerFile.setGPR64Bits(R12,0xccccccccccccccccL);
    registerFile.setGPR64Bits(R13,0xddddddddddddddddL);
    registerFile.setGPR64Bits(R14,0xeeeeeeeeeeeeeeeeL);
    registerFile.setGPR64Bits(R15,0xffffffffffffffffL);

    registerFile.setIP(0x1234);

    //ES=0,CS,SS,DS,FS,GS
    registerFile.setSR(ES,0x0000);registerFile.setSSR(ES,0x0000<<4);
    registerFile.setSR(CS,0x1111);registerFile.setSSR(CS,0x1111<<4);
    registerFile.setSR(SS,0x2222);registerFile.setSSR(SS,0x2222<<4);
    registerFile.setSR(DS,0x3333);registerFile.setSSR(DS,0x3333<<4);
    registerFile.setSR(FS,0x4444);registerFile.setSSR(FS,0x4444<<4);
    registerFile.setSR(GS,0x5555);registerFile.setSSR(GS,0x5555<<4);
}