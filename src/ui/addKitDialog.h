/***************************************************************************
 *            addKitDialog.h
 *
 *  Copyright  2007  Florent Berthaut
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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <iostream>
#include <string>


#ifndef _ADDKITDIALOG_H
#define _ADDKITDIALOG_H

class addKitDialog : public Fl_Window
{
	public:
			addKitDialog(int ,int ,const char*);
			~addKitDialog();

  			static void statOkBut(Fl_Widget* w,void* f){addKitDialog *tmpf = static_cast<addKitDialog *>(f);tmpf->cbOkBut(w);}
			void cbOkBut(Fl_Widget*);
  			static void statCancelBut(Fl_Widget* w,void* f){addKitDialog *tmpf = static_cast<addKitDialog*>(f);tmpf->cbCancelBut(w);}
			void cbCancelBut(Fl_Widget*);
  			static void statChoice(Fl_Widget* w,void* f){addKitDialog *tmpf = static_cast<addKitDialog *>(f);tmpf->cbChoice(w);}
			void cbChoice(Fl_Widget*);

			int getValue();
			void addKit(const char*);

	private:
			Fl_Choice* addKitChoice;
			Fl_Return_Button* addKitOk;
			Fl_Button *addKitCancel;
			int value;
};

#endif
