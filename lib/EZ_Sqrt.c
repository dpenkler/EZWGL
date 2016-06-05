/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 * *             Copyright (C) 1996, 1997, 1998  Maorong Zou
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **********************************************************************/
/*
 *  June 1996.  Beta Release.
 *  Sept 1996.  Release Version 1.0
 *  Dec 1996.  Release Version 1.1 Beta
 *  April 1997.  Release Version 1.2
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  December 1999.  Release Version 1.50
 */
/*
 * A High Speed, Low Precision Square Root
 * by Paul Lalonde and Robert Dawson
 * from "Graphics Gems", Academic Press, 1990
 * Modified by Javier Arevalo to avoid some unnecessary shifts.
 * Seems like this adds some precision too, but I'm unsure.
 */

/*
 * SPARC implementation of a fast square root by table
 * lookup.
 * SPARC floating point format is as follows:
 *
 * BIT 31    30  23      22  0
 *     sign  exponent    mantissa
 */

#ifdef USE_LOW_PRECISION_SQRT

/*************************************************************************/
void  EZ_BuildSqrtTable    MY_ANSIARGS((void));
float EZ_LowProcisionSqrt  MY_ANSIARGS((float f));
/*************************************************************************/

#include <math.h>

static long sqrttab[0x100];        /* declare table of square roots */
void EZ_BuildSqrtTable()
{
  unsigned short i;
  float f;
  unsigned int *fi = (unsigned*)&f;   /* To access the bits of a float in 
                                       * C quickly we must misuse pointers */
  for(i = 0; i <= 0x7f; i++) 
    {
      *fi = 0;
      /* Build a float with the bit pattern i as mantissa
       * and an exponent of 0, stored as 127
       */
      *fi = (i << 16) | (127 << 23);
      f = sqrt(f);

      /* Take the square root then strip the first 7 bits of
       * the mantissa into the table
       */
      sqrttab[i] = (*fi & 0x7fffff);

      /* Repeat the process, this time with an exponent of 1,
       * stored as 128
       */
      *fi = 0;
      *fi = (i << 16) | (128 << 23);
      f = sqrt(f);
      sqrttab[i+0x80] = (*fi & 0x7fffff);
    }
}

/* fsqrt - fast square root by table lookup, original C version */
float EZ_LowProcisionSqrt(n) 
     float n;
{
  unsigned int *num = (unsigned *) & n; /* to access the bits of a float in C
					 * we must misuse pointers */
  short e;        /* the exponent */
  if(n == 0) return (0); /* check for square root of 0 */
  e = (*num >> 23) - 127; /* get the exponent - on a SPARC the 
			   * exponent is stored with 127 added */
  *num &= 0x7fffff;   /* leave only the mantissa */
  if (e & 0x01) *num |= 0x800000;
  /* the exponent is odd so we have to */
  /* look it up in the second half of  */
  /* the lookup table, so we set the high bit */

  e >>= 1;    /* divide the exponent by two */

  /* note that in C the shift */
  /* operators are sign preserving */
  /* for signed operands */

  /* Do the table lookup, based on the quaternary mantissa,
   * then reconstruct the result back into a float
   */
  *num = ((sqrttab[*num >> 16])) + ((e + 127) << 23);
  return(n);
}

#endif
