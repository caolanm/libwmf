#include "wmfapi.h"

U32 read_32ubit(FILE *in)
	{
	U16 temp1,temp2;
	U32 ret;
	temp1 = read_16ubit(in);
	temp2 = read_16ubit(in); 
	ret = temp2;
	ret = ret << 16;
	ret += temp1;
	return(ret);
	}

U16 read_16ubit(FILE *in)
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
