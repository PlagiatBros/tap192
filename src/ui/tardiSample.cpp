/***************************************************************************
 *            tardiSample.cpp
 *
 *  Copyright  2008  Florent Berthaut
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

#include "tardiSample.h"

using namespace std;

tardiSample::tardiSample(int x, int y, int w, int h, const char* n) : Fl_Widget(x,y,w,h,n),nbChannels(1),zoom(1),buffers(NULL),e1X(0),e2X(0),e3X(0),e4X(0),e1Y(1.0),e2Y(1.0),e3Y(1.0),e4Y(1.0),pointSize(5)
{
/*
		e1X = 5000;
		e2X = 10000;
		e3X = 15000;
		e4X = 30000;
		
		e1Y=0;
		e2Y=0.8;
		e3Y=1.0;
		e4Y=0.2;
*/
}	
				
				
tardiSample::~tardiSample()
{
	
}

void tardiSample::setBuffers(float** b)
{
	buffers=b;
}

void tardiSample::setNbChannels(int n)
{
	nbChannels=n;
}

void tardiSample::setFramesCount(long fc)
{
	framesCount=fc;

	//for now
	e3X=framesCount;
	e4X=framesCount;
}
				
void tardiSample::draw()
{
	fl_push_clip(this->x(),this->y(),this->w(),this->h());

	fl_draw_box(FL_DOWN_BOX,this->x(),this->y(),this->w(),this->h(),FL_BACKGROUND_COLOR);

	if(buffers!=NULL)//if there is a sample
	{
		//draw the sample
		int largeur =(int)((float)(this->w())*zoom);
		step=framesCount/largeur;	
		float vertStep = ((float)this->h())/(((float)nbChannels)*2.0);
		double max=0;
		//first we check the max value to scale the drawing
		for(int i=0;i<largeur;i++)
		{
			for(int j=0;j<nbChannels;++j)
			{
				if(max<fabs(buffers[j][i*step]))
					max=fabs(buffers[j][i*step]); 
			}
		}
		double scale=1.0/max;	
		//then we draw the sample
		for(int i=0;i<largeur-1;i++)
		{
			if( i>=+e1X/step && i<=e4X/step)
				fl_color(Fl::get_color(FL_FOREGROUND_COLOR));
			else
				fl_color(Fl::get_color(FL_INACTIVE_COLOR));

			for(int j=0;j<nbChannels;++j)
			{
				float vertOffset = (float)this->h()/(float)nbChannels*(float)j + vertStep;
				int pos1 = (int)(((float)(buffers[j][i*step])*scale)*vertStep + vertOffset); 
				int pos2 = (int)(((float)(buffers[j][(i+1)*step])*scale)*vertStep + vertOffset); 
				fl_line(this->x()+i,this->y()+pos1,this->x()+i+1,this->y()+pos2);	
			}
		}


/*
		//draw the points
		fl_color(FL_SELECTION_COLOR);

		//enveloppe1
		fl_rectf(this->x()+e1X/step,this->y()+this->h()-(int)(e1Y*(float)(this->h())),pointSize,pointSize);
		
		fl_line(this->x()+e1X/step,this->y()+this->h()-(int)(e1Y*(float)(this->h())),this->x()+e2X/step,this->y()+this->h()-(int)(e2Y*(float)(this->h())));
		
		//enveloppe2
		fl_rectf(this->x()+e2X/step,this->y()+this->h()-(int)(e2Y*(float)(this->h())),pointSize,pointSize);

		fl_line(this->x()+e2X/step,this->y()+this->h()-(int)(e2Y*(float)(this->h())),this->x()+e3X/step,this->y()+this->h()-(int)(e3Y*(float)(this->h())));
		
		//enveloppe3
		fl_rectf(this->x()+e3X/step,this->y()+this->h()-(int)(e3Y*(float)(this->h())),pointSize,pointSize);

		fl_line(this->x()+e3X/step,this->y()+this->h()-(int)(e3Y*(float)(this->h())),this->x()+e4X/step,this->y()+this->h()-(int)(e4Y*(float)(this->h())));
		
		//enveloppe4
		fl_rectf(this->x()+e4X/step,this->y()+this->h()-(int)(e4Y*(float)(this->h())),pointSize,pointSize);
		
*/
	}

	fl_pop_clip();
	
}


int tardiSample::handle(int event)
{
	switch(event)
	{
	/*
		case FL_PUSH : 
			{
				if(Fl::event_button()==FL_LEFT_MOUSE)
				{
					handlePush(Fl::event_x(),Fl::event_y());
					do_callback();	
					return 1;	
				}
				else
					return Fl_Widget::handle(event);
			}
			break;
		case FL_DRAG : 
			{
				if(Fl::event_button()==FL_LEFT_MOUSE)
				{
					handleDrag(Fl::event_x(),Fl::event_y());
					do_callback();	
					return 1;	
				}
				else
					return Fl_Widget::handle(event);
			}
			break;
		case FL_RELEASE : 
			{
				if(Fl::event_button()==FL_LEFT_MOUSE)
				{
					handleRelease(Fl::event_x(),Fl::event_y());
					do_callback();	
					return 1;	
				}
				else
					return Fl_Widget::handle(event);
			}
			break;
		*/
		default : return Fl_Widget::handle(event);
	}
}

void tardiSample::handlePush(int mx,int my)
{
	selectedWidget=-1;

	mx-=this->x();
	my= this->h()-(my-this->y());

	//check the enveloppe points
	if(mx>(e1X/step-pointSize) && mx<(e1X/step+pointSize*2) && my>(e1Y*this->h()-pointSize*2) && my<(e1Y*this->h()+pointSize))
		selectedWidget=0;
	else
	if(mx>(e2X/step-pointSize) && mx<(e2X/step+pointSize*2) && my>(e2Y*this->h()-pointSize*2) && my<(e2Y*this->h()+pointSize))
		selectedWidget=1;
	else
	if(mx>(e3X/step-pointSize) && mx<(e3X/step+pointSize*2) && my>(e3Y*this->h()-pointSize*2) && my<(e3Y*this->h()+pointSize))
		selectedWidget=2;
	else
	if(mx>(e4X/step-pointSize) && mx<(e4X/step+pointSize*2) && my>(e4Y*this->h()-pointSize*2) && my<(e4Y*this->h()+pointSize))
		selectedWidget=3;
	//else check the zoom cursor
	else 
	{
	}

}

void tardiSample::handleDrag(int mx,int my)
{

	mx-=this->x();
	my= this->h()-(my-this->y());

	long newX = mx*step;
	if(newX<0)
		newX=0;
	if(newX>framesCount-1)
		newX=framesCount-1;
	float newY = (float)my/(float)(this->h());
	if(newY<0)
		newY=0;
	if(newY>1)
		newY=1;
	
	switch(selectedWidget)
	{
		case 0:
			{
				if(newX<=e2X)
				{
					e1X = newX;
					e1Y = newY;
				}
			}
			break;
		case 1:
			{
				if(newX>=e1X && newX<=e3X)
				{
					e2X = newX;
					e2Y = newY;
				}
			}
			break;
		case 2:
			{
				if(newX>=e2X && newX<=e4X)
				{
					e3X = newX;
					e3Y = newY;
				}
			}
			break;
		case 3:
			{
				if(newX>=e3X)
				{
					e4X = newX;
					e4Y = newY;
				}
			}
			break;
		case 4:
			{
			}
			break;
		default:
			break;
	}

	this->redraw();
}

void tardiSample::handleRelease(int mx,int my)
{

}
