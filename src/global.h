/* this header file defines global marco variabls and constans. Most c files
include this file*/
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <stdio.h>
#include <stdint.h>

//
// Maximum Supported File path
//
#define MAX_FILE_NAME_SIZE 256

#define BUFFER_LENGTH 150
#define TRUE 1
#define FALSE 0
#define MAX_SYMBOLS_IN_BLOCK 22

#define INTEGER_WAVELET 1
#define FLOAT_WAVELET 0
#define GAGGLE_SIZE 16

#define NOTRANSPOSE 0
#define TRANSPOSE 1

//typedef unsigned char uint8_t;
//typedef unsigned int  uint32_t;
//typedef unsigned long uint32_t;
//typedef unsigned short uint16_t;
typedef unsigned char BOOL;

//typedef signed int SINT;
//typedef signed char int8_t;
//typedef signed long int32_t;
//typedef double DOUBLE4;


#define ERR_MSG(err) BpeErrorMsg[err]
#define AMPLITUDE(a) ((a) >=0 ? (a): -(a))


#define BPE_OK            0
#define BPE_STREAM_END    1
#define BPE_FILE_ERROR   (2)
#define BPE_STREAM_ERROR (3)
#define BPE_DATA_ERROR   (4)
#define BPE_MEM_ERROR    (5)
#define BPE_BLOCKSCAN_CODING_ERROR    (6)
#define BPE_DYNAMICAL_RANGE_ERROR (7)
#define BPE_RATE_ERROR    (8)
#define BPE_RATE_UNREACHABLE (9)
#define BPE_WAVELET_INVALID (10)
#define BPE_IMAGE_SIZE_WRONG (11)
#define BPE_SCALLING_FILE_ERROR (12)
#define BPE_INVALID_HEADER (13)
#define BPE_INVALID_CODING_PARAMETERS (14)
#define BPE_PATTERNING_CODING_ERROR (15)
#define BPE_RICE_CODING_ERROR (16)
#define BPE_STAGE_CODING_ERROR (17)

#define   ENUM_NONE 0
#define   ENUM_TYPE_P 1
#define   ENUM_TRAN_B 2// Ds = 1 or 0.
#define   ENUM_TRAN_D 3 // i = 0, 1, 2.
#define   ENUM_TYPE_CI 4
#define   ENUM_TRAN_GI 5
#define   ENUM_TRAN_HI 6 // i = 0, 1, 2. j = 0, 1, 2, 3; four bits
#define   ENUM_TYPE_HIJ 7

#define BLOCK_SIZE 8
#define NEGATIVE_SIGN 1
#define POSITIVE_SIGN 0

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#define SIGN(var) ((var < 0) ? NEGATIVE_SIGN : POSITIVE_SIGN)


// restrictions on the coding parameters
#define IMAGE_WIDTH_MAX (1 << 20)
#define IMAGE_WIDTH_MIN 17
#define IMAGE_ROWS_MIN 17
#define SEGMENT_S_MIN 16
#define SEGMENT_S_MAX (1 << 20)
#define SEGBYTE_MAX (1 << 27)
#define DCDEPTH_MAX (1 << 5)
#define ACDEPTH_MAX (1 << 5)
#define MAX_SEGMENT_SIZE 134217728  // 2^27
//////////////////////////////////////////////////////////////////////////////

typedef struct BITSTREAM
{
   uint32_t SegBitCounter;
   uint32_t TotalBitCounter;
   uint32_t ByteBuffer_4Bytes;
   uint32_t CodeWordAlighmentBits;
   uint8_t CodeWord_Length;
   FILE *F_Bits;
} BitStream;

typedef struct SYMBOLDETAILS
{
   uint8_t sym_val : 4;
   uint8_t sym_len : 4;
   uint8_t sym_mapped_pattern : 4;
   uint8_t sign : 4;
   uint8_t type : 3;
}StrSymbolDetails;

typedef struct TYPEC
{
   uint8_t TypeC: 4;
}StrTypeC;

typedef struct TRANH
{
   uint8_t TranH: 4;
}StrTranH;

typedef struct TRANHI
{
   uint8_t TranH: 4;
}StrTranHI;

typedef struct TYPEHIJ
{
   StrTranHI TypeHij[4];
}StrTypeHij;

typedef struct PARENTREFINE
{
   uint8_t ParentRefSymbol : 4;
   uint8_t ParentSymbolLength: 2;
}ParentRefine;

typedef struct CHILDRENREF
{
   uint16_t ChildrenRefSymbol : 12;
   uint8_t ChildrenSymbolLength: 4;
}ChildrenRefine;

typedef struct GRANDCHILDREDREF
{
   uint16_t GrandChildrenRefSymbol : 16;
   uint8_t GrandChildrenSymbolLength: 5;
}GrandChildrenRefine;

typedef struct REFINEMENTBIT
{
   ParentRefine RefineParent;
   ChildrenRefine RefineChildren;
   GrandChildrenRefine RefineGrandChildren[3];
}StrRefine;

typedef struct PLANEHIT
{
   uint8_t TypeP : 3; // i = 0, 1, 2.
   uint8_t TranB : 1; // Ds = 1 or 0.
   uint8_t TranD : 3; // i = 0, 1, 2.
   StrTypeC TypeCi[3];
   uint8_t TranGi : 3;
   StrTranH TranHi[3];
   StrTypeHij TypeHij[3];
}PlaneHit;

