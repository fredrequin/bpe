/*
Bit plane encoder
Please note:
(1)   Before you download and use the program, you must read and agree the license agreement carefully.
(2)   We supply the source code and program WITHOUT ANY WARRANTIES. The users will be responsible
        for any loses or damages caused by the use of the source code and the program.

Author:
Hongqiang Wang
Department of Electrical Engineering
University of Nebraska-Lincoln
Email: hqwang@bigred.unl.edu, hqwang@eecomm.unl.edu

Your comment and suggestions are welcome. Please report bugs to me via email and I would greatly appreciate it.
Nov. 3, 2006
*/

#include <stdlib.h>
#include <stdio.h>
#include "global.h"

typedef struct
{
    uint32_t val;
    int      len;
} t_code_tab;

static const t_code_tab c_code_tab_2b[2][4] =
{
    {
        { 0, 2 }, // 00 : 0
        { 1, 2 }, // 01 : 1
        { 2, 2 }, // 10 : 2
        { 3, 2 }  // 11 : 3
    },
    {
        { 1, 1 }, // 1 : 0
        { 1, 2 }, // 01 : 1
        { 1, 3 }, // 001 : 2
        { 0, 3 }  // 000 : 3
    }
};

static const t_code_tab c_code_tab_3b[2][8] =
{
    {
        { 1, 1 }, // 1 : 0
        { 1, 2 }, // 01 : 1
        { 1, 3 }, // 001 : 2
        { 0, 5 }, // 00000 : 3
        { 1, 5 }, // 00001 : 4
        { 2, 5 }, // 00010 : 5
        { 6, 6 }, // 000110 : 6
        { 7, 6 }  // 000111 : 7
    },
    {
        { 2, 2 }, // 10 : 0
        { 3, 2 }, // 11 : 1
        { 2, 3 }, // 010 : 2
        { 3, 3 }, // 011 : 3
        { 2, 4 }, // 0010 : 4
        { 3, 4 }, // 0011 : 5
        { 0, 4 }, // 0000 : 6
        { 1, 4 }  // 0001 : 7
    }
};

static const t_code_tab c_code_tab_4b[4][16] =
{
    {
        {  1, 1 }, // 1 : 0
        {  1, 2 }, // 01 : 1
        {  1, 3 }, // 001 : 2
        {  1, 4 }, // 0001 : 3
        {  0, 7 }, // 0000000 : 4
        {  1, 7 }, // 0000001 : 5
        {  2, 7 }, // 0000010 : 6
        {  3, 7 }, // 0000011 : 7
        {  8, 8 }, // 00001000 : 8
        {  9, 8 }, // 00001001 : 9
        { 10, 8 }, // 00001010 : 10
        { 11, 8 }, // 00001011 : 11
        { 12, 8 }, // 00001100 : 12
        { 13, 8 }, // 00001101 : 13
        { 14, 8 }, // 00001110 : 14
        { 15, 8 }  // 00001111 : 15
    },
    {
        {  2, 2 }, // 10 : 0
        {  3, 2 }, // 11 : 1
        {  2, 3 }, // 010 : 2
        {  3, 3 }, // 011 : 3
        {  2, 4 }, // 0010 : 4
        {  3, 4 }, // 0011 : 5
        {  0, 6 }, // 000000 : 6
        {  1, 6 }, // 000001 : 7
        {  2, 6 }, // 000010 : 8
        {  3, 6 }, // 000011 : 9
        {  4, 6 }, // 000100 : 10
        {  5, 6 }, // 000101 : 11
        { 12, 7 }, // 0001100 : 12
        { 13, 7 }, // 0001101 : 13
        { 14, 7 }, // 0001110 : 14
        { 15, 7 }  // 0001111 : 15
    },
    {
        {  4, 3 }, // 100 : 0
        {  5, 3 }, // 101 : 1
        {  6, 3 }, // 110 : 2
        {  7, 3 }, // 111 : 3
        {  4, 4 }, // 0100 : 4
        {  5, 4 }, // 0101 : 5
        {  6, 4 }, // 0110 : 6
        {  7, 4 }, // 0111 : 7
        {  4, 5 }, // 00100 : 8
        {  5, 5 }, // 00101 : 9
        {  6, 5 }, // 00110 : 10
        {  7, 5 }, // 00111 : 11
        {  0, 5 }, // 00000 : 12
        {  1, 5 }, // 00001 : 13
        {  2, 5 }, // 00010 : 14
        {  3, 5 }  // 00011 : 15
    },
    {
        {  0, 4 }, // 0000 : 0
        {  1, 4 }, // 0001 : 1
        {  2, 4 }, // 0010 : 2
        {  3, 4 }, // 0011 : 3
        {  4, 4 }, // 0100 : 4
        {  5, 4 }, // 0101 : 5
        {  6, 4 }, // 0110 : 6
        {  7, 4 }, // 0111 : 7
        {  8, 4 }, // 1000 : 8
        {  9, 4 }, // 1001 : 9
        { 10, 4 }, // 1010 : 10
        { 11, 4 }, // 1011 : 11
        { 12, 4 }, // 1100 : 12
        { 13, 4 }, // 1101 : 13
        { 14, 4 }, // 1110 : 14
        { 15, 4 }  // 1111 : 15
    }
};

