/***************************************************************************
 *            tardiPiano.h
 *
 *  Sun Sep 10 19:41:41 2006
 *  Copyright  2006  Florent Berthaut
 *  florentberthaut@no-log.org
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _TARDI_PIANO_H
#define _TARDI_PIANO_H

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <iostream>
#include <math.h>
#include "flatWidgets.h"

using namespace std;

class tardiPiano : public Fl_Widget
{
	public 	:
			tardiPiano(int , int , int , int , const char*);
			~tardiPiano();
			void draw();
			int handle(int);
			int trouve_note(int,int);
			int getLSelect();
			int getRSelect();
			int getMSelect();
			void maj();
			void setLSelect(int);
			void setRSelect(int);
			void setMSelect(int);

	private :
		 	float touches[128][2];//0:posX,1:type
			int lSelect,rSelect,mSelect;
			int mouse;
};



#endif /* _TAP_PIANO_H */
