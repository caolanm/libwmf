#ifndef LIBWMF_REGION_H
#define LIBWMF_REGION_H

static void REGION_SubtractRegion(WINEREGION *regD, WINEREGION *regM, WINEREGION *regS );

static void REGION_SubtractO (WINEREGION *pReg, RECT *r1, RECT *r1End, RECT *r2, RECT *r2End, U16 top, U16 bottom);

static void REGION_CopyRegion(WINEREGION *dst, WINEREGION *src);

static S16 REGION_Coalesce (
	WINEREGION *pReg, /* Region to coalesce */
	S16  prevStart,  /* Index of start of previous band */
	S16 curStart    /* Index of start of current band */
	);

static void REGION_RegionOp(
	WINEREGION *newReg, /* Place to store result */
	WINEREGION *reg1,   /* First region in operation */
	WINEREGION *reg2,   /* 2nd region in operation */
	void (*overlapFunc)(),     /* Function to call for over-lapping bands */
	void (*nonOverlap1Func)(), /* Function to call for non-overlapping bands in region 1 */
	void (*nonOverlap2Func)()  /* Function to call for non-overlapping bands in region 2 */
	);

static void REGION_UnionO (WINEREGION *pReg, RECT *r1, RECT *r1End, RECT *r2, RECT *r2End, S16 top, S16 bottom);
static void REGION_UnionNonO (WINEREGION *pReg, RECT *r, RECT *rEnd, S16 top, S16 bottom);
static void REGION_UnionNonO (WINEREGION *pReg, RECT *r, RECT *rEnd, S16 top, S16 bottom);
static void REGION_UnionRegion(WINEREGION *newReg, WINEREGION *reg1, WINEREGION *reg2);
static void REGION_SubtractNonO1 (WINEREGION *pReg, RECT *r, RECT *rEnd, U16 top, U16 bottom);
static void REGION_SetExtents (WINEREGION *pReg);
static void REGION_IntersectRegion(WINEREGION *newReg, WINEREGION *reg1, WINEREGION *reg2);
static void REGION_IntersectO(WINEREGION *pReg,  RECT *r1, RECT *r1End, RECT *r2, RECT *r2End, S16 top, S16 bottom);

#endif /* ! LIBWMF_REGION_H */
