/***************************************************************************
 *            tardiSample.h
 *
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
 
#ifndef _TARDI_SAMPLE_H
#define _TARDI_SAMPLE_H

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <math.h>
#include <iostream>


using namespace std;

class tardiSample : public Fl_Widget
{
	public 	:
			tardiSample(int , int , int , int , const char*);
			~tardiSample();	
			void draw();
			int handle(int);
			void setBuffers(float**);
			void setNbChannels(int);
			void setFramesCount(long);
			void handlePush(int,int);
			void handleDrag(int,int);
			void handleRelease(int,int);
	private : 		
			int nbChannels;
			float zoom;	
			int step;
			float** buffers;
			long e1X,e2X,e3X,e4X;
			float e1Y,e2Y,e3Y,e4Y;
			long framesCount;	
			int pointSize;

			int selectedWidget;
			int prevMX,prevMY;
};



#endif /* _TARDI_SAMPLE_H */
