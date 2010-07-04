/*-------------------------------------------------------------------------
  ep0.c - USB endpoint 0 callbacks

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

/* $Id: ep0.h,v 1.2 2006/03/05 19:39:31 gaufille Exp $ */

#ifndef EP0_H_
#define EP0_H_

/* EP0 buffer size */
#define EP0_BUFFER_SIZE 8

#include "usb_std_req.h"

void ep0_init(void);
void ep0_in(void);
void ep0_setup(void);

#endif /*EP0_H_*/
