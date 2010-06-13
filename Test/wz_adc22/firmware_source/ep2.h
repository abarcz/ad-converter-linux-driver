/*-------------------------------------------------------------------------
  ep2.h - USB endpoint 2 callbacks

             (c) 2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

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

/* $Id: ep2.h,v 1.2 2006/03/05 19:39:31 gaufille Exp $ */

#ifndef EP2_H_
#define EP2_H_

/* EP1 buffer size */
#define EP2_BUFFER_SIZE 64

void ep2_init(void);
void ep2_in(void);
void prepare_ep2_in(void);

#endif /*EP2_H_*/
