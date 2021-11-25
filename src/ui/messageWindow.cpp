/***************************************************************************
 *            messageWindow.cpp
 *
 *  Copyright  2007 - 2013 Florent Berthaut, 2019 Jean-Emmanuel Doucet & Aurélien Roux
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

#include "messageWindow.h"
#include <iostream>

using namespace std;


messageWindow::messageWindow(int w,int h,const char* titre):Fl_Window(w,h,titre)
{
	//better look
	Fl::scheme("gtk+");

	messages = new Fl_Text_Display(0,0,w,h,"Messages");
	buf = new Fl_Text_Buffer();
	messages->buffer(buf);
	messages->insert_position(0);


}

messageWindow::~messageWindow()
{

}

void messageWindow::addMessage(std::string m)
{
	std::string mess = m+"\n";
	messages->insert(mess.c_str());
	messages->show_insert_position();
	messages->redraw();
	Fl::check();
}

void messageWindow::clear()
{
	buf->remove(0,buf->length());
	messages->redraw();
	Fl::check();
}
