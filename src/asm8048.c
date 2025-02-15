// asm8048.c

#define versionName "8048 assembler"
#include "asmx.h"

enum
{
    CPU_8048,
    CPU_8041,
    CPU_8021,
    CPU_8022
};

enum instrType
{
    o_None,         // No operands

    o_Arith,        // ADD, ADDC
    o_Logical,      // ANL, ORL, XRL, XCH
    o_XCHD,         // XCHD
    o_JMP,          // JMP, CALL
    o_RET,          // RET, RETR
    o_Branch,       // Jcc
    o_DJNZ,         // DJNZ
    o_INC_DEC,      // INC, DEC
    o_MOV,          // MOV
    o_MOVD,         // MOVD
    o_LogicalD,     // ANLD, ORLD
    o_CLR_CPL,      // CLR, CPL
    o_Accum,        // DA A, RL A, RLC A, RRC A, RR A, SWAP A
    o_EN_DIS,       // EN, DIS
    o_MOVX,         // MOVX
    o_ENT0,         // ENT0 CLK
    o_MOVP,         // MOVP MOVP3
    o_JMPP,         // JMPP @A
    o_STOP,         // STOP TCNT
    o_STRT,         // STRT CNT/T
    o_SEL,          // SEL
    o_IN,           // IN
    o_INS,          // INS
    o_OUTL,         // OUTL

//  o_Foo = o_LabelOp,
};

// Note: there seem to be three primary variations of the instruction set:
// 8048/8049/8050/8035/8039/8040 is the primary variant
// 8021/8022 doesn't have SEL MBx, SEL RBx, MOVX, and a few others
// 8041/8042 doesn't have SEL MBx or MOVX, and some other slight differences
// Oki and Toshiba also have some slight variations.
// These differences are currently handled in the simplest possible way:
// allow all that don't conflict with the main instruction set.

static const struct OpcdRec I8048_opcdTab[] =
{
    {"NOP",  o_None,  0x00},
    {"HALT", o_None,  0x01}, // on Oki MSM80Cxx and Toshiba 8048
    {"RET",  o_RET,   0x83}, // note: 8022 may want "RET I" instead of RETR
    {"RETR", o_RET,   0x93},
    {"HLTS", o_None,  0x82}, // on Oki MSM80Cxx
    {"FLT",  o_None,  0xA2}, // on Oki MSM80Cxx
    {"FLTT", o_None,  0xC2}, // on Oki MSM80Cxx

    {"ADD",  o_Arith, 0x00},
    {"ADDC", o_Arith, 0x10},

    {"ANL",  o_Logical, 0x50}, // no ports on 8022?
    {"ORL",  o_Logical, 0x40}, // no ports on 8022?
    {"XRL",  o_Logical, 0xD0}, // no ports
    {"XCH",  o_Logical, 0x20}, // no ports or immediate

    {"XCHD", o_XCHD,    0x30},

    {"JMP",  o_JMP,     0x04},
    {"CALL", o_JMP,     0x14},

    {"JB0",  o_Branch,  0x12}, // 8048 only
    {"JB1",  o_Branch,  0x32}, // 8048 only
    {"JB2",  o_Branch,  0x52}, // 8048 only
    {"JB3",  o_Branch,  0x72}, // 8048 only
    {"JB4",  o_Branch,  0x92}, // 8048 only
    {"JB5",  o_Branch,  0xB2}, // 8048 only
    {"JB6",  o_Branch,  0xD2}, // 8048 only
    {"JB7",  o_Branch,  0xF2}, // 8048 only

    {"JTF",  o_Branch,  0x16},
    {"JNT0", o_Branch,  0x26}, // not 8021?
    {"JT0",  o_Branch,  0x36}, // not 8021?
    {"JNT1", o_Branch,  0x46},
    {"JT1",  o_Branch,  0x56},
    {"JF1",  o_Branch,  0x76}, // not 8021/8022?
    {"JNI",  o_Branch,  0x86}, // not 8021/8022?
    {"JF0",  o_Branch,  0xb6}, // not 8021/8022?

    {"JZ",   o_Branch,  0xC6},
    {"JNZ",  o_Branch,  0x96},
    {"JC",   o_Branch,  0xF6},
    {"JNC",  o_Branch,  0xE6},

    {"DJNZ", o_DJNZ,    0xE8},

    {"INC",  o_INC_DEC, 0x10},
    {"DEC",  o_INC_DEC, 0xC0},

    {"MOV",  o_MOV,     0x00},

    {"MOVD", o_MOVD,    0x00},

    {"ANLD", o_LogicalD,0x9C},
    {"ORLD", o_LogicalD,0x8C},

    {"CLR",  o_CLR_CPL, 0x00},
    {"CPL",  o_CLR_CPL, 0x10},

    {"SWAP", o_Accum,   0x47},
    {"DA",   o_Accum,   0x57},
    {"RRC",  o_Accum,   0x67},
    {"RR",   o_Accum,   0x77},
    {"RL",   o_Accum,   0xE7},
    {"RLC",  o_Accum,   0xF7},

    {"EN",   o_EN_DIS,  0x00},
    {"DIS",  o_EN_DIS,  0x10},

    {"MOVX", o_MOVX,    0x00}, // 8048 only

    {"ENT0", o_ENT0,    0x00}, // 8048 only

    {"MOVP", o_MOVP,    0xA3},
    {"MOVP3",o_MOVP,    0xE3},

    {"JMPP", o_JMPP,    0xB3},

    {"STOP", o_STOP,    0x00},

    {"STRT", o_STRT,    0x00},

    {"SEL",  o_SEL,     0x00},

    {"IN",   o_IN,      0x00},
    {"INS",  o_INS,     0x00},
    {"OUTL", o_OUTL,    0x00}, // OUTL DBB,A is 0x90 on 8041/8021/8022


    {"",    o_Illegal,  0}
};


