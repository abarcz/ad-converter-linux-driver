/*-------------------------------------------------------------------------
  common_types.h - Common declarations and types

             (c) 2005 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
-------------------------------------------------------------------------*/

/* $Id: common_types.h,v 1.2 2006/03/05 19:39:31 gaufille Exp $ */

#ifndef VASCO_H_
#define VASCO_H_

/* 8 bits */
typedef unsigned char uchar;
typedef signed char schar;
typedef union
{
    uchar uc;
    schar sc;
    struct
    {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
    };
} Char;

/* 16 bits */
typedef unsigned int uint;
typedef signed int sint;
typedef union
{
    uint ui;
    sint si;
    Char c[2];
} Int;


/* 32 bits */
typedef unsigned long ulong;
typedef signed long slong;
typedef union
{
    ulong ul;
    slong sl;
    Int   i[2];
    Char  c[4];
} Long;

#define TRUE 1
#define FALSE 0
#define NULL 0

#endif /*VASCO_H_*/
