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

#include "global.h"
#include <stdlib.h>
#include <stdio.h>

extern void RiceCoding(uint8_t InputVal, uint8_t BitLength, uint8_t *Option, StructCodingPara *PtrCoding);

extern void BitPlaneSymbolReset(StrSymbolDetails *SymbolStr);

void StagesEnCodingGaggles1(StructCodingPara *PtrCoding,
                            BitPlaneBits *BlockInfo,
                            int BlocksInGaggles,
                            uint8_t Option[],
                            BOOL FlagCodeOptionOutput[])
{
    int bi; // Block index
    int si; // Symbol index
    
    // nkeffix : Implement StageStop in 122x0b1c3 section 4.2.3.1 Table 4-5
    if (PtrCoding->PtrHeader->Header.Part2.StageStop_2Bits == 0 &&
        (PtrCoding->PtrHeader->Header.Part2.BitPlaneStop_5Bits == PtrCoding->BitPlane - 1 ||
            PtrCoding->altStageStop))
        return;

    for (bi = 0; bi < BlocksInGaggles; bi++)
    {
        if (BlockInfo[bi].BitMaxAC < PtrCoding->BitPlane) continue;
    
        for (si = 0; si < MAX_SYMBOLS_IN_BLOCK; si++)
        {
            StrSymbolDetails *sym = &BlockInfo[bi].SymbolsBlock[si];

            if (sym->type == ENUM_TYPE_P)
            {
                switch (sym->sym_len)
                {
                    case 1:
                    case 2:
                    case 3:
                    {
                        int i;
                        int counter = 0;
                        
                        if (sym->sym_len >= 2)
                        {
                            if (FlagCodeOptionOutput[sym->sym_len - 2] == FALSE)
                            {
                                FlagCodeOptionOutput[sym->sym_len - 2] = TRUE;
            
                                switch (sym->sym_len)
                                {
                                    case 2:  BitsOutput(PtrCoding, Option[0], 1); break;
                                    case 3:  BitsOutput(PtrCoding, Option[1], 2); break;
                                    case 4:  BitsOutput(PtrCoding, Option[2], 2); break;
                                    default: ErrorMsg(BPE_STAGE_CODING_ERROR);
                                }
                            }
                        }
            
                        RiceCoding(sym->sym_mapped_pattern, sym->sym_len, Option, PtrCoding);
        
                        for (i = 0; i < sym->sym_len; i++)
                        {
                            if (sym->sym_val & (1 << i)) counter++;
                        }
                        BitsOutput(PtrCoding, sym->sign, counter);
                        BitPlaneSymbolReset(sym);
                        break;
                    }
                    default:
                    {
                        ErrorMsg(BPE_STAGE_CODING_ERROR); // Invalid length
                    }
                }
            }
        }
    }
    // nkeffix : removed redundant check
    //if((PtrCoding->PtrHeader->Header.Part1.Part2Flag == TRUE)
    //   &&(PtrCoding->PtrHeader->Header.Part2.StageStop_2Bits == 00))
    //   return;
}
   // begin the encoding of TranB+TranD+TypeCi

void StagesEnCodingGaggles2(StructCodingPara *PtrCoding,
                            BitPlaneBits *BlockInfo,
                            int BlocksInGaggles,
                            uint8_t Option[],
                            BOOL FlagCodeOptionOutput[])
{
    int bi; // Block index
    int si; // Symbol index
    
    // nkeffix : Implement StageStop in 122x0b1c3 section 4.2.3.1 Table 4-5
    if (PtrCoding->PtrHeader->Header.Part2.StageStop_2Bits < 1 &&
        (PtrCoding->PtrHeader->Header.Part2.BitPlaneStop_5Bits == PtrCoding->BitPlane - 1 ||
        PtrCoding->altStageStop))
        return;
    
    for (bi = 0; bi < BlocksInGaggles; bi++)
    {
        if (BlockInfo[bi].BitMaxAC < PtrCoding->BitPlane) continue;
    
        for (si = 0; si < MAX_SYMBOLS_IN_BLOCK; si++)
        {
            StrSymbolDetails *sym = &BlockInfo[bi].SymbolsBlock[si];
            switch (sym->type)
            {
                case ENUM_TRAN_B:
                case ENUM_TRAN_D:
                case ENUM_TYPE_CI:
                {
                    if (sym->sym_len >= 2)
                    {
                        if (FlagCodeOptionOutput[sym->sym_len - 2] == FALSE)
                        {
                            FlagCodeOptionOutput[sym->sym_len - 2] = TRUE;
                            switch (sym->sym_len)
                            {
                                case 2:  BitsOutput(PtrCoding, Option[0], 1); break;
                                case 3:  BitsOutput(PtrCoding, Option[1], 2); break;
                                case 4:  BitsOutput(PtrCoding, Option[2], 2); break;
                                default: ErrorMsg(BPE_STAGE_CODING_ERROR);
                            }
                        }
                    }
                    RiceCoding(sym->sym_mapped_pattern, sym->sym_len, Option, PtrCoding);
                    if (sym->type == ENUM_TYPE_CI)
                    {
                        int i;
                        int counter = 0;
                        for (i = 0; i < sym->sym_len; i++)
                        {
                            if (sym->sym_val & (1 << i)) counter++;
                        }
                        BitsOutput(PtrCoding, sym->sign, counter);
                    }

                    BitPlaneSymbolReset(sym);
                    break;
                }
            }
        }
    }
}

