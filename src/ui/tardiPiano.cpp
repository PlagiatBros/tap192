/***************************************************************************
 *            tardiPiano.cpp
 *
 *  Sun Sep 10 19:42:18 2006
 *  Copyright  2006 - 2013 Florent Berthaut, 2019 Jean-Emmanuel Doucet & Aurélien Roux
 *  florentberthaut@no-log.org jean-emmanuel.doucet@groolot.net orl@ammd.net
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

#include "tardiPiano.h"

using namespace std;

tardiPiano::tardiPiano(int x, int y, int w, int h, const char* n) : Fl_Widget(x,y,w,h,n),lSelect(60),rSelect(60),mSelect(60)
{

	float whiteW = 1.0/75.0;
	float blackW = whiteW/2.0;

	for(int i=0;i<10;++i) //for each octave
	{
		float pos=0;
		for(int j=0;j<5;j+=2,++pos) //first part of the white keys
		{
			touches[12*i+j][0] =  (float)(7*i)*whiteW + pos*whiteW;
			touches[12*i+j][1] = 0;
		}

		for(int j=5;j<12;j+=2,++pos) //second part of the white keys
		{
			touches[12*i+j][0] = (float)(7*i)*whiteW + pos*whiteW;
			touches[12*i+j][1] = 0;
		}

		pos=1;
		for(int j=1;j<4;j+=2,++pos) //first part of the black keys
		{
			touches[12*i+j][0] = (float)(7*i)*whiteW +pos*whiteW - blackW/2.0;
			touches[12*i+j][1] = 1;

		}
		++pos;
		for(int j=6;j<11;j+=2,++pos) // second part of the black keys
		{
			touches[12*i+j][0] = (float)(7*i)*whiteW + pos*whiteW - blackW/2.0;
			touches[12*i+j][1] = 1;
		}
	}


	//remaining keys
	int i=10;
	int pos=0;
	for(int j=0;j<5;j+=2,++pos) //white keys
	{
		touches[12*i+j][0] = (7*i)*whiteW + pos*whiteW;
		touches[12*i+j][1] = 0;
	}
	pos=1;
	for(int j=1;j<4;j+=2,++pos) //black keys
	{
		touches[12*i+j][0] = (7*i)*whiteW +pos*whiteW - blackW/2;
		touches[12*i+j][1] = 1;

	}

	//last white keys
	pos=3;
	touches[12*i+5][0] = (7*i)*whiteW + pos*whiteW;
	touches[12*i+5][1] = 0;
	++pos;
	touches[12*i+7][0] = (7*i)*whiteW + pos*whiteW;
	touches[12*i+7][1] = 0;


	//last black key
	touches[12*i+6][0] = (7*i)*whiteW +pos*whiteW - blackW/2;
	touches[12*i+6][1] = 1;

}


tardiPiano::~tardiPiano()
{

}

void tardiPiano::draw()
{
	//draw the white keys
	float noteH=this->h();
	float noteW=this->w()/75.0;
	for(int i=0;i<128;++i)
	{
		switch((int)(touches[i][1]))
		{
			case 0:
				{
					fl_draw_box(FL_THIN_UP_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,COLOR_WHITE);
				}
				break;
			case 2:
				{
					fl_draw_box(FL_THIN_UP_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,FL_INACTIVE_COLOR);
				}
				break;
			case 4:
				{
					fl_draw_box(FL_THIN_UP_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,COLOR_ACCENT);
				}
				break;
			default:break;
		}

/*
		if(touches[i][1]==0.0 || touches[i][1]==2.0 || touches[i][1]==4.0)
		{
			if(touches[i][1]==0.0)

				fl_draw_box(FL_THIN_UP_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,FL_BACKGROUND_COLOR);
			else
				fl_draw_box(FL_THIN_UP_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,FL_INACTIVE_COLOR);

		}
*/
	}

	//draw the black keys
	noteH=(float)(this->h())/2.0;
	noteW=this->w()/(75.0*2.0);
	for(int i=0;i<128;++i)
	{
		switch((int)(touches[i][1]))
		{
			case 1:
				{
					fl_draw_box(FL_FLAT_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,COLOR_BLACK);
				}
				break;
			case 3:
				{
					fl_draw_box(FL_FLAT_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,COLOR_KEYSELECT);
				}
				break;
			case 5:
				{
					fl_draw_box(FL_FLAT_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,COLOR_ACCENT);
				}
				break;
			default:break;
		}
		/*
		if(touches[i][1]==1.0 || touches[i][1]==3.0 || touches[i][1]==5.0)
		{
			if(touches[i][1]==1.0)
				fl_draw_box(FL_BORDER_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,FL_SELECTION_COLOR);
			else
				fl_draw_box(FL_BORDER_BOX,this->x()+(int)((float)this->w()*touches[i][0]),this->y(),(int)noteW,(int)noteH,FL_FOREGROUND_COLOR);

		}
		*/
	}

}


