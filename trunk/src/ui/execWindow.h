/***************************************************************************
 *            execWindow.h
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
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_ask.H>
#include <iostream>
#include <string>

#include "addKitDialog.h"
#include "messageWindow.h"
#include "tardiSample.h"
#include "tardiPiano.h"
#include "tardiFlFileBrowser.h"
#include "../base/setup.h"
#include "../base/kit.h"
#include "../base/instrument.h"
#include "../base/variation.h"
 
#ifndef _EXECWINDOW_H
#define _EXECWINDOW_H

class tapeutape;

class execWindow : public Fl_Window 
{
  public:
			execWindow(int ,int ,const char*,tapeutape* );
			~execWindow();

			
  			static void statWindow(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbWindow(w);}	
			void cbWindow(Fl_Widget*);

			//menu callback
  			static void statOpen(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbOpen(w);}	
			void cbOpen(Fl_Widget*);
  			static void statSave(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbSave(w);}	
			void cbSave(Fl_Widget*);
  			static void statSaveAs(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbSaveAs(w);}	
			void cbSaveAs(Fl_Widget*);
  			static void statQuit(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbQuit(w);}	
			void cbQuit(Fl_Widget*);
  			static void statMessages(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbMessages(w);}	
			void cbMessages(Fl_Widget*);

			
			//tabs
  			static void statTabs(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbTabs(w);}	
			void cbTabs(Fl_Widget*);

			//settings tab
  			static void statSetOutList(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbSetOutList(w);}	
			void cbSetOutList(Fl_Widget*);
  			static void statSetOutNew(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbSetOutNew(w);}	
			void cbSetOutNew(Fl_Widget*);
  			static void statSetOutRemove(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbSetOutRemove(w);}	
			void cbSetOutRemove(Fl_Widget*);
  			static void statSetOutName(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbSetOutName(w);}	
			void cbSetOutName(Fl_Widget*);
  			static void statSetCheckPoly(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbSetCheckPoly(w);}	
			void cbSetCheckPoly(Fl_Widget*);
  			static void statSetPoly(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbSetPoly(w);}	
			void cbSetPoly(Fl_Widget*);
  			static void statSetVolume(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbSetVolume(w);}	
			void cbSetVolume(Fl_Widget*);


			//creation tab
			//lists
  			static void statCreaSetupList(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaSetupList(w);}	
			void cbCreaSetupList(Fl_Widget*);
  			static void statCreaKitList(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaKitList(w);}	
			void cbCreaKitList(Fl_Widget*);
  			static void statCreaInstList(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaInstList(w);}	
			void cbCreaInstList(Fl_Widget*);
  			static void statCreaVarList(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaVarList(w);}	
			void cbCreaVarList(Fl_Widget*);
			
			//global
  		
			static void statCreaNew(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaNew(w);}	
			void cbCreaNew(Fl_Widget*);
			static void statCreaCopyPaste(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaCopyPaste(w);}	
			void cbCreaCopyPaste(Fl_Widget*);
  			static void statCreaRemove(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaRemove(w);}	
			void cbCreaRemove(Fl_Widget*);
  			static void statCreaUp(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaUp(w);}	
			void cbCreaUp(Fl_Widget*);
  			static void statCreaDown(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaDown(w);}	
			void cbCreaDown(Fl_Widget*);

  			static void statCreaName(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaName(w);}	
			void cbCreaName(Fl_Widget*);
  			static void statCreaPlayMode(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaPlayMode(w);}	
			void cbCreaPlayMode(Fl_Widget*);
  			static void statCreaCut(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaCut(w);}	
			void cbCreaCut(Fl_Widget*);
  			static void statCreaCheckCut(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaCheckCut(w);}	
			void cbCreaCheckCut(Fl_Widget*);
  			static void statCreaPoly(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaPoly(w);}	
			void cbCreaPoly(Fl_Widget*);
  			static void statCreaCheckPoly(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaCheckPoly(w);}	
			void cbCreaCheckPoly(Fl_Widget*);
  			static void statCreaCheckPitch(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaCheckPitch(w);}	
			void cbCreaCheckPitch(Fl_Widget*);
  			static void statCreaSampleOpen(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaSampleOpen(w);}	
			void cbCreaSampleOpen(Fl_Widget*);

			//audio
  			static void statCreaVolume(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaVolume(w);}	
			void cbCreaVolume(Fl_Widget*);
  			static void statCreaPan(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaPan(w);}	
			void cbCreaPan(Fl_Widget*);
  			static void statCreaAudioOutput(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaAudioOutput(w);}	
			void cbCreaAudioOutput(Fl_Widget*);

			//midi
  			static void statCreaMidiSel(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiSel(w);}	
			void cbCreaMidiSel(Fl_Widget*);
  			static void statCreaMidiAct(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiAct(w);}	
			void cbCreaMidiAct(Fl_Widget*);
  			static void statCreaMidiCC(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiCC(w);}	
			void cbCreaMidiCC(Fl_Widget*);
  			static void statCreaMidiChan(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiChan(w);}	
			void cbCreaMidiChan(Fl_Widget*);
  			static void statCreaMidiMin(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiMin(w);}	
			void cbCreaMidiMin(Fl_Widget*);
  			static void statCreaMidiRoot(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiRoot(w);}	
			void cbCreaMidiRoot(Fl_Widget*);
  			static void statCreaMidiRootFine(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiRootFine(w);}	
			void cbCreaMidiRootFine(Fl_Widget*);
  			static void statCreaMidiMax(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiMax(w);}	
			void cbCreaMidiMax(Fl_Widget*);
  			static void statCreaMidiPiano(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiPiano(w);}	
			void cbCreaMidiPiano(Fl_Widget*);
  			static void statCreaMidiLearn(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbCreaMidiLearn(w);}	
			void cbCreaMidiLearn(Fl_Widget*);
			
			//exec tab
			//lists
  			static void statExecSetupList(Fl_Widget* w,void* f){execWindow *tmpf = static_cast<execWindow *>(f);tmpf->cbExecSetupList(w);}	
			void cbExecSetupList(Fl_Widget*);

			void showMessage(bool,std::string);
			void setStatus(std::string);
			void setTitle(std::string);
			void init();
			void reset();
			void changeKit(int,int);
			void learnMidi(int,int,int);

			void updateExecTab();
			
			void displaySetup(int);
			void displayKit(int);
			void displayInst(int);
			void displayVar(int);
			void updateMidi();

  private:
			tapeutape *tap;	
			bool fileSaved;

			Fl_Menu_Bar *menu;

			messageWindow* messages;
			
			Fl_Group* status;			

			Fl_Tabs * tabs;

			Fl_Group* setTab;
			Fl_Value_Slider *setVolume;
  			Fl_Hold_Browser *setOutList;
			Fl_Button *setOutNew;
			Fl_Button *setOutRemove;
			Fl_Input *setOutName;
			Fl_Check_Button *setCheckPoly;
			Fl_Counter *setPoly;


			Fl_Group* execTab;
  			Fl_Hold_Browser **setupLists;


			Fl_Group* creaTab;			
  			Fl_Hold_Browser *creaSetupList;
  			Fl_Hold_Browser *creaKitList;
  			Fl_Hold_Browser *creaInstList;
  			Fl_Hold_Browser *creaVarList;
			Fl_Tabs * creaTabs;
			Fl_Group* creaGlobalTab;			
			Fl_Group* creaAudioTab;			
			Fl_Group* creaMidiTab;			
			Fl_Button *creaNew;
			Fl_Button *creaRemove;
			Fl_Toggle_Button *creaCopyPaste;
			Fl_Button *creaUp;
			Fl_Button *creaDown;
			Fl_Input *creaName;
			Fl_Output *creaSampleName;
			Fl_Button *creaSampleOpen;
			Fl_Counter *creaPoly;
			Fl_Choice *creaCut;
			Fl_Choice *creaPlayMode;
			Fl_Check_Button *creaPlayLoop;
			Fl_Check_Button *creaPlayReverse;
			Fl_Check_Button *creaCheckCut;
			Fl_Check_Button *creaCheckPoly;
			Fl_Check_Button *creaCheckPitch;
			

			Fl_Value_Slider *creaVolume;
			Fl_Value_Slider *creaPan;
			Fl_Choice *creaAudioOutput;
			tardiSample *creaSample;
			tardiPiano *creaMidiPiano;
			Fl_Choice *creaMidiSel;
			Fl_Check_Button *creaMidiAct;
			Fl_Counter *creaMidiCC;
			Fl_Counter *creaMidiMax;
			Fl_Counter *creaMidiRoot;
			Fl_Value_Slider *creaMidiRootFine;
			Fl_Counter *creaMidiMin;
			Fl_Counter *creaMidiChan;
			Fl_Button *creaMidiLearn;
			int creaActiveList;
			int creaSelectedSetup;
			int creaSelectedKit;
			int creaSelectedInst;
			int creaSelectedVar;
			setup* copySetup;
			kit* copyKit;
			instrument* copyInst;
			variation* copyVar;
			int copyType;
			std::string lastSampleDir;

			int creaMidiType,creaMidiCnt;
			

			std::string statusStr;
};

#endif
