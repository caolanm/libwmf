#include "wmfapi.h"

U32 wmfReadU32bit(FILE *in)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = wmfReadU16bit(in);
	temp2 = wmfReadU16bit(in); 
	ret = temp2;
	ret = ret << 16;
	ret += temp1;
	return(ret);
	}

U16 wmfReadU16bit(FILE *in)
	{
	U8 temp1,temp2;
	U16 ret;
	temp1 = getc(in);
	temp2 = getc(in);
	ret = temp2;
	ret = ret << 8;
	ret += temp1;
	return(ret);
	}
