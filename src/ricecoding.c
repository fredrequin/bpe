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


void RiceCoding(short InputVal,
              short BitLength,
              uint8_t *Option,
              StructCodingPara *PtrCoding )
{
   switch(BitLength)
   {
   case 0:    // no need to process
      return;
   case 1:
      BitsOutput(PtrCoding, InputVal, 1);
      break;
   case 2:
      //ouput FS code.
      if (Option[0] == 1)
         BitsOutput(PtrCoding, InputVal, 2);

      else if (Option[0] == 0) // nk = 0
      {
         if (InputVal <= 2)
         {
            BitsOutput(PtrCoding, 0, InputVal);
            BitsOutput(PtrCoding, 1, 1);
         }
         else
            BitsOutput(PtrCoding, 0, 3);
      }
      else
         ErrorMsg(BPE_RICE_CODING_ERROR);
      break;
   case 3:
      if (Option[1] == 0) // nk =0, no split.
      {
         if (InputVal <= 2)
         {
            BitsOutput(PtrCoding, 0, InputVal);
            BitsOutput(PtrCoding, 1, 1);
         }
         else if (InputVal <= 5)
         {
            BitsOutput(PtrCoding, 0, 3);
            BitsOutput(PtrCoding, InputVal - 3, 2);
         }
         else if (InputVal <= 7)
         {
            BitsOutput(PtrCoding, 0, 3);
            BitsOutput(PtrCoding, InputVal, 3);
         }
         else
            ErrorMsg(BPE_RICE_CODING_ERROR);
      }
      else if (Option[1] == 1) // nk =1
      {
         if (InputVal <= 1)
         {
            BitsOutput(PtrCoding, InputVal + 2, 2);
         }
         else if (InputVal <= 3)
         {
            BitsOutput(PtrCoding, InputVal, 3);
         }
         else if (InputVal <= 7)
         {
            BitsOutput(PtrCoding, 0, 2);
            if (InputVal == 4)
               BitsOutput(PtrCoding, 2, 2);
            else
               if (InputVal == 5)
                  BitsOutput(PtrCoding, 3, 2);
               else
                  if (InputVal == 6)
                     BitsOutput(PtrCoding, 0, 2);
                  else
                     if (InputVal == 7)
                        BitsOutput(PtrCoding, 1, 2);
         }
         else
            ErrorMsg(BPE_RICE_CODING_ERROR);
      }
      else if (Option[1] == 3) // uncoded .
      {
         BitsOutput(PtrCoding, InputVal, 3);
      }
      break;
   case 4: //4-bit
      if (Option[2] == 0) // no split.
      {
         if (InputVal <= 3)
         {
            BitsOutput(PtrCoding, 0, InputVal);
            BitsOutput(PtrCoding, 1, 1);
         }
         else if (InputVal <= 7)
         {
            BitsOutput(PtrCoding, 0, 5);
            BitsOutput(PtrCoding, InputVal - 4, 2);
         }
         else if (InputVal <= 15)
         {
            BitsOutput(PtrCoding, 0, 4);
            BitsOutput(PtrCoding, InputVal, 4);
         }
         else
            ErrorMsg(BPE_RICE_CODING_ERROR);
      }
      else if (Option[2] == 1) // split only one bit.
      {
         if (InputVal <= 1)
         {
            BitsOutput(PtrCoding, InputVal + 2, 2);
         }
         else if (InputVal <= 3)
         {
            BitsOutput(PtrCoding, InputVal, 3);
         }
         else if (InputVal <= 5)
         {
            BitsOutput(PtrCoding, 0, 2);
            BitsOutput(PtrCoding, InputVal - 2, 2);
         }
         else if (InputVal <= 11)
         {
            BitsOutput(PtrCoding, 0, 3);
            BitsOutput(PtrCoding, InputVal - 6, 3);
         }
         else if (InputVal <= 15)
         {
            BitsOutput(PtrCoding, 0, 3);
            BitsOutput(PtrCoding, InputVal, 4);
         }
         else
            ErrorMsg(BPE_RICE_CODING_ERROR);
      }
      else if (Option[2] == 2) // split two bits.
      {
         if (InputVal <= 3)
         {
            BitsOutput(PtrCoding, InputVal + 4, 3);
         }
         else if (InputVal <= 7)
         {
            BitsOutput(PtrCoding, InputVal, 4);
         }
         else if (InputVal <= 11)
         {
            BitsOutput(PtrCoding, 0, 2);
            BitsOutput(PtrCoding, InputVal - 4, 3);
         }
         else if (InputVal <= 15)
         {
            BitsOutput(PtrCoding, InputVal - 12, 5);
         }
         else
            ErrorMsg(BPE_RICE_CODING_ERROR);
      }

      else if (Option[2] == 3) // uncoded
      {
         BitsOutput(PtrCoding, InputVal, 4);
      }
      else
         ErrorMsg(BPE_RICE_CODING_ERROR);//"Invalid split option!\n");
      break;
   default:
      ErrorMsg(BPE_RICE_CODING_ERROR);//"Invalid bit length!\n");
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
                            *decoded = (WordReadin) ? 7 : 6;
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
                    // 000100 : 10
                    // 000101 : 11
                    // 0001100 : 12
                    // 0001101 : 13
                    // 0001110 : 14
                    // 0001111 : 15
                    // 000000 : 6
                    // 000001 : 7
                    // 000010 : 8
                    // 000011 : 9

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