typedef struct BLOCKBITSHIT
{
   // Block address
   long **PtrBlockAddress;
   float **PtrBlockAddressFloating;

   //Bit plane and block information.
   uint8_t BitPlane : 5;
   long BlockIndex : 20;
   // DC and AC_max information
   uint32_t MappedDC;
   uint32_t ShiftedDC;
   uint16_t DCRemainder;
   float DecodingDCRemainder;
   uint16_t BitMaxAC;
   uint16_t MappedAC;
   // status information ( hit history)
   //bit plane hit information.
   PlaneHit StrPlaneHitHistory;
   // code information.
   StrSymbolDetails SymbolsBlock[MAX_SYMBOLS_IN_BLOCK];
   // refinement bits
   StrRefine RefineBits;

} BitPlaneBits;

//#define DEFAULT_DWORD 100000;

typedef struct HEADER_STRUCTURE_PART1
  {
   BOOL StartImgFlag : 1;  // 1 bits
   BOOL EngImgFlg : 1; // 1 bit
   uint8_t SegmentCount_8Bits : 8; //8 bits Senment count/
   uint8_t BitDepthDC_5Bits : 5; // 5 bits
   uint8_t BitDepthAC_5Bits : 5; // 5 bits
   BOOL Reserved : 1; // 1 bits
   BOOL Part2Flag : 1; // 1 bit
   BOOL Part3Flag : 1; // 1 bit
   BOOL Part4Flag : 1; //1 bit
   uint8_t PadRows_3Bits : 3; // 3 bits replicate the last row so that new rows are integal of 8.
   uint8_t Reserved_5Bits : 5; // 5 bits: 00000
}HeaderPart1;

typedef struct HEADER_STRUCTURE_PART2
{
   uint32_t SegByteLimit_27Bits : 27; // 27 bits.
   BOOL DCstop : 1; //indicate whether the compressed output stops.
   uint8_t BitPlaneStop_5Bits : 5; // 5 bits
   uint8_t StageStop_2Bits : 2; // 2 bits, transform input data quantication.
   BOOL UseFill : 1;
   uint8_t Reserved_4Bits : 4; // 4 bits
}HeaderPart2;


typedef struct HEADER_STRUCTURE_PART3
{
   uint32_t S_20Bits : 20; //max number of blocks in a segment is limited to 2^20
   BOOL OptDCSelect : 1;
   BOOL OptACSelect : 1;
   uint8_t Reserved_2Bits : 2;
}HeaderPart3;

typedef struct HEADER_STRUCTURE_PART4
{
   BOOL DWTType : 1;
   uint8_t Reserved_2Bits : 2; //
   BOOL SignedPixels : 1;
   uint8_t PixelBitDepth_4Bits : 4;
   uint32_t ImageWidth_20Bits : 20; // maximum image width is limited 2^20
   BOOL TransposeImg : 1;
   uint8_t CodewordLength_2Bits : 2;
   BOOL Reserved : 1;
   BOOL CustomWtFlag : 1;
   uint8_t CustomWtHH1_2bits : 2;
   uint8_t CustomWtHL1_2bits : 2;
   uint8_t CustomWtLH1_2bits : 2;
   uint8_t CustomWtHH2_2bits : 2;
   uint8_t CustomWtHL2_2bits : 2;
   uint8_t CustomWtLH2_2bits : 2;
   uint8_t CustomWtHH3_2bits : 2;
   uint8_t CustomWtHL3_2bits : 2;
   uint8_t CustomWtLH3_2bits : 2;
   uint8_t CustomWtLL3_2bits : 2;
   uint16_t Reserved_11Bits : 11;
}HeaderPart4;

typedef struct HEADER
{
   HeaderPart1 Part1;
   HeaderPart2 Part2;
   HeaderPart3 Part3;
   HeaderPart4 Part4;
}StructHeader;

typedef union HEADERUNION {
   StructHeader Header;
   long Field[5];
}HeaderStruct;


typedef struct STR_STOPLOCATION {
   char BitPlaneStopDecoding; // for find adjustment.
   long BlockNoStopDecoding;
   short TotalBitsReadThisTime;
   BOOL LocationFind;
   char X_LocationStopDecoding;
   char Y_LocationStopDecoding;
   unsigned char stoppedstage;
}StrStopLocation;


typedef struct CODINGPARAMETERS
{
   BitStream *Bits;
   uint8_t BitPlane;
   float BitsPerPixel;      // default coding BitsPerPixel, bits per pixels
   uint32_t DecodingAllowedBitsSizeInSegment;      // for decoding purpose.
   BOOL RateReached;
   StrStopLocation DecodingStopLocations;
   uint8_t QuantizationFactorQ:6;
   uint32_t BlockCounter;
   uint32_t block_index;
   uint8_t N;
   BOOL SegmentFull;
   HeaderStruct * PtrHeader;
   uint32_t ImageRows;
   uint32_t ImageWidth;
   uint8_t PadCols_3Bits;
   uint8_t PixelByteOrder:1; // default byte order. 1 means LSB first.
   char InputFile[MAX_FILE_NAME_SIZE];
   char CodingOutputFile[MAX_FILE_NAME_SIZE];
   BOOL altStrageStop; // Alternative stage stop
} StructCodingPara;

typedef struct BLOCKSTRING
{
   long **FreqBlkString;
   float ** FloatingFreqBlk;
   uint32_t Blocks;
   struct BLOCKSTRING *next;
   struct BLOCKSTRING *previous;
}StructFreBlockString;



void ErrorMsg(int err);
void BitsOutput(StructCodingPara *,    uint32_t ,  int );
short BitsRead(StructCodingPara *,   uint32_t *,  short );

//FILE *F_CodingInfo;
#endif