char regs_8048[] = "R0 R1 R2 R3 R4 R5 R6 R7 @R0 @R1 # A PSW T";

enum
{
    reg_R0    =  0,
    reg_R1    =  1,
    reg_R2    =  2,
    reg_R3    =  3,
    reg_R4    =  4,
    reg_R5    =  5,
    reg_R6    =  6,
    reg_R7    =  7,
    reg_xR0   =  8,
    reg_xR1   =  9,
    reg_Imm   = 10,
    reg_A     = 11,
    reg_PSW   = 12,
    reg_T     = 13,
};


// This tracks the last SEL MB page. It is reset to -1 after an
// unconditional JMP or RET instruction. If it is unknown, the current
// page is used instead.
// This can be subverted through bad programming style, but should be
// sufficient if SEL MB is always set before long jumps/calls. (And
// presumably set back after long calls as well.)

int selmb;

// --------------------------------------------------------------


static int Get_8048_Reg(const char *regList)
{
    Str255  word;
    int     token;

    if (!(token = GetWord(word)))
    {
        MissingOperand();
        return reg_EOL;
    }

    // 8048 needs to handle '@' symbols as part of a register name
    if (token == '@')
    {
        GetWord(word+1);
    }

    return FindReg(word, regList);
}


static int I8048_DoCPUOpcode(int typ, int parm)
{
    int     val, reg1, reg2;
//  Str255  word;
//  char    *oldLine;
//  int     token;

    switch (typ)
    {
        case o_RET:
            // reset selmb after unconditional returns
            selmb = -1;
        // fall through...
        case o_None:
            InstrB(parm);
            break;

        case o_Arith:
            if (Expect("A")) break;
            if (Comma()) break;
            reg1 = Get_8048_Reg(regs_8048);
            switch (reg1)
            {
                case reg_R0: // A,Rn = parm + 0x68 + reg
                case reg_R1:
                case reg_R2:
                case reg_R3:
                case reg_R4:
                case reg_R5:
                case reg_R6:
                case reg_R7:
                    InstrB(parm + 0x68 + reg1 - reg_R0);
                    break;

                case reg_xR0: // A,@Rn = parm + 0x60 + reg
                case reg_xR1:
                    InstrB(parm + 0x60 + reg1 - reg_xR0);
                    break;

                case reg_Imm: // A,#imm = parm + 0x03
                    val = EvalByte();
                    InstrBB(parm + 0x03, val);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_Logical:
            reg1 = Get_8048_Reg("A P1 P2 BUS");
            switch (reg1)
            {
                case 0:
                    if (Comma()) break;
                    reg1 = Get_8048_Reg(regs_8048);
                    switch (reg1)
                    {
                        case reg_R0: // A,Rn = parm + 0x08 + reg
                        case reg_R1:
                        case reg_R2:
                        case reg_R3:
                        case reg_R4:
                        case reg_R5:
                        case reg_R6:
                        case reg_R7:
                            InstrB(parm + 0x08 + reg1 - reg_R0);
                            break;

                        case reg_xR0: // A,@Rn = parm + reg
                        case reg_xR1:
                            InstrB(parm + reg1 - reg_xR0);
                            break;

                        case reg_Imm: // A,#imm = parm + 0x03 (not 0x20)
                            if (parm == 0x20)
                            {
                                IllegalOperand();
                                break;
                            }
                            val = EvalByte();
                            InstrBB(parm + 0x03, val);
                            break;

                        case reg_EOL:
                            break;

                        default:
                            IllegalOperand();
                            break;
                    }
                    break;

                case 1: // Pn,#imm = parm + 0x48 + port (not 0xD0 or 0x20)
                case 2:
                    if (Comma()) break;
                    if (Expect("#")) break;
                    val = EvalByte();
                    if (parm == 0xD0 || parm == 0x20)
                    {
                        IllegalOperand();
                        break;
                    }
                    InstrBB(parm + 0x48 + reg1, val);
                    break;

                case 3: // BUS,#imm = parm + 0x48 (not 0xD0 or 0x20)
                    if (Comma()) break;
                    if (Expect("#")) break;
                    val = EvalByte();
                    if (parm == 0xD0 || parm == 0x20)
                    {
                        IllegalOperand();
                        break;
                    }
                    InstrBB(parm + 0x48, val);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_XCHD:
            if (Expect("A")) break;
            if (Comma()) break;
            reg1 = Get_8048_Reg("@R0 @R1");
            switch (reg1)
            {
                case 0: // A,@Rn = parm + reg
                case 1:
                    InstrB(parm + reg1);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_JMP:
            val = Eval();

            // check for jumps to the other SEL MB area
            if (selmb == -1)
            {
                // if no SEL MBx seen recently, assume PC is correct
                if ((val & 0xF800) != ((locPtr + 2) & 0xF800))
                {
                    Warning("Jump across code bank boundary");
                }
            }
            else
            {
                // if SEL MBx has been seen, use it for bank number
                if (selmb != ((val & 0xF800) >> 11))
                {
                    Warning("Jump across code bank boundary");
                }
            }

            // reset selmb after unconditional jumps
            if (parm == 0x04)
            {
                selmb = -1;
            }

            InstrBB(parm + ((val & 0x0700) >> 3), val & 0xFF);
            break;

        case o_Branch:
            val = Eval();
            if ((val & 0xFF00) != ((locPtr + 2) & 0xFF00))
            {
                Warning("Branch out of range");
            }
            InstrBB(parm, val & 0xFF);
            break;

        case o_DJNZ:
            reg1 = Get_8048_Reg(regs_8048);
            switch (reg1)
            {
                case reg_R0: // Rn,addr = parm + reg
                case reg_R1:
                case reg_R2:
                case reg_R3:
                case reg_R4:
                case reg_R5:
                case reg_R6:
                case reg_R7:
                    if (Comma()) break;
                    val = Eval();
                    if ((val & 0xFF00) != ((locPtr + 2) & 0xFF00))
                    {
                        Warning("Branch out of range");
                    }
                    InstrBB(parm + reg1 - reg_R0, val & 0xFF);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;

            }
            break;

        case o_INC_DEC:
            reg1 = Get_8048_Reg(regs_8048);
            switch (reg1)
            {
                case reg_R0: // Rn = parm + 0x08 + reg (no 0xC0 on 8021/8022)
                case reg_R1:
                case reg_R2:
                case reg_R3:
                case reg_R4:
                case reg_R5:
                case reg_R6:
                case reg_R7:
                    InstrB(parm + 0x08 + reg1 - reg_R0);
                    break;

                case reg_xR0: // @Rn = parm + reg
                case reg_xR1:
                    InstrB(parm + reg1 - reg_xR0);
                    break;

                case reg_A: // A = (parm & 0x10) + 0x07
                    InstrB((parm & 0x10) + 0x07);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_MOV:
            reg1 = Get_8048_Reg(regs_8048);
            switch (reg1)
            {
                case reg_R0: // Rn,
                case reg_R1:
                case reg_R2:
                case reg_R3:
                case reg_R4:
                case reg_R5:
                case reg_R6:
                case reg_R7:
                    if (Comma()) break;
                    reg2 = Get_8048_Reg("A #");
                    switch (reg2)
                    {
                        case 0: // Rn,A = 0xA8 + reg1
                            InstrB(0xA8 + reg1 - reg_R0);
                            break;

                        case 1: // Rn,#imm = 0xB8 + reg1
                            val = EvalByte();
                            InstrBB(0xB8 + reg1 - reg_R0, val);
                            break;

                        case reg_EOL:
                            break;

                        default:
                            IllegalOperand();
                            break;
                    }
                    break;

                case reg_xR0: // @Rn,
                case reg_xR1:
                    if (Comma()) break;
                    reg2 = Get_8048_Reg("A #");
                    switch (reg2)
                    {
                        case 0: // @Rn,A = 0xA0 + reg1
                            InstrB(0xA0 + reg1 - reg_xR0);
                            break;

                        case 1: // @Rn,#imm = 0xB0 + reg1
                            val = EvalByte();
                            InstrBB(0xB0 + reg1 - reg_xR0, val);
                            break;

                        case reg_EOL:
                            break;

                        default:
                            IllegalOperand();
                            break;
                    }
                    break;

                case reg_A: // A,
                    if (Comma()) break;
                    reg2 = Get_8048_Reg(regs_8048);
                    switch (reg2)
                    {
                        case reg_R0: // A,Rn = 0xF8 + reg2
                        case reg_R1:
                        case reg_R2:
                        case reg_R3:
                        case reg_R4:
                        case reg_R5:
                        case reg_R6:
                        case reg_R7:
                            InstrB(0xF8 + reg2 - reg_R0);
                            break;

                        case reg_xR1: // @Rn = 0xF0 + reg2
                        case reg_xR0:
                            InstrB(0xF0 + reg2 - reg_xR0);
                            break;

                        case reg_Imm: // A,#imm
                            val = EvalByte();
                            InstrBB(0x23, val);
                            break;

                        case reg_PSW: // A,PSW
                            InstrB(0xC7);
                            break;

                        case reg_T: // A,T
                            InstrB(0x42);
                            break;

                        case reg_EOL:
                            break;

                        default:
                            IllegalOperand();
                            break;
                    }
                    break;

                case reg_PSW: // PSW,A
                    if (Comma()) break;
                    if (Expect("A")) break;
                    InstrB(0xD7);
                    break;

                case reg_T: // T,A
                    if (Comma()) break;
                    if (Expect("A")) break;
                    InstrB(0x62);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_MOVD:
            reg1 = Get_8048_Reg("P4 P5 P6 P7 A");
            switch (reg1)
            {
                case 0: // Pn,A = 0x3C + reg
                case 1:
                case 2:
                case 3:
                    if (Comma()) break;
                    if (Expect("A")) break;
                    InstrB(0x3C + reg1);
                    break;

                case 4: // A,Pn
                    if (Comma()) break;
                    reg1 = Get_8048_Reg("P4 P5 P6 P7");
                    switch (reg1)
                    {
                        case 0: // A,Pn = 0x0C + reg
                        case 1:
                        case 2:
                        case 3:
                            InstrB(0x0C + reg1);
                            break;

                        case reg_EOL:
                            break;

                        default:
                            IllegalOperand();
                            break;
                    }
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_LogicalD:
            reg1 = Get_8048_Reg("P4 P5 P6 P7");
            switch (reg1)
            {
                case 0: // Pn,A = parm + reg
                case 1:
                case 2:
                case 3:
                    if (Comma()) break;
                    if (Expect("A")) break;
                    InstrB(parm + reg1);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_CLR_CPL:
            reg1 = Get_8048_Reg("A F0 C F1");
            switch (reg1)
            {
                case 0: // A  = parm + 0x27
                    InstrB(parm + 0x27);
                    break;

                case 1: // F0 = parm + 0x85 (not on 8021/8022)
                    InstrB(parm + 0x85);
                    break;

                case 2: // C  = parm + 0x97
                    InstrB(parm + 0x97);
                    break;

                case 3: // F1 = parm + 0xA5 (not on 8021/8022)
                    InstrB(parm + 0xA5);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_Accum:
            if (Expect("A")) break;
            InstrB(parm);
            break;

        case o_EN_DIS:
            reg1 = Get_8048_Reg("I TCNTI");
            switch (reg1)
            {
                case 0: // I  = parm + 0x05
                    InstrB(parm + 0x05);
                    break;

                case 1: // TCNTI = parm + 0x25
                    InstrB(parm + 0x25);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_MOVX: // 8048 only
            reg1 = Get_8048_Reg("@R0 @R1 A");
            switch (reg1)
            {
                case 0: // @Rn,A = 0x90 + reg
                case 1:
                    if (Comma()) break;
                    if (Expect("A")) break;
                    InstrB(0x90 + reg1);
                    break;

                case 2: // A,@Rn
                    if (Comma()) break;
                    reg1 = Get_8048_Reg("@R0 @R1");
                    switch (reg1)
                    {
                        case 0: // @A,Rn = 0x80 + reg
                        case 1:
                            InstrB(0x80 + reg1);
                            break;

                        case reg_EOL:
                            break;

                        default:
                            IllegalOperand();
                            break;
                    }
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_ENT0: // 8048 only
            reg1 = Get_8048_Reg("CLK");
            switch (reg1)
            {
                case 0: // CLK
                    InstrB(0x75);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_MOVP:
            if (Expect("A")) break;
            if (Comma()) break;
        // fall through...
        case o_JMPP:
            // reset selmb after unconditional jumps
            if ((typ == o_JMPP) && (parm == 0xB3))
            {
                selmb = -1;
            }

            reg1 = Get_8048_Reg("@A");
            switch (reg1)
            {
                case 0: // @A
                    InstrB(parm);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_STOP:
            reg1 = Get_8048_Reg("TCNT");
            switch (reg1)
            {
                case 0: // TCNT
                    InstrB(0x65);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_STRT:
            reg1 = Get_8048_Reg("CNT T");
            switch (reg1)
            {
                case 0: // CNT
                    InstrB(0x45);
                    break;

                case 1: // T
                    InstrB(0x55);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_SEL:
            reg1 = Get_8048_Reg("RB0 RB1 MB0 MB1");
            switch (reg1)
            {
                case 2: // MB0
                case 3: // MB1
                    // remember last SEL MBx
                    selmb = reg1 - 2;
                    break;
            }
            switch (reg1)
            {
                case 0: // RB0 = 0xC5 not 8022?
                case 1: // RB1 = 0xD5 not 8022?
                case 2: // MB0 = 0xE5 8048 only
                case 3: // MB1 = 0XF5 8048 only
                    InstrB(0xC5 + reg1*16);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_IN:
        case o_INS:
            if (Expect("A")) break;
            if (Comma()) break;
            reg1 = Get_8048_Reg("BUS P1 P2");
            switch (reg1)
            {
                case 0: // A,BUS
                    if (typ == o_IN)
                    {
                        IllegalOperand();
                        break;
                    }
                    InstrB(0x08);
                    break;

                case 1: // A,P1
                case 2: // A,P2
                    if (typ != o_IN)
                    {
                        IllegalOperand();
                        break;
                    }
                    InstrB(0x08 + reg1);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        case o_OUTL:
            reg1 = Get_8048_Reg("BUS P1 P2");
            switch (reg1)
            {
                case 0: // BUS,A
                    if (Comma()) break;
                    if (Expect("A")) break;
                    InstrB(0x02); // 0x90 for 8041/8021/8022 "OUTL DBB,A"
                    break;

                case 1: // P1,A
                case 2: // P2,A
                    if (Comma()) break;
                    if (Expect("A")) break;
                    InstrB(0x38 + reg1);
                    break;

                case reg_EOL:
                    break;

                default:
                    IllegalOperand();
                    break;
            }
            break;

        default:
            return 0;
            break;
    }
    return 1;
}


void I8048_PassInit(void)
{
    // start each pass with last selmb page undefined
    selmb = -1;
}


void Asm8048Init(void)
{
    void *p = AddAsm(versionName, &I8048_DoCPUOpcode, NULL, &I8048_PassInit);

    AddCPU(p, "8048",  CPU_8048, LITTLE_END, ADDR_16, LIST_24, 8, 0, I8048_opcdTab);
//  AddCPU(p, "8041",  CPU_8041, LITTLE_END, ADDR_16, LIST_24, 8, 0, I8048_opcdTab);
//  AddCPU(p, "8021",  CPU_8021, LITTLE_END, ADDR_16, LIST_24, 8, 0, I8048_opcdTab);
//  AddCPU(p, "8022",  CPU_8022, LITTLE_END, ADDR_16, LIST_24, 8, 0, I8048_opcdTab);
}
