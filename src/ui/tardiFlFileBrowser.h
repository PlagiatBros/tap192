/***************************************************************************
 *            tardiFlFileBrowser.h
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
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/filename.H>
#include <FL/fl_ask.H>
#include <iostream>
#include <cstring>
#include <vector>
#include <cstdlib>

#ifndef _TARDI_FL_FILE_BROWSER_H
#define _TARDI_FL_FILE_BROWSER_H

class tardiFlFileBrowser : public Fl_Window
{
	public:
			tardiFlFileBrowser();
			~tardiFlFileBrowser();

  			static void statList(Fl_Widget* w,void* f){tardiFlFileBrowser *tmpf = static_cast<tardiFlFileBrowser *>(f);tmpf->cbList(w);}
			void cbList(Fl_Widget*);
  			static void statPrevButton(Fl_Widget* w,void* f){tardiFlFileBrowser *tmpf = static_cast<tardiFlFileBrowser *>(f);tmpf->cbPrevButton(w);}
			void cbPrevButton(Fl_Widget*);
  			static void statNextButton(Fl_Widget* w,void* f){tardiFlFileBrowser *tmpf = static_cast<tardiFlFileBrowser *>(f);tmpf->cbNextButton(w);}
			void cbNextButton(Fl_Widget*);
  			static void statPathButton(Fl_Widget* w,void* f){tardiFlFileBrowser *tmpf = static_cast<tardiFlFileBrowser *>(f);tmpf->cbPathButton(w);}
			void cbPathButton(Fl_Widget*);
  			static void statFileName(Fl_Widget* w,void* f){tardiFlFileBrowser *tmpf = static_cast<tardiFlFileBrowser *>(f);tmpf->cbFileName(w);}
			void cbFileName(Fl_Widget*);
  			static void statOk(Fl_Widget* w,void* f){tardiFlFileBrowser *tmpf = static_cast<tardiFlFileBrowser *>(f);tmpf->cbOk(w);}
			void cbOk(Fl_Widget*);
  			static void statCancel(Fl_Widget* w,void* f){tardiFlFileBrowser *tmpf = static_cast<tardiFlFileBrowser *>(f);tmpf->cbCancel(w);}
			void cbCancel(Fl_Widget*);
  			static void statHidden(Fl_Widget* w,void* f){tardiFlFileBrowser *tmpf = static_cast<tardiFlFileBrowser *>(f);tmpf->cbHidden(w);}
			void cbHidden(Fl_Widget*);
  			static void statAdd(Fl_Widget* w,void* f){tardiFlFileBrowser *tmpf = static_cast<tardiFlFileBrowser *>(f);tmpf->cbAdd(w);}
			void cbAdd(Fl_Widget*);

			void open(std::string,std::string,std::string);
			int displayCurrentDir();
			void storeCurrentPath();
			void displayCurrentPath(int);

			char* getResult();

	private:
			Fl_Button *prevButton;
			Fl_Button *nextButton;

			Fl_Group *pathButtonsGroup;
			std::vector<Fl_Toggle_Button*> pathButtons;

  			Fl_Hold_Browser *filesList;
			Fl_Button *addButton;
			Fl_Toggle_Button *hiddenButton;

			Fl_Input *fileName;

			Fl_Return_Button *okButton;
			Fl_Button *cancelButton;


			std::string currentDir;
			bool showHidden;
			std::string pattern;
			std::string currentPattern;

			char* result;
};

#endif
