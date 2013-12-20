#ifndef EXECSEMENTREGISTEROPERAND_H
#define EXECSEMENTREGISTEROPERAND_H
#include "execreadwriteoperand.h"
#include "cpu/register/registerfile.h"
class ExecSegmentRegisterOperand:public ExecReadWriteOperand
{
public:
    ExecSegmentRegisterOperand(RegisterFile& file,u8 segmentRegister)
        :_file(file),_segmentRegister(segmentRegister)
    {
        _size=DATA_SIZE_WORD;
    }
    virtual void prepareReadSigned()
    {
//        assert(0);
        _sValue = _file.getSR(_segmentRegister);
    }
    virtual void prepareReadUnsigned()
    {
        _uValue = _file.getSR(_segmentRegister);
    }
    virtual void setU8(u8 value)
    {
		(void)value;
        assert(0);
    }
    virtual void setU16(u16 value)
    {
        _file.setSR(_segmentRegister,value);
        _file.setSSR(_segmentRegister,value<<4);//if mode=16bits.
    }
    virtual void setU32(u32 value)
    {
		(void)value;
        assert(0);
    }
    virtual void setU64(u64 value)
    {
		(void)value;
        assert(0);
    }
protected:
    RegisterFile& _file;
    u8 _segmentRegister;
};

#endif // EXECSEMENTREGISTEROPERAND_H
