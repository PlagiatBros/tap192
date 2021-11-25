/***************************************************************************
 *            addKitDialog.cpp
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

#include "addKitDialog.h"
#include <iostream>

using namespace std;


addKitDialog::addKitDialog(int w,int h,const char* titre):Fl_Window(w,h,titre)
{
	//better look
	Fl::scheme("gtk+");

	addKitChoice= new Fl_Choice(50,20,150,30,"");
	addKitChoice->callback(statChoice,this);
	addKitOk = new Fl_Return_Button(40,65,70,30,"Ok");
	addKitOk->callback(statOkBut,this);
	addKitCancel = new Fl_Button(135,65,70,30,"Cancel");
	addKitCancel->callback(statCancelBut,this);

	value=0;

}

addKitDialog::~addKitDialog()
{
	delete addKitChoice;
	delete addKitOk;
	delete addKitCancel;
}


void addKitDialog::cbOkBut(Fl_Widget*)
{
	this->hide();
}

void addKitDialog::cbCancelBut(Fl_Widget*)
{
	value=-1;
	this->hide();
}

void addKitDialog::cbChoice(Fl_Widget*)
{
	value = addKitChoice->value();
}

int addKitDialog::getValue()
{
	return value;
}

void addKitDialog::addKit(const char* ch)
{
	addKitChoice->add(ch);
}