void StagesEnCodingGaggles3(StructCodingPara *PtrCoding,
                            BitPlaneBits *BlockInfo,
                            int BlocksInGaggles,
                            uint8_t Option[],
                            BOOL FlagCodeOptionOutput[])
{
    int bi; // Block index
    int si; // Symbol index
    
    // nkeffix : Implement StageStop in 122x0b1c3 section 4.2.3.1 Table 4-5
    if (PtrCoding->PtrHeader->Header.Part2.StageStop_2Bits < 2 &&
        (PtrCoding->PtrHeader->Header.Part2.BitPlaneStop_5Bits == PtrCoding->BitPlane - 1 ||
            PtrCoding->altStageStop))
        return;
    
    for (bi = 0; bi < BlocksInGaggles; bi++)
    {
        if (BlockInfo[bi].BitMaxAC < PtrCoding->BitPlane) continue;
    
        for (si = 0; si < MAX_SYMBOLS_IN_BLOCK; si++)
        {
            StrSymbolDetails *sym = &BlockInfo[bi].SymbolsBlock[si];
            
            switch (sym->type)
            {
                case ENUM_TRAN_GI:
                case ENUM_TRAN_HI:
                case ENUM_TYPE_HIJ:
                {
                    if (sym->sym_len >= 2)
                    {
                        if (FlagCodeOptionOutput[sym->sym_len - 2] == FALSE)
                        {
                            FlagCodeOptionOutput[sym->sym_len - 2] = TRUE;
                            switch (sym->sym_len)
                            {
                                case 2:  BitsOutput(PtrCoding, Option[0], 1); break;
                                case 3:  BitsOutput(PtrCoding, Option[1], 2); break;
                                case 4:  BitsOutput(PtrCoding, Option[2], 2); break;
                                default: ErrorMsg(BPE_STAGE_CODING_ERROR);
                            }
                        }
                    }
                    RiceCoding(sym->sym_mapped_pattern , sym->sym_len, Option, PtrCoding);
                    if (sym->type == ENUM_TYPE_HIJ)
                    {
                        int i;
                        int counter = 0;
                        for (i = 0; i < sym->sym_len; i++)
                        {
                            if (sym->sym_val & (1 << i)) counter++;
                        }
                        BitsOutput(PtrCoding, sym->sign, counter);
                    }
                    BitPlaneSymbolReset(sym);
                    break;
                }
            }
        }
    }
}

extern void RiceDecoding(uint32_t *decoded,
            short BitLength,
            uint8_t *splitOption,
            StructCodingPara *Ptr);

extern void DeMappingPattern(StrSymbolDetails *StrSymbol);