int tardiPiano::handle(int event)
{

	int posX = Fl::event_x();
	int posY = Fl::event_y();

	int note=60;
	note = trouve_note(posX,posY);

	switch(event)
	{
		//si clique : on recupere la pos de la souris
		case FL_PUSH :
		{
			switch(Fl::event_button())
			{
				case FL_LEFT_MOUSE : //if left clic, begin range
					{
						mouse=FL_LEFT_MOUSE;
						if(note<=rSelect)
							lSelect=note;
						else
							lSelect=rSelect;
						maj();
						do_callback();
						return 1;
					}break;
				case FL_RIGHT_MOUSE : //if right clic, end range
					{
						mouse=FL_RIGHT_MOUSE;
						if(note>=lSelect)
							rSelect=note;
						else
							rSelect=lSelect;
						maj();
						do_callback();
						return 1;
					}break;
				case FL_MIDDLE_MOUSE : //if middle clic
					{
						mouse=FL_MIDDLE_MOUSE;
						mSelect=note;
						maj();
						do_callback();
						return 1;
					}break;
				default:
      					return Fl_Widget::handle(event);
			};
		}break;
		case FL_DRAG:
			{
				switch(mouse)
				{
					case FL_LEFT_MOUSE:
						{
							if(note<=rSelect)
								lSelect=note;
							else
								lSelect=rSelect;
							maj();
							do_callback();
							return 1;
						}
						break;
					case FL_RIGHT_MOUSE:
						{
							if(note>=lSelect)
								rSelect=note;
							else
								rSelect=lSelect;
							maj();
							do_callback();
							return 1;
						}
						break;
					case FL_MIDDLE_MOUSE:
						{
							mSelect=note;
							maj();
							do_callback();
							return 1;
						}
						break;
					default:
						return 1;
						break;
				}
			}
			break;
		case FL_RELEASE:
			{
				mouse=-1;
				return 1;
			}
			break;
		default:
      			return Fl_Widget::handle(event);
	};
}


int tardiPiano::trouve_note(int posX,int posY)
{
	int res=0;

	posX-=this->x();
	posY-=this->y();

	if(posY>this->h()/2)//si on est apres la moitie en hauteur
	{	//alors on ne cherche que dans les blanches
		for(int i=0;i<128;++i)
			if(fmod(touches[i][1],2)==0)
				if(posX - (int)(touches[i][0]*(float)(this->w()))<=(int)((float)(this->w())/75.0) &&  (posX - (int)(touches[i][0]*(float)(this->w())))>=0)
				{
					res=i;
					break;
				}
	}
	else			//sinon on cherche deja dans les noires
	{
		for(int i=0;i<128;++i)
			if(fmod(touches[i][1],2)!=0)
				if(posX - (int)(touches[i][0]*(float)(this->w()))<=(int)((float)(this->w())/(75.0*2)) &&  (posX - (int)(touches[i][0]*(float)(this->w())))>=0)
				{
					res=i;
					break;
				}

		if(res==0)	//on ne l'a pas trouve dans les noires
		{
			for(int i=0;i<128;++i)
				if(touches[i][1]==0 || touches[i][1]==2)
				if(posX - (int)(touches[i][0]*(float)(this->w()))<=(int)((float)(this->w())/75.0) &&  (posX - (int)(touches[i][0]*(float)(this->w())))>=0)
					{
						res=i;
						break;
					}
		}
	}
	return res;
}


int tardiPiano::getLSelect()
{
	return lSelect;
}

int tardiPiano::getRSelect()
{
	return rSelect;
}

int tardiPiano::getMSelect()
{
	return mSelect;
}

void tardiPiano::setLSelect(int l)
{
	lSelect = l;
	maj();
}

void tardiPiano::setRSelect(int r)
{
	 rSelect = r;
	maj();
}

void tardiPiano::setMSelect(int m)
{
	 mSelect = m;
	maj();
}

void tardiPiano::maj()
{
	for(int i=0;i<128;++i)
	{
		if(touches[i][1]==2 || touches[i][1]==4)
			touches[i][1]=0;
		if(touches[i][1]==3 || touches[i][1]==5)
			touches[i][1]=1;
	}
	for(int i=lSelect;i<=rSelect;++i)
		if(touches[i][1]==0)
			touches[i][1]=2;
		else
			touches[i][1]=3;

	if(fmod(touches[mSelect][1],2)==0)
		touches[mSelect][1]=4;
	else
		touches[mSelect][1]=5;

	this->redraw();
}