void RiceCoding(uint8_t InputVal, uint8_t BitLength, uint8_t *Option, StructCodingPara *PtrCoding)
{
    const t_code_tab *p;
    
    switch(BitLength)
    {
        case 0:
        {
            return; // no need to process
        }

        case 1:
        {
            // 0 : 0
            // 1 : 1

            BitsOutput(PtrCoding, InputVal, 1);
            break;
        }

        case 2:
        {
            if ((Option[0] <= 1) && (InputVal <= 3))
            {
                p = &c_code_tab_2b[Option[0]][InputVal];
                BitsOutput(PtrCoding, p->val, p->len);
            }
            else
            {
                ErrorMsg(BPE_RICE_CODING_ERROR);
            }
            break;
        }

        case 3:
        {
            if ((Option[1] <= 1) && (InputVal <= 7))
            {
                p = &c_code_tab_3b[Option[1]][InputVal];
                BitsOutput(PtrCoding, p->val, p->len);
            }
            else if (Option[1] == 3) // Uncoded
            {
                // 000 : 0
                // 001 : 1
                // 010 : 2
                // 011 : 3
                // 100 : 4
                // 101 : 5
                // 110 : 6
                // 111 : 7

                BitsOutput(PtrCoding, InputVal, 3);
            }
            else
            {
                ErrorMsg(BPE_RICE_CODING_ERROR);
            }
            break;
        }

        case 4:
        {
            if ((Option[2] <= 3) && (InputVal <= 15))
            {
                p = &c_code_tab_4b[Option[2]][InputVal];
                BitsOutput(PtrCoding, p->val, p->len);
            }
            else
            {
                ErrorMsg(BPE_RICE_CODING_ERROR);
            }
            break;
        }

        default:
        {
            ErrorMsg(BPE_RICE_CODING_ERROR);
        }
    }
}