void StagesDeCodingGaggles1(StructCodingPara *PtrCoding,
                     BitPlaneBits *BlockCodingInfo,
                     int BlocksInGaggles,
                     uint8_t *CodeOptionsAllGaggles,
                     BOOL *FlagCodeOptionOutput)
{
   uint8_t temp_x;
   uint8_t temp_y;
   uint8_t counter;
   int BlockSeq;
   uint8_t RefCounter = 0;
   int i;
   uint8_t BitPlane = PtrCoding->BitPlane;
   StrSymbolDetails StrSymbol;

   for (BlockSeq = 0; BlockSeq < BlocksInGaggles; BlockSeq++)
   {
      if (BlockCodingInfo[BlockSeq].BitMaxAC < BitPlane)
         continue;
      counter = 0;

      RefCounter = 0;
      for(i = 0; i < 3; i++)
      {
         if((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
         {
            if(((i == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL3_2bits >= BitPlane)) || // HL3 band.
               ((i == 1) && (PtrCoding->PtrHeader->Header.Part4.CustomWtLH3_2bits >= BitPlane))  ||// HL3 band.
               ((i == 2) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHH3_2bits >= BitPlane)))  // HL3 band.
               continue;
         }
         RefCounter ++;
         if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TypeP & (1 << (2 - i))) == 0)
            counter ++;
      }

      if(RefCounter != 0)
      {
         if(counter != 0)
         {
            uint32_t temp_DWORD;
            if(counter != 1)
            {
               if(FlagCodeOptionOutput[counter - 2] == FALSE)
               {
                  FlagCodeOptionOutput[counter - 2] = TRUE;
                  if(counter == 2)
                  {
                     BitsRead(PtrCoding, &temp_DWORD, 1);
                  }
                  else
                  {
                     BitsRead(PtrCoding, &temp_DWORD, 2);
                  }
                  CodeOptionsAllGaggles[counter - 2] = (uint8_t) temp_DWORD;

                  if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                     && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
                  {
                     PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                     PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
                     PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 1;
                     PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                     return;
                  }
               }
            }

            RiceDecoding(&temp_DWORD, counter, CodeOptionsAllGaggles, PtrCoding);

            if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
               && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
            {
               PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
               PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
               PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 1;
               PtrCoding->DecodingStopLocations.LocationFind = TRUE;
               return;
            }
            StrSymbol.sym_mapped_pattern= (uint8_t)temp_DWORD;
            StrSymbol.sym_len = counter;
            StrSymbol.type = ENUM_TYPE_P;
            DeMappingPattern(&StrSymbol);

            for(i = 0; i < 3; i++)
            {

               if((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
               {

               if(((i == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL3_2bits >= BitPlane)) || // HL3 band.
                  ((i == 1) && (PtrCoding->PtrHeader->Header.Part4.CustomWtLH3_2bits >= BitPlane))  ||// HL3 band.
                  ((i == 2) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHH3_2bits >= BitPlane)))  // HL3 band.
                  continue;
               }

               temp_x = (i >= 1 ? 1 : 0);
               temp_y = (i != 1 ? 1 : 0);
               if ((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TypeP & (1 << (2 - i))) == 0)
               {
                  temp_DWORD = (StrSymbol.sym_val & (1 << (counter - 1)));
                  counter --;
                  if(temp_DWORD > 0)
                  {
                     temp_DWORD = (1 << (BitPlane -1));
                     BlockCodingInfo[BlockSeq].PtrBlockAddress[temp_x][temp_y] += temp_DWORD;
                     BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TypeP += (1 << (2 - i));
                     BitsRead(PtrCoding, &temp_DWORD, 1);
                     if (temp_DWORD == NEGATIVE_SIGN)
                        BlockCodingInfo[BlockSeq].PtrBlockAddress[temp_x][temp_y] =
                        -BlockCodingInfo[BlockSeq].PtrBlockAddress[temp_x][temp_y];
                     if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                        && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
                     {
                        PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = temp_x;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = temp_y;
                        PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                        return;
                     }
                  }
               }
               else
               {
                  BlockCodingInfo[BlockSeq].RefineBits.RefineParent.ParentRefSymbol += ( 1 << (2 - i) );
                  BlockCodingInfo[BlockSeq].RefineBits.RefineParent.ParentSymbolLength ++;
               }

               if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                  && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
               {
                  PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                  PtrCoding->DecodingStopLocations.X_LocationStopDecoding = temp_x;
                  PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = temp_y;
                  PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                  return;
               }
            }
         }
         else
         {
            BlockCodingInfo[BlockSeq].RefineBits.RefineParent.ParentSymbolLength = RefCounter;
            BlockCodingInfo[BlockSeq].RefineBits.RefineParent.ParentRefSymbol = 0;
            if((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
            {
               for( i = 0; i < 3; i++)
               {
                  if((i == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL3_2bits < BitPlane)) // HL3 band.
                     BlockCodingInfo[BlockSeq].RefineBits.RefineParent.ParentRefSymbol += 0x4;

                  if((i == 1) && (PtrCoding->PtrHeader->Header.Part4.CustomWtLH3_2bits < BitPlane))  //|| HL3 band.
                     BlockCodingInfo[BlockSeq].RefineBits.RefineParent.ParentRefSymbol += 0x2;

                  if((i == 2) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHH3_2bits < BitPlane))
                     BlockCodingInfo[BlockSeq].RefineBits.RefineParent.ParentRefSymbol += 0x1;
               }
            }
            else
            {
               BlockCodingInfo[BlockSeq].RefineBits.RefineParent.ParentRefSymbol = 0x7;

            }
         }
      }
   }
}

void StagesDeCodingGaggles2(StructCodingPara *PtrCoding,
                     BitPlaneBits *BlockCodingInfo,
                     int BlocksInGaggles,
                     uint8_t *CodeOptionsAllGaggles,
                     BOOL *FlagCodeOptionOutput)
{
   uint8_t temp_x;
   uint8_t temp_y;
   uint8_t counter;
   int BlockSeq;
   int i;
   int k;
   int p;
   uint32_t temp_DWORD;
   uint8_t BitPlane = PtrCoding->BitPlane;
   StrSymbolDetails StrSymbol;
   BOOL Flag;

   // nkeffix : Implement StageStop in 122x0b1c3 section 4.2.3.1 Table 4-5
   if(PtrCoding->PtrHeader->Header.Part2.StageStop_2Bits < 1 &&
      (PtrCoding->PtrHeader->Header.Part2.BitPlaneStop_5Bits == BitPlane - 1 ||
       PtrCoding->altStageStop))
      return;

   for (BlockSeq = 0; BlockSeq < BlocksInGaggles; BlockSeq++)
   {
      if (BlockCodingInfo[BlockSeq].BitMaxAC < BitPlane)
         continue;
      Flag = FALSE;
         // decoding TranB
      counter = 0;
      for(i = 0; i < 3; i++)
      {
         if((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
         {
            if(((i == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL2_2bits >= BitPlane) &&
               (PtrCoding->PtrHeader->Header.Part4.CustomWtHL1_2bits >= BitPlane)) // HL3 band.
               || ((i == 1) &&   (PtrCoding->PtrHeader->Header.Part4.CustomWtLH2_2bits >= BitPlane)&&
               (PtrCoding->PtrHeader->Header.Part4.CustomWtLH1_2bits >= BitPlane)) // HL3 band.
               || ((i == 2) &&   (PtrCoding->PtrHeader->Header.Part4.CustomWtHH2_2bits >= BitPlane)&&
               (PtrCoding->PtrHeader->Header.Part4.CustomWtHH1_2bits >= BitPlane))) // HL3 band.
               continue;
         }
         Flag = TRUE;
         if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & (1 << (2 - i))) == 0)
            counter ++;
      }

      if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
         && PtrCoding->RateReached == TRUE && (!PtrCoding->DecodingStopLocations.LocationFind))
      {
         PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
         PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
         PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
         PtrCoding->DecodingStopLocations.LocationFind = TRUE;
         return;
      }

      if(Flag == FALSE)
         continue;

      if (BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranB != 1)
      {
         uint32_t Temp_Word;
         BitsRead(PtrCoding, &Temp_Word, 1);
         BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranB = (uint8_t)Temp_Word;

         if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
            && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
         {
            PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
            PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
            PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
            PtrCoding->DecodingStopLocations.LocationFind = TRUE;
            return;
         }
      }
      if(BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranB == 0)
         continue;
      // decoding TranD

      if(counter != 0)
      {
         uint32_t TempWord;

         if(counter != 1)
         {
            if(FlagCodeOptionOutput[counter - 2] == FALSE)
            {
               FlagCodeOptionOutput[counter - 2] = TRUE;
               if(counter == 2)
               {
                  BitsRead(PtrCoding, &TempWord, 1);
               }
               else
               {
                  BitsRead(PtrCoding, &TempWord, 2);
               }
               CodeOptionsAllGaggles[counter - 2] = (uint8_t) TempWord;

               if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                  && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
               {
                  PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                  if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & 0x4) == 0)
                  {
                     PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
                     PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
                  }
                  else if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & 0x2) == 0)
                  {
                     PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 2;
                     PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 0;
                  }
                  else
                  {
                     PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 2;
                     PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
                  }

                  PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                  return;
               }
            }
         }

         RiceDecoding(&TempWord, counter, CodeOptionsAllGaggles, PtrCoding);

         if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
            && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
         {
            PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
            if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & 0x4) == 0)
            {
               PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
               PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
            }
            else if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & 0x2) == 0)
            {
               PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 2;
               PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 0;
            }
            else
            {
               PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 2;
               PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
            }

            PtrCoding->DecodingStopLocations.LocationFind = TRUE;
            return;
         }
         StrSymbol.sym_mapped_pattern = (uint8_t) TempWord;
         StrSymbol.sym_len = counter;
         StrSymbol.type = ENUM_TRAN_D;
         DeMappingPattern(&StrSymbol);
         // assign StrSymbol.sym_val to TranD.
         for( i = 0; i < 3; i ++)
         {
            if((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
            {
               if(((i == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL2_2bits >= BitPlane)&&
                  (PtrCoding->PtrHeader->Header.Part4.CustomWtHL1_2bits >= BitPlane)) // HL3 band.
                  || ((i == 1) &&   (PtrCoding->PtrHeader->Header.Part4.CustomWtLH2_2bits >= BitPlane)&&
                  (PtrCoding->PtrHeader->Header.Part4.CustomWtLH1_2bits >= BitPlane)) // HL3 band.
                  || ((i == 2) &&   (PtrCoding->PtrHeader->Header.Part4.CustomWtHH2_2bits >= BitPlane)&&
                  (PtrCoding->PtrHeader->Header.Part4.CustomWtHH1_2bits >= BitPlane))) // HL3 band.
                  continue;
            }

            if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & ( 1 << (2 - i))) > 0)
               continue;
            BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD += (((StrSymbol.sym_val >> (counter - 1)) & 0x01) << (2- i));
            counter --;
         }

      }

      for(k = 0; k < 3; k ++)
      {
         BlockCodingInfo[BlockSeq].RefineBits.RefineChildren.ChildrenRefSymbol <<= 4;

         if((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
         {
            if(((k == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL2_2bits >= BitPlane)) // HL3 band.
               || ((k == 1) &&   (PtrCoding->PtrHeader->Header.Part4.CustomWtLH2_2bits >= BitPlane)) // HL3 band.
               || ((k == 2) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHH2_2bits >= BitPlane))) // HL3 band.
               continue;
         }
         if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & ( 1 << (2 - k))) > 0)
         {
            counter = 0;
            for (i = 0; i < 4; i ++)
            {
               if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TypeCi[k].TypeC & (1 << (3 - i))) == 0)
                  counter ++;
            }
            if(counter != 0)
            {
               uint32_t TempWord;
               short t = 0;
               if(counter != 1)
               {
                  if(FlagCodeOptionOutput[counter - 2] == FALSE)
                  {
                     FlagCodeOptionOutput[counter - 2] = TRUE;
                     if(counter == 2)
                     {
                        BitsRead(PtrCoding, &TempWord, 1);
                     }
                     else
                     {
                        BitsRead(PtrCoding, &TempWord, 2);
                     }
                     CodeOptionsAllGaggles[counter - 2] = (uint8_t) TempWord;

                     if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                           && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
                        {
                           PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                           if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & 0x4) == 0)
                           {
                              PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
                              PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
                           }
                           else if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & 0x2) == 0)
                           {
                              PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 2;
                              PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 0;
                           }
                           else
                           {
                              PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 2;
                              PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
                           }

                           PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                           return;
                        }
                  }
               }

               RiceDecoding(&TempWord, counter, CodeOptionsAllGaggles, PtrCoding);
               if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                  && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
               {
                     PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                     if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & 0x4) == 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
                     }
                     else if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & 0x2) == 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 2;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 0;
                     }
                     else
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 2;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 2;
                     }

                     PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                     return;
               }

               StrSymbol.sym_mapped_pattern =  (uint8_t)TempWord;
               StrSymbol.sym_len = counter;
               StrSymbol.type = ENUM_TYPE_CI;
               DeMappingPattern(&StrSymbol);

               temp_x = (k >= 1 ? 1 : 0);
               temp_x *= 2;
               temp_y = (k != 1 ? 1 : 0);
               temp_y *= 2;
               for (i = temp_x ; i < temp_x + 2; i ++)
                  for ( p = temp_y; p < temp_y + 2; p ++)
                  {
                     if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TypeCi[k].TypeC & (1 << (3 - t))) == 0)
                     {
                        temp_DWORD = ((StrSymbol.sym_val >> (counter - 1)) & 0x01);
                        if(temp_DWORD > 0)
                        {
                           uint32_t TempWord;
                           BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TypeCi[k].TypeC += (1 << (3 - t));
                           BlockCodingInfo[BlockSeq].PtrBlockAddress[i][p] += (1 << (BitPlane - 1));
                           BitsRead(PtrCoding, &TempWord, 1); // sign bit
                           if (TempWord == NEGATIVE_SIGN)
                              BlockCodingInfo[BlockSeq].PtrBlockAddress[i][p] = - BlockCodingInfo[BlockSeq].PtrBlockAddress[i][p];

                           if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                              && PtrCoding->RateReached == TRUE && (!PtrCoding->DecodingStopLocations.LocationFind))
                           {
                              PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                              PtrCoding->DecodingStopLocations.X_LocationStopDecoding = i;
                              PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = p;
                              PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                              return;
                           }
                        }
                        counter --;
                     }
                     else
                     {
                        BlockCodingInfo[BlockSeq].RefineBits.RefineChildren.ChildrenSymbolLength ++;
                        BlockCodingInfo[BlockSeq].RefineBits.RefineChildren.ChildrenRefSymbol += (1 << (3 - t));
                     }
                     t ++;

                     if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                        && PtrCoding->RateReached == TRUE && (!PtrCoding->DecodingStopLocations.LocationFind))
                     {
                        PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = i;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = p;
                        PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                        return;
                     }

                  }
            }
            else
            {
               BlockCodingInfo[BlockSeq].RefineBits.RefineChildren.ChildrenSymbolLength += 4;
               BlockCodingInfo[BlockSeq].RefineBits.RefineChildren.ChildrenRefSymbol += 0xF;
            }
         }
      }
   }
}

