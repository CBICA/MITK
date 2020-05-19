/*============================================================================

 Copyright (c) German Cancer Research Center (DKFZ)
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 - Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

 - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 - All advertising materials mentioning features or use of this software must
   display the following acknowledgement:

     "This product includes software developed by the German Cancer Research
      Center (DKFZ)."

 - Neither the name of the German Cancer Research Center (DKFZ) nor the names
   of its contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE GERMAN CANCER RESEARCH CENTER (DKFZ) AND
   CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
   BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE GERMAN
   CANCER RESEARCH CENTER (DKFZ) OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
   DAMAGE.

============================================================================*/

/** @file
 *  calculates the size of the TSV's value in bytes
 */

#ifndef lint
  static char *what = { "@(#)_mitkIpPicTSVSize\t\tGerman Cancer Research Center (DKFZ)\t"__DATE__"\t$Revision$" };
#endif

#include "mitkIpPic.h"

 /**  calculates the size of the TSV's value in bytes
 *  @param tsv	the tsv
 *
 *  @return the size of the tsv in bytes
 *
 *
 * AUTHOR & DATE
 *  Andre Schroeter	23.01.95
 *
 * UPDATES
 *  none
 */


mitkIpUInt4_t _mitkIpPicTSVSize( mitkIpPicTSV_t *tsv )
{
  mitkIpUInt4_t i;
  mitkIpUInt4_t elements;

  if( tsv->dim == 0 )
    return( 0 );

  elements = tsv->n[0];
  for( i = 1; i < tsv->dim; i++ )
    elements *= tsv->n[i];

  return( elements * tsv->bpe / 8 );
}