void RiceDecoding(uint32_t *decoded, short BitLength, uint8_t *splitOption, StructCodingPara *Ptr)
{
    uint32_t WordReadin;
    uint32_t i;

    switch(BitLength)
    {
        case 0:
        {
            return; // no need to process
        }

        case 1:
        {
            // 0 : 0
            // 1 : 1

            BitsRead(Ptr, decoded, 1);
            break;
        }

        case 2:
        {
            // Ouput FS code.
            switch (splitOption[0])
            {
                case 1:
                {
                    // 00 : 0
                    // 01 : 1
                    // 10 : 2
                    // 11 : 3

                    BitsRead(Ptr, &WordReadin, 2);
                    if (Ptr->RateReached == TRUE)
                    {
                        *decoded = 0;
                        return;
                    }
            
                    *decoded = WordReadin;
                    return;
                }

                case 0:
                {
                    // 1 : 0
                    // 01 : 1
                    // 001 : 2
                    // 000 : 3

                    for (i = 0; i < 3; i++)
                    {
                        BitsRead(Ptr, &WordReadin, 1);
            
                        if (Ptr->RateReached == TRUE)
                        {
                            *decoded = 0;
                            return;
                        }
                        if (WordReadin == 1) break;
                    }
                    *decoded = i;
                    break;
                }

                default:
                {
                    ErrorMsg(BPE_RICE_CODING_ERROR);
                }
            }
            break;
        }

        case 3:
        {
            switch (splitOption[1])
            {
                case 3: // Uncoded
                    // 000 : 0
                    // 001 : 1
                    // 010 : 2
                    // 011 : 3
                    // 100 : 4
                    // 101 : 5
                    // 110 : 6
                    // 111 : 7

                    BitsRead(Ptr, &WordReadin, 3);
                    if (Ptr->RateReached == TRUE)
                    {
                        *decoded = 0;
                        return;
                    }
                    *decoded = WordReadin;
                    return;
            
                case 1: // nk = 1, max bits value is 11b. i.e., decoded can take values form 0 to 3.
                {
                    // 10 : 0
                    // 11 : 1
                    // 010 : 2
                    // 011 : 3
                    // 0000 : 6
                    // 0001 : 7
                    // 0010 : 4
                    // 0011 : 5

                    BitsRead(Ptr, &WordReadin, 2);
                    if (Ptr->RateReached == TRUE)
                    {
                        *decoded = 0;
                        return;
                    }
                    
                    if (WordReadin & 0x2)
                    {
                        *decoded = (WordReadin & 0x1) ? 1 : 0;
                    }
                    else if (WordReadin & 0x1)
                    {
                        BitsRead(Ptr, &WordReadin, 1);
                        if (Ptr->RateReached == TRUE)
                        {
                            *decoded = 0;
                            return;
                        }
            
                        *decoded = (WordReadin) ? 3 : 2;
                    }
                    else
                    {
                        BitsRead(Ptr, &WordReadin, 2);
                        if (Ptr->RateReached == TRUE)
                        {
                            *decoded = 0;
                            return;
                        }
            
                        switch (WordReadin)
                        {
                            case 0x0:
                            {
                                *decoded = 6;
                                break;
                            }
                            case 0x1:
                            {
                                *decoded = 7;
                                break;
                            }
                            case 0x2:
                            {
                                *decoded = 4;
                                break;
                            }
                            case 0x3:
                            {
                                *decoded = 5;
                                break;
                            }
                        }
                    }
                    return;
                }
            
                case 0:
                {
                    // 1 : 0
                    // 01 : 1
                    // 001 : 2
                    // 00000 : 3
                    // 00001 : 4
                    // 00010 : 5
                    // 000110 : 6
                    // 000111 : 7

                    WordReadin = 0;
                    for ( i = 0; i < 3; i++)
                    {
                        BitsRead(Ptr, &WordReadin, 1);
                        if (Ptr->RateReached == TRUE)
                        {
                            *decoded = 0;
                            return;
                        }
                        if (WordReadin == 1) break;
                    }
                    if (WordReadin == 1)  // the third letter is zero.
                    {
                        *decoded = i;
                    }
                    else
                    {
                        BitsRead(Ptr, &WordReadin, 2);
            
                        if (Ptr->RateReached == TRUE)
                        {
                            *decoded = 0;
                            return;
                        }
            
                        if (WordReadin != 3)
                        {
                            *decoded = WordReadin + 3;
                        }
                        else
                        {
                            BitsRead(Ptr, &WordReadin, 1);
                            *decoded = WordReadin + 6;
                        }
            
                    }
                    return;
                }

                default:
                {
                    ErrorMsg(BPE_RICE_CODING_ERROR);
                }
            }
            break;
        }

        case 4:
        {
            switch (splitOption[2])
            {
                case 3: // Uncoded
                {
                    // 0000 : 0
                    // 0001 : 1
                    // 0010 : 2
                    // 0011 : 3
                    // 0100 : 4
                    // 0101 : 5
                    // 0110 : 6
                    // 0111 : 7
                    // 1000 : 8
                    // 1001 : 9
                    // 1010 : 10
                    // 1011 : 11
                    // 1100 : 12
                    // 1101 : 13
                    // 1110 : 14
                    // 1111 : 15

                    BitsRead(Ptr, &WordReadin, 4);
                    if (Ptr->RateReached == TRUE)
                    {
                        *decoded = 0;
                        return;
                    }
                    *decoded = WordReadin;
                    return;
                }
                
                case 2:
                {
                    // 100 : 0
                    // 101 : 1
                    // 110 : 2
                    // 111 : 3
                    // 0100 : 4
                    // 0101 : 5
                    // 0110 : 6
                    // 0111 : 7
                    // 00100 : 8
                    // 00101 : 9
                    // 00110 : 10
                    // 00111 : 11
                    // 00000 : 12
                    // 00001 : 13
                    // 00010 : 14
                    // 00011 : 15

                    BitsRead(Ptr, &WordReadin, 3);
                    if (Ptr->RateReached == TRUE)
                    {
                        *decoded = 0;
                        return;
                    }

                    if (WordReadin & 0x4)
                    {
                        *decoded = WordReadin & 0x3;
                    }
                    else if (WordReadin & 0x2)
                    {
                        if (WordReadin & 0x1)
                        {
                            BitsRead(Ptr, &WordReadin, 1);
                            if (Ptr->RateReached == TRUE)
                            {
                                *decoded = 0;
                                return;
                            }
                            *decoded = WordReadin + 6;
                        }
                        else
                        {
                            BitsRead(Ptr, &WordReadin, 1);
                            if (Ptr->RateReached == TRUE)
                            {
                                *decoded = 0;
                                return;
                            }
                            *decoded = WordReadin + 4;
                        }
                    }
                    else
                    {
                        if (WordReadin & 0x1)
                        {
                            BitsRead(Ptr, &WordReadin, 2);
                            if (Ptr->RateReached == TRUE)
                            {
                                *decoded = 0;
                                return;
                            }
                            *decoded = WordReadin + 8;
                        }
                        else
                        {
                            BitsRead(Ptr, &WordReadin, 2);
                            if (Ptr->RateReached == TRUE)
                            {
                                *decoded = 0;
                                return;
                            }
                            *decoded = WordReadin + 12;
                        }
                    }
                    return;
                }
        
                case 1:
                {
                    // 10 : 0
                    // 11 : 1
                    // 010 : 2
                    // 011 : 3
                    // 0010 : 4
                    // 0011 : 5
                    // 000000 : 6
                    // 000001 : 7
                    // 000010 : 8
                    // 000011 : 9
                    // 000100 : 10
                    // 000101 : 11
                    // 0001100 : 12
                    // 0001101 : 13
                    // 0001110 : 14
                    // 0001111 : 15

                    BitsRead(Ptr, &WordReadin, 2);
                    if (Ptr->RateReached == TRUE)
                    {
                        *decoded = 0;
                        return;
                    }

                    if (WordReadin & 0x2)
                    {
                        *decoded = WordReadin - 2;
                    }
                    else if (WordReadin & 0x1)
                    {
                        BitsRead(Ptr, &WordReadin, 1);
                        if (Ptr->RateReached == TRUE)
                        {
                            *decoded = 0;
                            return;
                        }
                        
                        *decoded = WordReadin + 2;
                    }
                    else
                    {
                        BitsRead(Ptr, &WordReadin, 2);
                        if (Ptr->RateReached == TRUE)
                        {
                            *decoded = 0;
                            return;
                        }

                        if (WordReadin & 0x2)
                        {
                            *decoded = WordReadin + 2;
                        }
                        else if (WordReadin & 0x1)
                        {
                            BitsRead(Ptr, &WordReadin, 2);
                            if (Ptr->RateReached == TRUE)
                            {
                                *decoded = 0;
                                return;
                            }
            
                            if (WordReadin & 0x2)
                            {
                                if (WordReadin & 0x1)
                                {
                                    BitsRead(Ptr, &WordReadin, 1);
                                    if (Ptr->RateReached == TRUE)
                                    {
                                        *decoded = 0;
                                        return;
                                    }

                                    *decoded = WordReadin + 14;
                                }
                                else
                                {
                                    BitsRead(Ptr, &WordReadin, 1);
                                    if (Ptr->RateReached == TRUE)
                                    {
                                        *decoded = 0;
                                        return;
                                    }

                                    *decoded = WordReadin + 12;
                                }
                            }
                            else
                            {
                                *decoded = WordReadin + 10;
                            }
                        }
                        else
                        {
                            BitsRead(Ptr, &WordReadin, 2);
                            if (Ptr->RateReached == TRUE)
                            {
                                *decoded = 0;
                                return;
                            }
                            *decoded = WordReadin + 6;
                        }
                    }
                    return;
                }
                case 0:
                {
                    // 1 : 0
                    // 01 : 1
                    // 001 : 2
                    // 0001 : 3
                    // 0000000 : 4
                    // 0000001 : 5
                    // 0000010 : 6
                    // 0000011 : 7
                    // 00001000 : 8
                    // 00001001 : 9
                    // 00001010 : 10
                    // 00001011 : 11
                    // 00001100 : 12
                    // 00001101 : 13
                    // 00001110 : 14
                    // 00001111 : 15

                    for ( i = 0; i < 4; i++)
                    {
                        BitsRead(Ptr, &WordReadin, 1);
                        if (Ptr->RateReached == TRUE)
                        {
                            *decoded = 0;
                            return;
                        }
                        if (WordReadin == 1) break;
                    }
    
                    if (i != 4)
                    {
                        *decoded = i;
                    }
                    else
                    {
                        BitsRead(Ptr, &WordReadin, 3);
                        if (Ptr->RateReached == TRUE)
                        {
                            *decoded = 0;
                            return;
                        }
    
                        if (WordReadin & 0x4)
                        {
                            *decoded = WordReadin << 1;
    
                            BitsRead(Ptr, &WordReadin, 1);
                            if (Ptr->RateReached == TRUE)
                            {
                                *decoded = 0;
                                return;
                            }
    
                            *decoded += WordReadin;
                        }
                        else
                        {
                            *decoded = WordReadin + 4;
                        }
                    }
                    return;
                }

                default:
                {
                    ErrorMsg(BPE_RICE_CODING_ERROR);
                }
            }
            break;
        }

        default:
        {
            ErrorMsg(BPE_RICE_CODING_ERROR);
        }
    }
    return;
}