void StagesDeCodingGaggles3(StructCodingPara *PtrCoding,
                     BitPlaneBits *BlockCodingInfo,
                     int BlocksInGaggles,
                     uint8_t *CodeOptionsAllGaggles,
                     BOOL *FlagCodeOptionOutput)
{
   uint8_t temp_x;
   uint8_t temp_y;
   uint8_t counter;
   int BlockSeq;
   int i;
   int k;
   int p;
   uint32_t temp_DWORD;
   BOOL flag = FALSE;
   uint8_t BitPlane = PtrCoding->BitPlane;
   StrSymbolDetails StrSymbol;

   // nkeffix : Implement StageStop in 122x0b1c3 section 4.2.3.1 Table 4-5
   if (PtrCoding->PtrHeader->Header.Part2.StageStop_2Bits < 2 &&
      (PtrCoding->PtrHeader->Header.Part2.BitPlaneStop_5Bits == BitPlane - 1 ||
       PtrCoding->altStageStop))
      return;

   for (BlockSeq = 0; BlockSeq < BlocksInGaggles; BlockSeq ++)
   {
      if (BlockCodingInfo[BlockSeq].BitMaxAC < BitPlane)
         continue;

      if (BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranB == 0)
         continue;
   // TranGi

      if(PtrCoding->PtrHeader->Header.Part1.SegmentCount_8Bits == 28 && PtrCoding->BitPlane  == 13
         && BlockSeq== 10)
      BlockSeq = BlockSeq;

      flag = FALSE;
      counter = 0;
      if((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
      {
         for(k = 0; k < 3; k ++)
         {
            if((((k == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL1_2bits >= BitPlane)) // HL3 band.
               || ((k == 1) &&   (PtrCoding->PtrHeader->Header.Part4.CustomWtLH1_2bits >= BitPlane)) // HL3 band.
               || ((k == 2) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHH1_2bits >= BitPlane)))) // HL3 band.
               continue;
            flag = TRUE;

            if(((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & ( 1 << (2 - k))) > 0) &&
               ((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi & ( 1 << (2 - k))) == 0))
               counter ++;
         }
      }
      else
      {
         flag = TRUE;
         for(k = 0; k < 3; k ++)
         {
            if(((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & ( 1 << (2 - k))) > 0) &&
               ((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi & ( 1 << (2 - k))) == 0))
               counter ++;
         }
      }

      if (flag == FALSE)
         continue;

      if(counter > 0)
      {
         uint32_t TempWord;

         if(counter != 1)
         {
            if(FlagCodeOptionOutput[counter - 2] == FALSE)
            {
               FlagCodeOptionOutput[counter - 2] = TRUE;
               if(counter == 2)
               {
                  BitsRead(PtrCoding, &TempWord, 1);
               }
               else
               {
                  BitsRead(PtrCoding, &TempWord, 2);
               }
               CodeOptionsAllGaggles[counter - 2] = (uint8_t) TempWord;

               if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                  && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
               {
                     PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                     if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi  & 0x4) == 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 4;
                     }
                     else if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi & 0x2)== 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 4;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 0;
                     }
                     else
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 4;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 4;
                     }

                     PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                     return;
               }
            }
         }

         RiceDecoding(&TempWord, counter, CodeOptionsAllGaggles, PtrCoding);

         if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                  && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
               {
                     PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                     if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi  & 0x4) == 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 4;
                     }
                     else if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi & 0x2) == 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 4;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 0;
                     }
                     else
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 4;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 4;
                     }

                     PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                     return;
               }
         StrSymbol.sym_mapped_pattern = (uint8_t)TempWord;
         StrSymbol.sym_len = counter;
         StrSymbol.type = ENUM_TRAN_GI;
         DeMappingPattern(&StrSymbol);

         for( i = 0; i < 3; i++)
         {
            if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranD & ( 1 << (2 - i))) > 0)
            {

               if ((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
               {
                  if((((i == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL1_2bits >= BitPlane)) // HL3 band.
                  || ((i == 1) &&   (PtrCoding->PtrHeader->Header.Part4.CustomWtLH1_2bits >= BitPlane)) // HL3 band.
                  || ((i == 2) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHH1_2bits >= BitPlane)))) // HL3 band.
                     continue;
               }
               if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi & ( 1 << (2 - i))) == 0)
               {
                  BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi += (((StrSymbol.sym_val >> (counter - 1)) & 0x01)  << (2 - i));
                  counter --;
               }
            }
         }
      }
   // TranHi
      for(k = 0; k < 3; k ++)
      {
         if((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
         {
            if((((k == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL1_2bits >= BitPlane)) // HL3 band.
               || ((k == 1) &&   (PtrCoding->PtrHeader->Header.Part4.CustomWtLH1_2bits >= BitPlane)) // HL3 band.
               || ((k == 2) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHH1_2bits >= BitPlane)))) // HL3 band.
               continue;
         }

         if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi & ( 1 << (2 - k))) != 0)
         {
            uint32_t TempWord;
            counter = 0;
            for (i = 0; i < 4; i ++)
            {
               if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranHi[k].TranH & ( 1 << (3 - i))) == 0)
                  counter ++;
            }
            if (counter == 0)
               continue;

            if(counter != 1)
            {
               if(FlagCodeOptionOutput[counter - 2] == FALSE)
               {
                  FlagCodeOptionOutput[counter - 2] = TRUE;
                  if(counter == 2)
                  {
                     BitsRead(PtrCoding, &TempWord, 1);
                  }
                  else
                  {
                     BitsRead(PtrCoding, &TempWord, 2);
                  }
                  CodeOptionsAllGaggles[counter - 2] = (uint8_t) TempWord;

                  if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                     && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
                  {
                     PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                     if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi  & 0x4) == 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 4;
                     }
                     else if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi & 0x2) == 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 4;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 0;
                     }
                     else
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 4;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 4;
                     }

                     PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                     return;
                  }
               }
            }

            RiceDecoding(&TempWord, counter, CodeOptionsAllGaggles, PtrCoding);


            if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                  && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
               {
                     PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                     if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi  & 0x4) == 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 0;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 4;
                     }
                     else if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranGi & 0x2) == 0)
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 4;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 0;
                     }
                     else
                     {
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 4;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 4;
                     }

                     PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                     return;
               }

            StrSymbol.sym_mapped_pattern = (uint8_t) TempWord;
            StrSymbol.sym_len = counter;
            StrSymbol.type = ENUM_TRAN_HI;
            DeMappingPattern(&StrSymbol);

            if (StrSymbol.sym_val > 0)
            {
               for (i = 0; i < 4; i ++)
               {
                  if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranHi[k].TranH & ( 1 << (3 - i))) != 0)
                     continue;
                  temp_DWORD = ((StrSymbol.sym_val >> (counter - 1)) & 0x01);
                  BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranHi[k].TranH += (((short)temp_DWORD) <<  (3 - i));
                  counter --;
               }
            }
         }
      }

      if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
         && PtrCoding->RateReached == TRUE && (!PtrCoding->DecodingStopLocations.LocationFind))
      {
         PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
         PtrCoding->DecodingStopLocations.X_LocationStopDecoding = 4;
         PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = 0;
         PtrCoding->DecodingStopLocations.LocationFind = TRUE;
         return;
      }

      //TypeHij
      for (i = 0; i < 3; i ++)
      {
         if((PtrCoding->PtrHeader->Header.Part4).DWTType == INTEGER_WAVELET)
         {
            if((((i == 0) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHL1_2bits >= BitPlane)) // HL3 band.
               || ((i == 1) &&   (PtrCoding->PtrHeader->Header.Part4.CustomWtLH1_2bits >= BitPlane)) // HL3 band.
               || ((i == 2) && (PtrCoding->PtrHeader->Header.Part4.CustomWtHH1_2bits >= BitPlane)))) // HL3 band.
               continue;
         }

         for ( k = 0;  k < 4;  k ++)
         {
            temp_x = (i >= 1 ? 1 : 0) * 4 + (k >= 2 ? 1 : 0) * 2;
            temp_y = (i != 1 ? 1 : 0) * 4 + (k % 2) * 2;

            BlockCodingInfo[BlockSeq].RefineBits.RefineGrandChildren[i].GrandChildrenRefSymbol <<= 4;

            if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TranHi[i].TranH & ( 1 << ( 3 - k))) > 0)
            {
               uint32_t TempWord;
               counter = 0;
               for (p = 0; p < 4; p++)
               {
                  if(((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TypeHij[i].TypeHij[k].TranH & (1 << ( 3 - p)))) == 0)
                     counter ++;
               }
               if (counter != 0)
               {
                  if(counter != 1)
                  {
                     if(FlagCodeOptionOutput[counter - 2] == FALSE)
                     {
                        FlagCodeOptionOutput[counter - 2] = TRUE;
                        if(counter == 2)
                           BitsRead(PtrCoding, &TempWord, 1);
                        else
                           BitsRead(PtrCoding, &TempWord, 2);
                        CodeOptionsAllGaggles[counter - 2] = (uint8_t) TempWord;

                        if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                           && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
                        {
                              PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                              PtrCoding->DecodingStopLocations.X_LocationStopDecoding = temp_x;
                              PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = temp_y;
                              PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                              return;
                        }
                     }
                  }

                  RiceDecoding(&TempWord, counter, CodeOptionsAllGaggles, PtrCoding);

                  if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                     && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
                  {
                     PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                     PtrCoding->DecodingStopLocations.X_LocationStopDecoding = temp_x;
                     PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = temp_y;
                     PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                     return;
                  }

                  StrSymbol.sym_mapped_pattern = (uint8_t) TempWord;
                  StrSymbol.sym_len = (uint8_t) counter;
                  StrSymbol.type = ENUM_TYPE_HIJ;
                  DeMappingPattern(&StrSymbol);


                  for (p = 0; p < 4; p ++)
                  {
                     if((BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TypeHij[i].TypeHij[k].TranH & ( 1 << (3 - p))) != 0)
                     {
                        BlockCodingInfo[BlockSeq].RefineBits.RefineGrandChildren[i].GrandChildrenRefSymbol += ( 1 << (3 - p));
                        BlockCodingInfo[BlockSeq].RefineBits.RefineGrandChildren[i].GrandChildrenSymbolLength ++;
                        continue;
                     }
                     temp_DWORD = (StrSymbol.sym_val & (1 << (counter - 1)));
                     counter --;
                     if(temp_DWORD > 0)
                     {
                        uint32_t TempWord;
                        BlockCodingInfo[BlockSeq].PtrBlockAddress[temp_x + p / 2][temp_y + p % 2] += (1 << (BitPlane - 1));
                        BlockCodingInfo[BlockSeq].StrPlaneHitHistory.TypeHij[i].TypeHij[k].TranH += (1 << ( 3 - p));
                        BitsRead(PtrCoding, &TempWord, 1); // sign bit
                        if (TempWord == NEGATIVE_SIGN)
                           BlockCodingInfo[BlockSeq].PtrBlockAddress[temp_x + p / 2][temp_y + p % 2] = - BlockCodingInfo[BlockSeq].PtrBlockAddress[temp_x + p / 2][temp_y + p % 2];

                        if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                           && PtrCoding->RateReached == TRUE && !PtrCoding->DecodingStopLocations.LocationFind)
                        {
                           PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                           PtrCoding->DecodingStopLocations.X_LocationStopDecoding = temp_x + p / 2;
                           PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = temp_y + p % 2;
                           PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                           return;
                        }
                     }
                     if ((PtrCoding->DecodingStopLocations.BitPlaneStopDecoding != -1)
                        && PtrCoding->RateReached == TRUE
                        && (!PtrCoding->DecodingStopLocations.LocationFind ))
                     {
                        PtrCoding->DecodingStopLocations.BlockNoStopDecoding = BlockSeq;
                        PtrCoding->DecodingStopLocations.X_LocationStopDecoding = temp_x + p / 2;
                        PtrCoding->DecodingStopLocations.Y_LocationStopDecoding = temp_y + p % 2;
                        PtrCoding->DecodingStopLocations.LocationFind = TRUE;
                        return;
                     }
                  }
               }
               else
               {
                  BlockCodingInfo[BlockSeq].RefineBits.RefineGrandChildren[i].GrandChildrenRefSymbol += 0xF;
                  BlockCodingInfo[BlockSeq].RefineBits.RefineGrandChildren[i].GrandChildrenSymbolLength += 4;
               }
            }
         }
      }
   }
   return;
}
