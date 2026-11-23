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
#include "global.h"

void OutputCodeWord(StructCodingPara * Ptr)
{
    Ptr->Bits->CodeWordAlighmentBits++;
    Ptr->Bits->SegBitCounter++;
    Ptr->Bits->TotalBitCounter++;
    
    if (Ptr->Bits->CodeWordAlighmentBits == Ptr->Bits->CodeWord_Length)
    {
        /* MSB first, not machine dependant */
        uint32_t n;
    
        for (n = Ptr->Bits->CodeWord_Length; n > 0; n -= 8)
        {
            putc((uint8_t)(Ptr->Bits->ByteBuffer_4Bytes >> (n - 8)), Ptr->Bits->F_Bits);
        }
        Ptr->Bits->CodeWordAlighmentBits = 0;
    }
}



void BitsOutput(StructCodingPara *Ptr, uint32_t bit, int length)
{
    uint32_t bit_mask;

    if (length == 0) return;
    if (Ptr->SegmentFull == TRUE) return;
    
    // determine if there is limitation on the total number of bytes output in this segment.
    // first check if Part 2 of the header is available.
    
    if (Ptr->PtrHeader->Header.Part2.SegByteLimit_27Bits != 0)
    {
        // means that there might be a limitation on the total number of bits.
        if ((Ptr->Bits->SegBitCounter + length) >= (Ptr->PtrHeader->Header.Part2.SegByteLimit_27Bits * 8))
        {
            int bits_left;

            Ptr->SegmentFull = TRUE;
            bits_left = (int)(Ptr->PtrHeader->Header.Part2.SegByteLimit_27Bits * 8 - Ptr->Bits->SegBitCounter);
            bit_mask  = (uint32_t)1 << (length - 1);
    
            while (bits_left > 0)
            {
                Ptr->Bits->ByteBuffer_4Bytes <<= 1;
                if (bit & bit_mask) Ptr->Bits->ByteBuffer_4Bytes++;
                OutputCodeWord(Ptr);
                bit_mask >>= 1;
                bits_left--;
            }
    
            return;
        }
    }

    // Determine the Codeword length by the header part 4.
    // When length > 32 : bits #32-63 are all zeros ???
    while (length > 32)
    {
        Ptr->Bits->ByteBuffer_4Bytes <<= 1;
        OutputCodeWord(Ptr);
        length--;
    }

    bit_mask = (uint32_t)1 << (length - 1);
    while (length > 0)
    {
        Ptr->Bits->ByteBuffer_4Bytes <<= 1;
        if (bit & bit_mask) Ptr->Bits->ByteBuffer_4Bytes++;
        OutputCodeWord(Ptr);
        bit_mask >>= 1;
        length--;
    }

    return;
}


short BitsRead(StructCodingPara *Ptr,
            uint32_t *bit,
            short length)
{
   uint8_t i;
   *bit = 0;

   if((length == 0) || (Ptr->SegmentFull == TRUE))
   {
      *bit = 0;
      return BPE_OK;
   }

   if(Ptr->RateReached == TRUE)
   {
      *bit = 0;
      return BPE_OK;
   }

   if (Ptr->SegmentFull != TRUE)
   {
      for (i = 0; i < length; i ++)
      {
         if ( Ptr->Bits->CodeWordAlighmentBits == 0)
         {
            Ptr->Bits->ByteBuffer_4Bytes = getc(Ptr->Bits->F_Bits) ;
            Ptr->Bits->CodeWordAlighmentBits = 8;
         }
         (*bit) <<= 1;
         (*bit) += (uint8_t) ((Ptr->Bits->ByteBuffer_4Bytes >> (Ptr->Bits->CodeWordAlighmentBits - 1)) & 0x01);
         Ptr->Bits->CodeWordAlighmentBits --;
         Ptr->Bits->SegBitCounter++;
         Ptr->Bits->TotalBitCounter ++;

         // for embedded decoding rate control
         if((Ptr->Bits->SegBitCounter >= Ptr->DecodingAllowedBitsSizeInSegment)
            && Ptr->DecodingAllowedBitsSizeInSegment != 0)
         {
            uint32_t CurrentTotalBytes =  (Ptr->Bits->SegBitCounter +
               Ptr->Bits->CodeWordAlighmentBits) / 8;

            Ptr->RateReached = TRUE;
            Ptr->DecodingStopLocations.BitPlaneStopDecoding = Ptr->BitPlane - 1;
            Ptr->DecodingStopLocations.TotalBitsReadThisTime = i + 1;
            (*bit) <<= (length - i - 1);
            while(CurrentTotalBytes < Ptr->PtrHeader->Header.Part2.SegByteLimit_27Bits)
            {
               getc(Ptr->Bits->F_Bits);
               CurrentTotalBytes++;
            }
            Ptr->SegmentFull = TRUE;
            return BPE_OK;
         }
         // end of rate control.
      }
   }
   return BPE_OK;
}

//void SegmentBufferFlushEncoder(StructCodingPara *StrCoding) // flush codes and reset
//{
//
//   if(StrCoding->Bits->CodeWordAlighmentBits != 0)
//   {
//      int shift = 0;
//      shift = StrCoding->Bits->CodeWord_Length - StrCoding->Bits->CodeWordAlighmentBits;
//      BitsOutput(StrCoding, 0, shift);
//   }
//
//   if((StrCoding->PtrHeader->Header.Part2.SegByteLimit_27Bits != 0)
//      && (StrCoding->SegmentFull == FALSE)
//      && StrCoding->PtrHeader->Header.Part2.UseFill == TRUE)
//   {
//      while(StrCoding->SegmentFull == FALSE)
//      {
//         BitsOutput(StrCoding, 0, 8);
//      }
//   }
//
//   StrCoding->Bits->SegBitCounter = 0;
//   StrCoding->Bits->ByteBuffer_4Bytes = 0;
//   StrCoding->Bits->CodeWordAlighmentBits = 0;
//   return;
//}


