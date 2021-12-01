/***************************************************************************
 *            execWindow.cpp
 *
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

#include "execWindow.h"
#include "../base/tapeutape.h"

#include "../nsm/nsm.h"
extern nsm_client_t *nsm;
extern bool global_nsm_opional_gui;
extern bool global_nsm_visible;

using namespace std;

#define WINDOW_MINWIDTH 400
#define WINDOW_MINHEIGHT 500
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MENUBAR_HEIGHT 26
#define TAB_HEIGHT 32
#define CREAPANEL_HEIGHT 220
#define BUTTON_HEIGHT 26

execWindow::execWindow(const char* titre,tapeutape *t):
	Flat_Double_Window(WINDOW_WIDTH,WINDOW_HEIGHT,titre),
	tap(t),
	fileSaved(true),
	copyType(0),
	lastSampleDir("")
{

	size_range(WINDOW_MINWIDTH, WINDOW_MINHEIGHT);

	int w = WINDOW_WIDTH;
	int h = WINDOW_HEIGHT;
	int y;

	setupLists = NULL;

	//fltk init
	Fl::visual(FL_DOUBLE|FL_RGB);

	//window class
	this->iconlabel(std::string("/usr/local/share/pixmaps/tapeutape.png").c_str());

	//callback
	this->callback(statWindow,this);

	//menu
	Fl_Menu_Item menuitems[] =
	{
  		{ "&File",0, 0, 0, FL_SUBMENU },
    			{ "&Open File",FL_CTRL + 'o',(Fl_Callback *)statOpen,this },
    			{ "&Save File",FL_CTRL + 's',(Fl_Callback *)statSave,this},
    			{ "S&ave File as",FL_CTRL + FL_SHIFT + 's',(Fl_Callback *)statSaveAs,this},
    			//{ "O&ptions", FL_CTRL + 'p',NULL,0},
    			{ "&Messages", FL_CTRL + 'm', statMessages,this},
    			{ global_nsm_opional_gui ? "&Hide" : "&Quit",FL_CTRL + 'q', (Fl_Callback *)statQuit,this},
			{0},
		{0}
	};
	menu = new Flat_Menu_Bar(-1, 0, w+2, MENUBAR_HEIGHT);
	menu->copy(menuitems);
	add(*menu);

	//main wrapper
	tabsWrapper = new Flat_Group(0,MENUBAR_HEIGHT,w,h-MENUBAR_HEIGHT*2);
	add(*tabsWrapper);
	resizable(tabsWrapper);

	//status
	status = new Flat_Group(-1,h-MENUBAR_HEIGHT,w+2,MENUBAR_HEIGHT+1,"");
	status->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT);
	status->box(FL_BORDER_BOX);
	add(*status);

	//messages
	messages = new messageWindow(300,150,"Messages");

	//create the tabs
	tabs = new Flat_Tabs(0,MENUBAR_HEIGHT,w,h-MENUBAR_HEIGHT*2);
	tabsWrapper->add(*tabs);
	tabs->when(FL_WHEN_CHANGED);
	tabs->callback(statTabs,this);
	execTab = new Flat_Group(0,MENUBAR_HEIGHT+TAB_HEIGHT,w,h,"Execution");
	creaTab = new Flat_Group(0,MENUBAR_HEIGHT+TAB_HEIGHT,w,h,"Creation");
	setTab = new Flat_Group(0,MENUBAR_HEIGHT+TAB_HEIGHT,w,h,"Settings");
	tabs->insert(*setTab,0);
	tabs->insert(*creaTab,1);
	tabs->insert(*execTab,2);
	tabs->resizable(creaTab);


	//settings
	setVolume = new Flat_Value_Slider(134,80,150,20,"Global Volume    ");
	setVolume->align(FL_ALIGN_LEFT);
	setVolume->type(FL_HOR_FILL_SLIDER);
	setVolume->bounds(0.0,3.0);
	setVolume->value(1.0);
	setVolume->callback(statSetVolume,this);
	setCheckPoly = new Flat_Check_Button(129,120,20,20,"Limit Polyphony ");
	setCheckPoly->align(FL_ALIGN_LEFT);
	setCheckPoly->callback(statSetCheckPoly,this);
	setPoly = new Flat_Counter(151,120,70,20,"");
	setPoly->type(FL_SIMPLE_COUNTER);
	setPoly->step(1);
	setPoly->bounds(1,100);
	setPoly->value(1);
	setPoly->callback(statSetPoly,this);
	setPoly->deactivate();
	setOutList = new Flat_Hold_Browser(10,180,200,100,"Stereo Outputs");
	setOutList->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);
	setOutList->callback(statSetOutList,this);
	setOutName = new Fl_Input(220,180,100,20,"Output name");
	setOutName->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);
	setOutName->callback(statSetOutName,this);
	setOutName->when(FL_WHEN_CHANGED);
	setOutNew = new Flat_Button(10,290,40,BUTTON_HEIGHT,"Add");
	setOutNew->callback(statSetOutNew,this);
	setOutRemove = new Flat_Button(60,290,80,BUTTON_HEIGHT,"Remove");
	setOutRemove->callback(statSetOutRemove,this);
	setTab->insert(*setVolume,0);
	setTab->insert(*setCheckPoly,1);
	setTab->insert(*setPoly,2);
	setTab->insert(*setOutList,3);
	setTab->insert(*setOutNew,4);
	setTab->insert(*setOutRemove,5);
	setTab->insert(*setOutName,6);
	setTab->resizable(0);

	//creation tab
	y = MENUBAR_HEIGHT+TAB_HEIGHT;


	y += 30;
	execPack = new Flat_Group(0, y,w,h-MENUBAR_HEIGHT*2-TAB_HEIGHT-30);
	execTab->insert(*execPack, 0);
	execTab->resizable(*execPack);

	creaPack = new Flat_Group(10, y,w-20,h-CREAPANEL_HEIGHT-MENUBAR_HEIGHT*2-TAB_HEIGHT-BUTTON_HEIGHT - 20 - 30);

	int _h = h - y - CREAPANEL_HEIGHT - BUTTON_HEIGHT - 20 - MENUBAR_HEIGHT;
	creaSetupList = new Flat_Hold_Browser(10,y,(w-50)/4,_h,"Setups");
	creaSetupList->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	creaSetupList->callback(statCreaSetupList,this);
	creaKitList = new Flat_Hold_Browser(20 + (w-50)/4,y,(w-50)/4,_h,"Kits");
	creaKitList->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	creaKitList->callback(statCreaKitList,this);
	creaInstList = new Flat_Hold_Browser(30 + 2*(w-50)/4,y,(w-50)/4,_h,"Instruments");
	creaInstList->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	creaInstList->callback(statCreaInstList,this);
	creaVarList = new Flat_Hold_Browser(40 + 3*(w-50)/4,y,(w-50)/4,_h,"Variations");
	creaVarList->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
	creaVarList->callback(statCreaVarList,this);

	y = h-CREAPANEL_HEIGHT-MENUBAR_HEIGHT - 20 - BUTTON_HEIGHT;
	creaBtns = new Flat_Group(10, y, w-20, BUTTON_HEIGHT + 20,"");
	y += 10;
	creaNew = new Flat_Button(10,y,40,BUTTON_HEIGHT,"Add");
	creaNew->callback(statCreaNew,this);
	creaNew->tooltip("Add an element to the list");
	creaCopyPaste = new Flat_Toggle_Button(60,y,50,BUTTON_HEIGHT,"Copy");
	creaCopyPaste->callback(statCreaCopyPaste,this);
	creaCopyPaste->tooltip("Copy/Paste the element");
	creaUp = new Flat_Button(120,y,40,BUTTON_HEIGHT,"Up");
	creaUp->callback(statCreaUp,this);
	creaUp->tooltip("Move the element up");
	creaDown = new Flat_Button(170,y,50,BUTTON_HEIGHT,"Down");
	creaDown->callback(statCreaDown,this);
	creaDown->tooltip("Move the element down");
	creaRemove = new Flat_Button(230,y,60,BUTTON_HEIGHT,"Delete");
	creaRemove->callback(statCreaRemove,this);
	creaRemove->tooltip("Remove the element");
	creaBtns->insert(*creaNew,0);
	creaBtns->insert(*creaCopyPaste,1);
	creaBtns->insert(*creaUp,2);
	creaBtns->insert(*creaDown,3);
	creaBtns->insert(*creaRemove,4);
	creaBtns->resizable(0);

	y = h-CREAPANEL_HEIGHT-MENUBAR_HEIGHT;
	creaTabsWrapper = new Flat_Group(0,y,w+1,CREAPANEL_HEIGHT,"");
	creaTabsWrapper->box(FL_BORDER_BOX);
	creaTabs = new Flat_Tabs(0,y+1,w+1,CREAPANEL_HEIGHT-1,"");
	creaTabsWrapper->insert(*creaTabs,0);
	creaPack->insert(*creaSetupList,0);
	creaPack->insert(*creaKitList,1);
	creaPack->insert(*creaInstList,2);
	creaPack->insert(*creaVarList,3);
	creaTab->insert(*creaPack,0);
	creaTab->insert(*creaBtns,1);
	creaTab->insert(*creaTabsWrapper,2);
	creaTab->resizable(creaPack);

	creaActiveList=-1;
	creaSelectedKit=-1;
	creaSelectedSetup=-1;
	creaSelectedInst=-1;
	creaSelectedVar=-1;

	y += TAB_HEIGHT;
	creaGlobalTab = new Flat_Group(0, y,w,CREAPANEL_HEIGHT-TAB_HEIGHT,"Global");
	creaAudioTab = new Flat_Group(0, y,w,CREAPANEL_HEIGHT-TAB_HEIGHT,"Audio");
	creaMidiTab = new Flat_Group(0, y,w,CREAPANEL_HEIGHT-TAB_HEIGHT,"Midi");
	creaTabs->insert(*creaGlobalTab,0);
	creaTabs->insert(*creaAudioTab,1);
	creaTabs->insert(*creaMidiTab,2);
	creaGlobalTab->resizable(0);
	// creaAudioTab->resizable(0);
	// creaMidiTab->resizable(0);
	// creaTabs->resizable(0);


	y += 10;
	creaName = new Fl_Input(60,y,200,20,"Name ");
	creaName->align(FL_ALIGN_LEFT);
	creaName->callback(statCreaName,this);
	creaName->when(FL_WHEN_CHANGED);
	creaPlayMode= new Flat_Choice(90,y + 30,80,20,"Play Mode ");
	creaPlayMode->callback(statCreaPlayMode,this);
	creaPlayMode->add("Normal");
	creaPlayMode->add("Trigger");
	creaPlayMode->value(0);
	creaPlayLoop = new Flat_Check_Button(220, y + 30,20,20,"Loop");
	creaPlayLoop->align(FL_ALIGN_LEFT);
	creaPlayLoop->callback(statCreaPlayMode,this);
	creaPlayReverse = new Flat_Check_Button(310, y +30,20,20,"Reverse");
	creaPlayReverse->align(FL_ALIGN_LEFT);
	creaPlayReverse->callback(statCreaPlayMode,this);
	creaPoly = new Flat_Counter(110,y+60,70,20,"");
	creaPoly->align(FL_ALIGN_LEFT);
	creaPoly->type(FL_SIMPLE_COUNTER);
	creaPoly->step(1);
	creaPoly->bounds(1,10);
	creaPoly->value(1);
	creaPoly->callback(statCreaPoly,this);
	creaPoly->deactivate();
	creaCheckPoly = new Flat_Check_Button(10,y+60,20,20,"Polyphony");
	creaCheckPoly->callback(statCreaCheckPoly,this);
	creaPoly->align(FL_ALIGN_RIGHT);
	creaCut = new Flat_Choice(90,h-45,90,20,"");
	creaCut->align(FL_ALIGN_LEFT);
	creaCut->callback(statCreaCut,this);
	creaCut->deactivate();
	creaCheckCut = new Flat_Check_Button(10,y+90,20,20,"Cut");
	creaCheckCut->align(FL_ALIGN_RIGHT);
	creaCheckCut->callback(statCreaCheckCut,this);
	creaCheckPitch = new Flat_Check_Button(320,y+60,20,20,"Pitch Over Range");
	creaCheckPitch->align(FL_ALIGN_LEFT);
	creaCheckPitch->callback(statCreaCheckPitch,this);
	creaSampleName = new Fl_Output(72,y,250,20,"Sample ");
	creaSampleOpen = new Flat_Button(10,y+30,60,BUTTON_HEIGHT,"Open");
	creaSampleOpen->callback(statCreaSampleOpen,this);
	creaGlobalTab->insert(*creaName,5);
	creaGlobalTab->insert(*creaPlayMode,6);
	creaGlobalTab->insert(*creaPlayLoop,6);
	creaGlobalTab->insert(*creaPlayReverse,6);
	creaGlobalTab->insert(*creaCheckPoly,7);
	creaGlobalTab->insert(*creaPoly,8);
	creaGlobalTab->insert(*creaCheckCut,9);
	creaGlobalTab->insert(*creaCut,10);
	creaGlobalTab->insert(*creaCheckPitch,11);
	creaGlobalTab->insert(*creaSampleName,12);
	creaGlobalTab->insert(*creaSampleOpen,13);
	creaPlayMode->hide();
	creaPlayLoop->hide();
	creaPlayReverse->hide();
	creaPoly->hide();
	creaCut->hide();
	creaCheckPoly->hide();
	creaCheckCut->hide();
	creaCheckPitch->hide();
	creaSampleName->hide();
	creaSampleOpen->hide();


	Flat_Group * audioControls= new Flat_Group(0, y, w, 200);
	audioControls->resizable(0);
	creaVolume = new Flat_Value_Slider(70,y,150,20,"Volume");
	creaVolume->align(FL_ALIGN_LEFT);
	creaVolume->type(FL_HOR_FILL_SLIDER);
	creaVolume->bounds(0.0,3.0);
	creaVolume->callback(statCreaVolume,this);
	creaPan = new Flat_Value_Slider(40,y+30,150,20,"Pan");
	creaPan->align(FL_ALIGN_LEFT);
	creaPan->type(FL_HOR_SLIDER);
	creaPan->bounds(-1.0,1.0);
	creaPan->value(0.0);
	creaPan->callback(statCreaPan,this);
	creaAudioOutput = new Flat_Choice(107,y+60,100,20,"Audio Output");
	creaAudioOutput->callback(statCreaAudioOutput,this);
	audioControls->end();
	creaSample = new tardiSample(10,y,w-20,100,"Sample");
	creaAudioTab->insert(*audioControls,0);
	creaAudioTab->insert(*creaSample,1);
	creaAudioTab->resizable(creaSample);
	creaVolume->hide();
	creaPan->hide();
	creaAudioOutput->hide();
	creaSample->hide();

	Flat_Group * midiControls= new Flat_Group(0, y, w, 200);
	midiControls->resizable(0);
	creaMidiSel = new Flat_Choice(10,y,200,20,"");
	creaMidiSel->callback(statCreaMidiSel,this);
	creaMidiAct = new Flat_Check_Button(60,y+95,20,20,"Active");
	creaMidiAct->align(FL_ALIGN_LEFT);
	creaMidiAct->callback(statCreaMidiAct,this);
	creaMidiAct->hide();
	creaMidiChan = new Flat_Counter(10,y+50,62,20,"Channel");
	creaMidiChan->type(FL_SIMPLE_COUNTER);
	creaMidiChan->align(FL_ALIGN_TOP);
	creaMidiChan->step(1);
	creaMidiChan->bounds(1,16);
	creaMidiChan->deactivate();
	creaMidiChan->deactivate();
	creaMidiChan->callback(statCreaMidiCC,this);
	creaMidiCC = new Flat_Counter(140,y+50,50,20,"Control");
	creaMidiCC->type(FL_SIMPLE_COUNTER);
	creaMidiCC->align(FL_ALIGN_TOP);
	creaMidiCC->step(1);
	creaMidiCC->bounds(0,127);
	creaMidiCC->deactivate();
	creaMidiCC->hide();
	creaMidiCC->callback(statCreaMidiCC,this);
	creaMidiMin = new Flat_Counter(82,y+50,50,20,"Min");
	creaMidiMin->type(FL_SIMPLE_COUNTER);
	creaMidiMin->align(FL_ALIGN_TOP);
	creaMidiMin->step(1);
	creaMidiMin->bounds(0,127);
	creaMidiMin->callback(statCreaMidiMin,this);
	creaMidiMin->hide();
	creaMidiRoot = new Flat_Counter(142,y+50,50,20,"Root");
	creaMidiRoot->type(FL_SIMPLE_COUNTER);
	creaMidiRoot->align(FL_ALIGN_TOP);
	creaMidiRoot->step(1);
	creaMidiRoot->bounds(0,127);
	creaMidiRoot->callback(statCreaMidiRoot,this);
	creaMidiRoot->hide();
	creaMidiRootFine = new Flat_Value_Slider(50,y+85,100,20,"Tune");
	creaMidiRootFine->align(FL_ALIGN_LEFT);;
	creaMidiRootFine->type(FL_HOR_SLIDER);
	creaMidiRootFine->bounds(-1.0,1.0);
	creaMidiRootFine->value(0.0);
	creaMidiRootFine->callback(statCreaMidiRootFine,this);
	creaMidiRootFine->hide();
	creaMidiMax = new Flat_Counter(202,y+50,50,20,"Max");
	creaMidiMax->type(FL_SIMPLE_COUNTER);
	creaMidiMax->align(FL_ALIGN_TOP);
	creaMidiMax->step(1);
	creaMidiMax->bounds(0,127);
	creaMidiMax->callback(statCreaMidiMax,this);
	creaMidiMax->hide();
	creaMidiLearn = new Flat_Button(262,y+50,50,20,"Learn");
	creaMidiLearn->type(FL_TOGGLE_BUTTON);
	creaMidiLearn->callback(statCreaMidiLearn,this);
	creaMidiLearn->hide();
	midiControls->end();
	creaMidiPiano = new tardiPiano(10,y+120,w-20,30,"");
	creaMidiPiano->callback(statCreaMidiPiano,this);
	creaMidiPiano->hide();
	creaMidiTab->insert(*midiControls,0);
	creaMidiTab->insert(*creaMidiPiano,1);
	creaMidiTab->resizable(creaMidiPiano);

	visible = false;

	//flat theme
	tabsWrapper->color(COLOR_REAR);
	creaTabsWrapper->color(COLOR_REAR);
}

execWindow::~execWindow()
{
}

void execWindow::setTitle(std::string t)
{
	label(t.c_str());
	this->redraw();
	Fl::check();
}

void execWindow::cbWindow(Fl_Widget*)
{
	if (Fl::event()==FL_SHORTCUT && Fl::event_key()==FL_Escape)
    		return; // ignore Escape

	cbQuit(this);
}

void execWindow::reset()
{
	//erase everything
	setOutList->clear();
	creaAudioOutput->clear();

	//erase everything
	creaSample->setBuffers(NULL);

	//erase the execution  lists
	if(setupLists)
	{
		for(int i=0;i<tap->getNbSetups();++i)
		{
			execTab->remove(setupLists[i]);
			delete setupLists[i];
		}
		delete [] setupLists;
		setupLists=NULL;
	}

	execTab->redraw();

	//clear the creation lists
	creaVarList->clear();
	creaInstList->clear();
	creaKitList->clear();
	creaSetupList->clear();

	this->redraw();
	Fl::check();
}

void execWindow::init()
{

	setStatus("");

	creaMidiType=0;
	creaMidiCnt=0;
	fileSaved=true;


	//create the list of audio outputs
	for(unsigned int i=0;i<tap->getNbJackStereoChannels();++i)
	{
		setOutList->add(tap->getJackStereoChannelName(i).c_str());
		creaAudioOutput->add(tap->getJackStereoChannelName(i).c_str());
	}

	//set the global volume and the polyphony
	setVolume->value(tap->getGlobalVolume());
	setPoly->value(tap->getPolyphony());


	//create the setup list in the crea tab
	creaSetupList->clear();
	for(int j=0;j<tap->getNbSetups();++j)
	{
		std::string setName = tap->getSetup(j)->getName();
		creaSetupList->add(setName.c_str());
	}

	updateExecTab();

	tabs->value(setTab);
	// tabs->value(execTab);

	this->redraw();
	Fl::check();
}

//-----------------------------------MENU--------------------------------
void execWindow::cbOpen(Fl_Widget* w)
{
	//open file dialog
	Fl_Native_File_Chooser *fb = new Fl_Native_File_Chooser();
	fb->title("Open .tap file");
	fb->type(Fl_Native_File_Chooser::BROWSE_FILE);
	fb->filter("Tapeutape session\t*.tap");

	string dir = "/home/";
	size_t found = tap->getCompleteFileName().find_last_of("/");
	if(found!=tap->getCompleteFileName().npos)
	{
		dir=tap->getCompleteFileName().substr(0,found+1);
	}
	fb->directory(dir.c_str());
	if (!fb->show()) {
		tap->load((char *)fb->filename());
		tap->start();
	}
	delete fb;
}

void execWindow::cbSave(Fl_Widget* w)
{
	if(tap->getCompleteFileName()=="")
	{
		cbSaveAs(this);
	}
	else
	{
		size_t size = tap->getCompleteFileName().size() + 1;
		char * buffer = new char[ size ];
		strncpy( buffer, tap->getCompleteFileName().c_str(), size );
		tap->save(buffer);
		delete [] buffer;
		std::string m = "Saved as "+tap->getFileName();
		fileSaved=true;
		setStatus(m);
	}
}

void execWindow::cbSaveAs(Fl_Widget* w)
{
	//open file dialog
	Fl_Native_File_Chooser *fb = new Fl_Native_File_Chooser();
	fb->title("Save .tap file");
	fb->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	fb->filter("Tapeutape session\t*.tap");
	fb->options(Fl_Native_File_Chooser::SAVEAS_CONFIRM|Fl_Native_File_Chooser::USE_FILTER_EXT);

	string dir = "/home/";
	size_t found = tap->getCompleteFileName().find_last_of("/");
	if(found!=tap->getCompleteFileName().npos)
	{
		dir=tap->getCompleteFileName().substr(0,found+1);
	}
	fb->directory(dir.c_str());
	if (!fb->show()) {
		string fn = fb->filename();
		string suffix = fn.substr( fn.find_last_of(".") + 1, string::npos);
		for (basic_string<char>::iterator p = suffix.begin();
				p != suffix.end(); p++) {
			*p = tolower(*p);
		}
		if (suffix != "tap") fn = fn + ".tap";

		tap->save((char *)fn.c_str());
		std::string f = tap->getFileName();
		std::string m = "Saved as "+f;
		setStatus(m);
		fileSaved=true;
	}
	delete fb;
}

void execWindow::cbQuit(Fl_Widget* w)
{
	if (global_nsm_opional_gui) {
		global_nsm_visible = false;
		return;
	}

	if(!fileSaved)
	{
		switch(fl_choice("File not saved ! Quit without saving ?", "Quit without saving","Save then Quit" ,"Don't quit"))
		{
			case 0:
			break;
			case 1:
			{
				cbSave(this);
				if(!fileSaved)
					return;
			}
			break;
			default: return;

		}
	}

	//tout nettoyer puis quitter
	this->reset();
	tap->stop();

	exit(0);
}


//--------------------------------MESSAGES-------------------------------------
void execWindow::cbMessages(Fl_Widget*)
{
	messages->show();
}

void execWindow::showMessage(bool e,std::string mess)
{
	setStatus(mess);
	messages->addMessage(mess);
	if(e)
	{
		messages->show();
	}
}

void execWindow::setStatus(std::string st)
{
	statusStr = st;
	status->label(statusStr.c_str());
	Fl::check();
}

//---------------------------------TABS-----------------------------------------
void execWindow::cbTabs(Fl_Widget*)
{
	updateExecTab();
	setStatus("");
}

//---------------------------------SETTINGS-------------------------------------
void execWindow::cbSetOutList(Fl_Widget*)
{
	fileSaved=false;
	if(setOutList->value()>0)
	{
		setOutName->value(setOutList->text(setOutList->value()));
	}
}

void execWindow::cbSetOutNew(Fl_Widget*)
{
	fileSaved=false;
	bool outputCreated=false;
	unsigned int outputNb=0;
	std::string outputName;
	while(!outputCreated)
	{
		std::ostringstream oss;
		oss << outputNb;
		outputName ="Output"+oss.str();
		outputCreated=true;
		for(unsigned int i=0;i<tap->getNbJackStereoChannels();++i)
		{
			if(tap->getJackStereoChannelName(i)==outputName)
			{
				outputCreated=false;
			}
		}
		++outputNb;
	}
	tap->addCreateJackStereoChannel(outputName);
	setOutList->add(outputName.c_str());
	creaAudioOutput->add(outputName.c_str());
}

void execWindow::cbSetOutRemove(Fl_Widget*)
{
	fileSaved=false;
	//remove the output, if there is one left after that
	if(tap->getNbJackStereoChannels()>1 && setOutList->value()>0)
	{
		tap->removeJackStereoChannel(setOutList->value()-1);
		creaAudioOutput->remove(setOutList->value()-1);
		setOutList->remove(setOutList->value());
	}
}

void execWindow::cbSetOutName(Fl_Widget*)
{
	fileSaved=false;
	if(setOutList->value()>0)
	{
		tap->setJackStereoChannelName(setOutList->value()-1,std::string(setOutName->value()));
		setOutList->text(setOutList->value(),setOutName->value());
		creaAudioOutput->replace(setOutList->value()-1,setOutName->value());
	}
}

void execWindow::cbSetCheckPoly(Fl_Widget*)
{
	fileSaved=false;
	if(setCheckPoly->value()==1)
	{
		setPoly->activate();
		setPoly->value(10);
		tap->setPolyphony(100);
	}
	else
	{
		setPoly->deactivate();
		setPoly->value(100);
		tap->setPolyphony(100);
	}
}

void execWindow::cbSetPoly(Fl_Widget*)
{
	fileSaved=false;
	tap->setPolyphony((int)(setPoly->value()));
}

void execWindow::cbSetVolume(Fl_Widget*)
{
	fileSaved=false;
	tap->setGlobalVolume(setVolume->value());
}

//---------------------------------CREATION-------------------------------------
//clic on the setup list
void execWindow::cbCreaSetupList(Fl_Widget*)
{
	//set the action
	setStatus("Editing Setups List");

	creaActiveList=0;

	if(creaSetupList->value()>0)
	{
		creaSelectedSetup =  creaSetupList->value() - 1;
	}
	else
	{
		creaSelectedSetup=-1;
	}

	displaySetup(creaSelectedSetup);
}

void execWindow::displaySetup(int s)
{
	creaKitList->clear();
	creaInstList->clear();
	creaVarList->clear();
	creaMidiCC->value(0);
	creaMidiChan->value(0);
	creaName->value("");

	if(s>=0)
	{
		//show the setup's kits
		for(int j=0;j<tap->getSetup(s)->getNbKits();++j)
		{
			std::string kitName = tap->getSetup(s)->getKit(j)->getName();
			creaKitList->add(kitName.c_str());
		}

		//display the setup's name
		creaName->value(tap->getSetup(s)->getName().c_str());

		//midi
		creaMidiSel->clear();
		creaMidiSel->add("Select Kit");
		creaMidiSel->value(0);
		if(tap->getSetup(s)->getCC()==-1)
		{
			creaMidiAct->value(0);
			creaMidiCC->value(-1);
			creaMidiCC->deactivate();
			creaMidiChan->value(-1);
			creaMidiChan->deactivate();
			creaMidiLearn->deactivate();
		}
		else
		{
			creaMidiAct->value(1);
			creaMidiCC->activate();
			creaMidiChan->activate();
			creaMidiLearn->activate();
			creaMidiCC->value(tap->getSetup(s)->getCC());
			creaMidiChan->value(tap->getSetup(s)->getChannel());
		}
		creaMidiType=0;
	}

	creaPlayMode->hide();
	creaPlayLoop->hide();
	creaPlayReverse->hide();
	creaPoly->hide();
	creaCut->hide();
	creaCheckPoly->hide();
	creaCheckCut->hide();
	creaCheckPitch->hide();
	creaName->show();
	creaVolume->hide();
	creaPan->hide();
	creaAudioOutput->hide();
	creaSample->hide();
	creaSampleName->hide();
	creaSampleOpen->hide();

	creaMidiPiano->hide();
	creaMidiMin->hide();
	creaMidiMax->hide();
	creaMidiRoot->hide();
	creaMidiCC->show();
	creaMidiChan->show();
	creaMidiAct->show();
	creaMidiLearn->show();

	Fl::check();
}

void execWindow::cbCreaKitList(Fl_Widget*)
{
	//set the action
	setStatus("Editing Kits List");

	creaActiveList=1;

	if(creaKitList->value()>0)
	{
		creaSelectedKit = creaKitList->value()-1;
	}
	else
	{
		creaSelectedKit=-1;
	}

	displayKit(creaSelectedKit);

}


void execWindow::displayKit(int k)
{
	creaInstList->clear();
	creaVarList->clear();
	creaCut->clear();
	creaName->value("");

	if(k>=0)
	{
		//show the kit's instruments
		for(int j=0;j<tap->getSetup(creaSelectedSetup)->getKit(k)->getNbInstruments();++j)
		{
			std::string instName = tap->getSetup(creaSelectedSetup)->getKit(k)->getInstrument(j)->getName();
			creaInstList->add(instName.c_str());

			//set the other instruments of the kit for the cut
			creaCut->add(instName.c_str());
		}

		//get the name
		creaName->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getName().c_str());

		//midi
		creaMidiSel->clear();
		//creaMidiSel->add("Change Volume");
		//creaMidiSel->value(0);
		creaMidiType=0;

		//audio
		creaVolume->show();
		creaVolume->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getVolume());
	}

	//hide all the other widgets
	creaPlayMode->hide();
	creaPlayLoop->hide();
	creaPlayReverse->hide();
	creaPoly->hide();
	creaCut->hide();
	creaCheckPoly->hide();
	creaCheckCut->hide();
	creaCheckPitch->hide();
	creaName->show();
	creaPan->hide();
	creaAudioOutput->hide();
	creaSample->hide();
	creaSampleName->hide();
	creaSampleOpen->hide();

	creaMidiPiano->hide();
	creaMidiMin->hide();
	creaMidiMax->hide();
	creaMidiRoot->hide();
	creaMidiRootFine->hide();
	creaMidiCC->show();
	creaMidiChan->show();
	creaMidiAct->show();
	creaMidiLearn->show();

	creaTabs->redraw();
	Fl::check();
}


void execWindow::cbCreaInstList(Fl_Widget*)
{
	//set the action
	setStatus("Editing Instruments List");

	creaActiveList=2;
	if(creaInstList->value()>0)
		creaSelectedInst = creaInstList->value()-1;
	else
		creaSelectedInst=-1;

	displayInst(creaSelectedInst);

}



void execWindow::displayInst(int ins)
{

	creaVarList->clear();
	creaName->value("");

	if(ins>=0)
	{
		//show the instrument's variations
		for(int j=0;j<tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getNbVariations();++j)
		{
			sample* s = tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getVariation(j)->getSample();
			std::string varName="no sample yet";
			if(s)
			{
				varName = s->getName();
			}
			creaVarList->add(varName.c_str());
		}


		//get the name
		creaName->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getName().c_str());

		//get the play mode
		creaPlayMode->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getPlayMode());
		creaPlayLoop->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getPlayLoop());
		creaPlayReverse->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getPlayReverse());

		//get the polyphony
		int pol=tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getPolyphony();
		if(pol!=0)
		{
			creaCheckPoly->value(1);
			creaPoly->activate();
			creaPoly->value(pol);
		}
		else
		{
			creaCheckPoly->value(0);
			creaPoly->deactivate();
		}


		//get the cut
		int cut=tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getCut();
		if(cut>=0)
		{
			creaCheckCut->value(1);
			creaCut->activate();
			creaCut->value(cut);
		}
		else
		{
			creaCheckCut->value(0);
			creaCut->deactivate();
		}

		//get the pitch over range
		creaCheckPitch->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getPitchOverRange());

		//get the volume
		creaVolume->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getVolume());
		//get the pan
		creaPan->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getPan());
		//get the audio output
		creaAudioOutput->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getJackStereoChannel());



		//get the note range
		unsigned short minNote= tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getMinNote();
		unsigned short maxNote= tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getMaxNote();
		unsigned short rootNote= tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getRootNote();
		float rootNoteFine= tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getRootNoteFine();
		unsigned short chan= tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(ins)->getMidiChannel();

		creaMidiPiano->setLSelect(minNote);
		creaMidiPiano->setMSelect(rootNote);
		creaMidiPiano->setRSelect(maxNote);
		creaMidiMin->value(minNote);
		creaMidiRoot->value(rootNote);
		creaMidiRootFine->value(rootNoteFine);
		creaMidiMax->value(maxNote);
		creaMidiChan->value(chan);

	}

	//midi
	creaMidiSel->clear();
	creaMidiSel->add("Note Range");
	creaMidiType=2;
	/*
	creaMidiSel->add("Volume");
	creaMidiSel->add("Pan");
	creaMidiSel->add("Play Mode");
	creaMidiSel->add("Polyphony");
	creaMidiSel->add("Cut By");
	creaMidiSel->add("Audio Output");
	*/
	creaMidiSel->value(0);

	creaPlayMode->show();
	creaPlayLoop->show();
	creaPlayReverse->show();
	creaPoly->show();
	//creaCut->show();
	creaCheckPoly->show();
	//creaCheckCut->show();
	creaCheckPitch->show();
	creaName->show();
	creaVolume->show();
	creaPan->show();
	creaAudioOutput->show();
	creaSample->hide();
	creaSampleName->hide();
	creaSampleOpen->hide();

	creaMidiLearn->activate();
	creaMidiChan->activate();
	creaMidiCC->hide();
	creaMidiAct->hide();
	creaMidiLearn->show();
	creaMidiMin->show();
	creaMidiMax->show();
	creaMidiRoot->show();
	creaMidiRootFine->show();
	creaMidiChan->show();
	creaMidiPiano->show();


	creaTabs->redraw();
	Fl::check();
}

void execWindow::cbCreaVarList(Fl_Widget*)
{
	//set the action
	setStatus("Editing Variations List");

	creaActiveList=3;
	if(creaVarList->value()>0)
		creaSelectedVar = creaVarList->value()-1;
	else
		creaSelectedVar=-1;

	displayVar(creaSelectedVar);
}

void execWindow::displayVar(int v)
{
	//sample name
	creaSampleName->value("");
	//show the sample
	creaSample->setNbChannels(0);
	creaSample->setFramesCount(0);
	creaSample->setBuffers(NULL);


	//velocity range
	creaMidiMin->value(0);
	creaMidiMax->value(1);

	if(v>=0)
	{
		sample* s = tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->getVariation(v)->getSample();
		if(s)
		{
			//sample name
			creaSampleName->value(s->getAbsoluteName().c_str());
			//show the sample
			creaSample->setNbChannels(s->getChannelsCount());
			creaSample->setFramesCount(s->getFramesCount());
			creaSample->setBuffers(s->getBuffers());


			//velocity range
			creaMidiMin->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->getVariation(creaSelectedVar)->getMinVeloc());
			creaMidiMax->value(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->getVariation(creaSelectedVar)->getMaxVeloc());
		}
	}

	creaPlayMode->hide();
	creaPlayLoop->hide();
	creaPlayReverse->hide();
	creaPoly->hide();
	creaCut->hide();
	creaCheckPoly->hide();
	creaCheckCut->hide();
	creaCheckPitch->hide();
	creaName->hide();
	creaSampleName->show();
	creaSampleOpen->show();

	creaVolume->hide();
	creaPan->hide();
	creaAudioOutput->hide();
	creaSample->show();


	creaMidiLearn->activate();
	creaMidiCC->hide();
	creaMidiAct->hide();
	creaMidiLearn->show();
	creaMidiMin->show();
	creaMidiMax->show();
	creaMidiRootFine->hide();
	creaMidiChan->deactivate();
	creaMidiPiano->hide();

	//midi
	creaMidiSel->clear();
	creaMidiSel->add("Velocity Range");
	creaMidiSel->value(0);
	creaMidiType=1;

	creaSample->redraw();
	tabs->redraw();
	creaTabs->redraw();
	Fl::check();
}

void execWindow::cbCreaNew(Fl_Widget*)
{
	fileSaved=false;
	switch(creaActiveList)
	{
		case 0://setup
			{
				setup* ns = new setup();
				ns->setName("new setup");
				tap->addSetup(ns);
				creaSetupList->add("new setup");
				creaSetupList->value(creaSetupList->size());
				creaSelectedSetup=creaSetupList->size()-1;
				creaKitList->clear();
				kit* nk = new kit();
				nk->setName("new kit");
				tap->getSetup(creaSelectedSetup)->addKit(nk);
				creaKitList->add("new kit");
				creaSelectedKit=creaKitList->size()-1;
				creaInstList->clear();
				instrument* ni = new instrument();
				ni->setName("new instrument");
				tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->addInstrument(ni);

				creaInstList->add("new instrument");
				creaSelectedInst=creaInstList->size()-1;
				creaVarList->clear();
				variation* nv = new variation();
				tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->addVariation(nv);
				creaVarList->add("no sample yet");

				displaySetup(creaSelectedSetup);

			}
			break;
		case 1://kit
			{
				if(creaSelectedSetup>=0)
				{
					kit* nk = new kit();
					nk->setName("new kit");
					tap->getSetup(creaSelectedSetup)->addKit(nk);
					creaKitList->add("new kit");
					creaKitList->value(creaKitList->size());
					creaSelectedKit=creaKitList->size()-1;
					creaInstList->clear();
					instrument* ni = new instrument();
					ni->setName("new instrument");
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->addInstrument(ni);
					creaInstList->add("new instrument");
					creaSelectedInst=creaInstList->size()-1;
					creaVarList->clear();
					variation* nv = new variation();
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->addVariation(nv);
					creaVarList->add("no sample yet");


					displayKit(creaSelectedKit);
				}
			}
			break;
		case 2://instrument
			{
				if(creaSelectedKit>=0)
				{
					instrument* ni = new instrument();
					ni->setName("new instrument");
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->addInstrument(ni);
					creaInstList->add("new instrument");
					creaInstList->value(creaInstList->size());
					creaSelectedInst=creaInstList->size()-1;
					creaVarList->clear();
					variation* nv = new variation();
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->addVariation(nv);
					creaVarList->add("no sample yet");

					displayInst(creaSelectedInst);

					//recreate the tap list
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
				}
			}
			break;
		case 3://var
			{
				if(creaSelectedInst>=0)
				{
					variation* nv = new variation();
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->addVariation(nv);
					creaVarList->add("no sample yet");
					creaVarList->value(creaVarList->size());
					creaSelectedVar=creaVarList->size()-1;

					//update the variations
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->updateVariations(-1);

					displayVar(creaSelectedVar);
				}
			}
			break;
		default:
			break;
	}
}

void execWindow::cbCreaCopyPaste(Fl_Widget*)
{
	fileSaved=false;
	bool pasteError=false;
	switch(creaActiveList)
	{
		case 0://setup
			{
				//copying: must be on a setup
				if(creaCopyPaste->value()==1)
				{
					if(creaSelectedSetup>-1)
					{
						copySetup = new setup(*(tap->getSetup(creaSelectedSetup)));
						copyType=1;
						creaCopyPaste->label("Paste");
						showMessage(false,"Setup "+copySetup->getName()+" copied");
					}
					else
					{
						showMessage(false,"Must be on a setup to copy");
						creaCopyPaste->value(0);
					}
				}
				else //pasting: on a setup or not
				{
					if(copyType==1)
					{
						tap->addSetup(copySetup);
						creaSetupList->add(copySetup->getName().c_str());
						creaSetupList->value(creaSetupList->size());
						creaSelectedSetup=creaSetupList->size()-1;
						displaySetup(creaSelectedSetup);
						showMessage(false,"Setup "+copySetup->getName()+" pasted");
					}
					else
					{
						delete copySetup;
						pasteError=true;
					}
					creaCopyPaste->label("C");
				}
			}
			break;
		case 1://kit
			{
				//copying: must be on a kit
				if(creaCopyPaste->value()==1)
				{
					if(creaSelectedKit>-1)
					{
						copyKit = new kit(*(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)));
						copyType=2;
						creaCopyPaste->label("Paste");
						showMessage(false,"Kit "+copyKit->getName()+" copied");
					}
					else
					{
						showMessage(false,"Must be on a kit to copy");
						creaCopyPaste->value(0);
					}
				}
				else //pasting: on a kit or not
				{
					if(copyType==2 && creaSelectedSetup>-1)
					{
						tap->getSetup(creaSelectedSetup)->addKit(copyKit);
						creaKitList->add(copyKit->getName().c_str());
						creaKitList->value(creaKitList->size());
						creaSelectedKit=creaKitList->size()-1;
						displayKit(creaSelectedKit);
						showMessage(false,"Kit "+copyKit->getName()+" pasted");
					}
					else
					{
						delete copyKit;
						pasteError=true;
					}
					creaCopyPaste->label("C");
				}
			}
			break;
		case 2://instrument
			{
				//copying: must be on an instrument
				if(creaCopyPaste->value()==1)
				{
					if(creaSelectedInst>-1)
					{
						copyInst = new instrument(*(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)));
						copyType=3;
						creaCopyPaste->label("Paste");
						showMessage(false,"Instrument "+copyInst->getName()+" copied");
					}
					else
					{
						showMessage(false,"Must be on an instrument to copy");
						creaCopyPaste->value(0);
					}
				}
				else //pasting: on an instrument or not
				{
					if(copyType==3 && creaSelectedKit>-1)
					{
						tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->addInstrument(copyInst);
						//recreate the tap list
						tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
						creaInstList->add(copyInst->getName().c_str());
						creaInstList->value(creaInstList->size());
						creaSelectedInst=creaInstList->size()-1;
						displayInst(creaSelectedInst);
						showMessage(false,"Instrument "+copyInst->getName()+" pasted");
					}
					else
					{
						delete copyInst;
						pasteError=true;
					}
					creaCopyPaste->label("C");
				}
			}
			break;
		case 3://var
			{
				//copying: must be on a variation
				if(creaCopyPaste->value()==1)
				{
					if(creaSelectedVar>-1)
					{
						copyVar = new variation(*(tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->getVariation(creaSelectedVar)));
						copyType=4;
						creaCopyPaste->label("Paste");
						showMessage(false,"Variation "+copyVar->getSample()->getName()+" copied");
					}
					else
					{
						showMessage(false,"Must be on an variation to copy");
						creaCopyPaste->value(0);
					}
				}
				else //pasting: on an instrument or not
				{
					if(copyType==4 && creaSelectedInst>-1)
					{
						tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->addVariation(copyVar);
						//update the variations
						tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->updateVariations(-1);
						//recreate the tap list
						tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
						creaVarList->add(copyVar->getSample()->getName().c_str());
						creaVarList->value(creaVarList->size());
						creaSelectedVar=creaVarList->size()-1;
						displayVar(creaSelectedVar);
						showMessage(false,"Variation "+copyVar->getSample()->getName()+" pasted");
					}
					else
					{
						delete copyVar;
						pasteError=true;
					}
					creaCopyPaste->label("C");
				}
			}
			break;
		default:
			break;
	}

	if(pasteError)
	{
		std::string errorType;
		switch(copyType)
		{
			case 1:{errorType="the setup list";}break;
			case 2:{errorType="a setup's kits list";}break;
			case 3:{errorType="a kit's instruments list";}break;
			case 4:{errorType="an instrument's variations list";}break;
		}
		showMessage(false,"Must be on "+errorType+" to paste");
	}

}

void execWindow::cbCreaRemove(Fl_Widget*)
{
	fileSaved=false;
	switch(creaActiveList)
	{
		case 0://setup
			if(creaSelectedSetup!=-1)
			{
				//remove the setup
				tap->removeSetup(creaSelectedSetup);
				creaSetupList->remove(creaSelectedSetup+1);
				//if there are setups remaining
				if(creaSetupList->size()>0)
				{
					//if it was the last setup, select another setup
					if(creaSelectedSetup==tap->getNbSetups())
						creaSelectedSetup--;
					//set the new value in the list
					creaSetupList->value(creaSelectedSetup+1);
					displaySetup(creaSelectedSetup);
				}//if there is no setup left
				else
				{
					creaSelectedSetup=-1;
					creaName->value(" ");
					creaKitList->clear();
					creaInstList->clear();
					creaVarList->clear();
				}
			}
			break;
		case 1://kit
			if(creaSelectedKit!=-1)
			{
				//remove the kit
				tap->getSetup(creaSelectedSetup)->removeKit(creaSelectedKit);
				creaKitList->remove(creaSelectedKit+1);
				//if there are kits remaining
				if(creaKitList->size()>0)
				{
					//if it was the last kit, select another kit
					if(creaSelectedKit==tap->getSetup(creaSelectedSetup)->getNbKits())
						creaSelectedKit--;
					//set the new value in the list
					creaKitList->value(creaSelectedKit+1);
					displayKit(creaSelectedKit);
				}//if there is no kit left
				else
				{
					creaSelectedKit=-1;
					creaName->value(" ");
					creaInstList->clear();
				}
			}
			break;
		case 2:
			if(creaSelectedInst!=-1)
			{
				//if there are inst remaining
				if(creaInstList->size()>1)
				{
					//remove the inst
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->removeInstrument(creaSelectedInst);
					creaInstList->remove(creaSelectedInst+1);

					//if it was the last inst, select another inst
					if(creaSelectedInst==tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getNbInstruments())
						creaSelectedInst--;
					//set the new value in the list
					creaInstList->value(creaSelectedInst+1);
					displayInst(creaSelectedInst);

					//recreate the tap list
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
				}
			}
			break;
		case 3:
			if(creaSelectedVar!=-1)
			{
				//if there are var remaining
				if(creaVarList->size()>1)
				{
					//remove the var
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->removeVariation(creaSelectedVar);
					creaVarList->remove(creaSelectedVar+1);

					//if it was the last var, select another var
					if(creaSelectedVar==tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->getNbVariations())
						creaSelectedVar--;
					//set the new value in the list
					creaVarList->value(creaSelectedVar+1);
					//update the variations
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->updateVariations(-1);
					//recreate the tap list
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
					displayVar(creaSelectedVar);
				}
			}
			break;
		default:
			break;
	}
}

void execWindow::cbCreaUp(Fl_Widget*)
{
	fileSaved=false;
	switch(creaActiveList)
	{
		case 0://setup
			{
				//if selectedSetup>0, swap the setup with the previous one
				if(creaSelectedSetup>0)
				{
					setup* prevSetup = tap->getSetup(creaSelectedSetup-1);
					setup* nextSetup = tap->getSetup(creaSelectedSetup);
					tap->setSetup(creaSelectedSetup-1,nextSetup);
					tap->setSetup(creaSelectedSetup,prevSetup);
					creaSetupList->swap(creaSelectedSetup,creaSelectedSetup+1);
					creaSelectedSetup--;
					creaSetupList->value(creaSelectedSetup+1);
				}

			}
			break;
		case 1://kit
			{
				//if selectedKit>0, swap the kit with the previous one
				if(creaSelectedKit>0)
				{
					kit* prevKit = tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit-1);
					kit* nextKit = tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit);
					tap->getSetup(creaSelectedSetup)->setKit(creaSelectedKit-1,nextKit);
					tap->getSetup(creaSelectedSetup)->setKit(creaSelectedKit,prevKit);
					creaKitList->swap(creaSelectedKit+1,creaSelectedKit);
					creaSelectedKit--;
					creaKitList->value(creaSelectedKit+1);
				}
			}
			break;
		case 3://variation
			{
				//if selectedVar>0, swap the var with the previous one
				if(creaSelectedVar>0)
				{
					//move the tap
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->moveVariation(false,creaSelectedVar);
					//recreate the tap list
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
					creaVarList->swap(creaSelectedVar+1,creaSelectedVar);
					creaSelectedVar--;
					creaVarList->value(creaSelectedVar+1);

				}
			}
			break;
		default:break;
	}
}

void execWindow::cbCreaDown(Fl_Widget*)
{
	fileSaved=false;
	switch(creaActiveList)
	{
		case 0://setup
			{
				//swap the setup with the next one
				if(creaSelectedSetup>-1 && creaSelectedSetup<creaSetupList->size()-1)
				{
					setup* prevSetup = tap->getSetup(creaSelectedSetup);
					setup* nextSetup = tap->getSetup(creaSelectedSetup+1);
					tap->setSetup(creaSelectedSetup,nextSetup);
					tap->setSetup(creaSelectedSetup+1,prevSetup);
					creaSetupList->text(creaSelectedSetup+2,prevSetup->getName().c_str());
					creaSetupList->text(creaSelectedSetup+1,nextSetup->getName().c_str());
					creaSelectedSetup++;
					creaSetupList->value(creaSelectedSetup+1);
				}
			}
			break;
		case 1: //kit
			{
				//swap the kit with the next one
				if(creaSelectedKit>-1 && creaSelectedKit<creaKitList->size()-1)
				{
					kit* prevKit = tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit);
					kit* nextKit = tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit+1);
					tap->getSetup(creaSelectedSetup)->setKit(creaSelectedKit,nextKit);
					tap->getSetup(creaSelectedSetup)->setKit(creaSelectedKit+1,prevKit);
					creaKitList->text(creaSelectedKit+2,prevKit->getName().c_str());
					creaKitList->text(creaSelectedKit+1,nextKit->getName().c_str());
					creaSelectedKit++;
					creaKitList->value(creaSelectedKit+1);
				}
			}
		case 3://variation
			{
				//if selectedVar not the last, swap the var with the next one
				if(creaSelectedVar>-1 && creaSelectedVar<creaVarList->size()-1 )
				{
					//move the tap
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->moveVariation(true,creaSelectedVar);
					//recreate the tap list
					tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
					creaVarList->swap(creaSelectedVar+1,creaSelectedVar+2);
					creaSelectedVar++;
					creaVarList->value(creaSelectedVar+1);
				}
			}
			break;
			default:break;
	}
}

void execWindow::cbCreaName(Fl_Widget*)
{
	fileSaved=false;
	switch(creaActiveList)
	{
		case 0:
			if(creaSelectedSetup!=-1)
			{
				tap->getSetup(creaSelectedSetup)->setName(creaName->value());
				creaSetupList->text(creaSelectedSetup+1,creaName->value());
			}
			break;
		case 1:
			if(creaSelectedKit!=-1)
			{
				tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->setName(creaName->value());
				creaKitList->text(creaSelectedKit+1,creaName->value());
			}
			break;
		case 2:
			if(creaSelectedInst!=-1)
			{
				tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setName(creaName->value());
				creaInstList->text(creaSelectedInst+1,creaName->value());
			}
			break;
		default:
			break;
	}
}

void execWindow::cbCreaPlayMode(Fl_Widget* w)
{
	fileSaved=false;
	std::string widgetName(w->label());
	if(widgetName.compare("Play Mode ")==0) {
		if(creaSelectedInst!=-1) {
			tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setPlayMode(creaPlayMode->value());
		}
	}
	else if(widgetName.compare("Loop")==0) {
		if(creaSelectedInst!=-1) {
			tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setPlayLoop(creaPlayLoop->value());
		}
	}
	else if(widgetName.compare("Reverse")==0) {
		if(creaSelectedInst!=-1) {
			tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setPlayReverse(creaPlayReverse->value());
		}
	}
}

void execWindow::cbCreaCut(Fl_Widget*)
{
	fileSaved=false;
	if(creaSelectedInst!=-1)
		tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setCut(creaCut->value());
}

void execWindow::cbCreaPoly(Fl_Widget*)
{
	fileSaved=false;
	if(creaSelectedInst!=-1)
		tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setPolyphony((int)(creaPoly->value()));
}

void execWindow::cbCreaCheckCut(Fl_Widget*)
{
	fileSaved=false;
	if(creaSelectedInst!=-1)
	{
		if(creaCheckCut->value()==1)
		{
			creaCut->activate();
			tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setCut(creaCut->value());

		}
		else
		{
			creaCut->deactivate();
			tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setCut(-1);
		}
	}
}

void execWindow::cbCreaCheckPoly(Fl_Widget*)
{
	fileSaved=false;
	if(creaSelectedInst!=-1)
	{
		if(creaCheckPoly->value()==1)
		{
			creaPoly->activate();
			tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setPolyphony((int)(creaPoly->value()));
		}
		else
		{
			creaPoly->deactivate();
			tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setPolyphony(0);
		}
	}
}

void execWindow::cbCreaCheckPitch(Fl_Widget*)
{
	fileSaved=false;
	if(creaSelectedInst!=-1)
	{
		tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setPitchOverRange(creaCheckPitch->value());
		//recreate the tap list
		tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
	}
}

void execWindow::cbCreaSampleOpen(Fl_Widget*)
{
	fileSaved=false;

	if(creaSelectedVar!=-1)
	{
		std::string previousDir ="";
		instrument *tmpInst = tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst);
		sample* curSample =tmpInst->getVariation(creaSelectedVar)->getSample();
		if(curSample)
		{
			previousDir = curSample->getAbsoluteName();
		}
		//if no sample in this variation
		//try the other variations
		for(unsigned int i=0;i<tmpInst->getNbVariations() && previousDir=="";++i)
		{
			if(tmpInst->getVariation(i)->getSample())
			{
				previousDir = tmpInst->getVariation(i)->getSample()->getAbsoluteName();
			}
		}
		//if still  nothing try the lastSampleDir
		if(previousDir=="")
		{
			previousDir=lastSampleDir;
		}
 		//if still  nothing try tapDir
		if(previousDir=="")
		{
			previousDir=tap->getCompleteFileName();
		}

		//open file dialog
		Fl_Native_File_Chooser *fb = new Fl_Native_File_Chooser();
		fb->title("Open audio file");
		fb->type(Fl_Native_File_Chooser::BROWSE_FILE);
		fb->filter("Audio file\t*.{wav,flac,aiff}");

		string dir = "/home/";
		size_t found = tap->getCompleteFileName().find_last_of("/");
		if(found!=tap->getCompleteFileName().npos)
		{
			dir=tap->getCompleteFileName().substr(0,found+1);
		}
		fb->directory(dir.c_str());
		if (!fb->show()) {

			char* audioFile = (char *)fb->filename();
			char* audioFileR=NULL;
			lastSampleDir=audioFile;

			//cut the file name
			std::string audioFileStr(audioFile);
			size_t found = audioFileStr.find_last_of("/\\");
			audioFileStr = audioFileStr.substr(found+1);
			//display loading
			setStatus("Loading sample: "+audioFileStr);
			creaSampleName->value("");

			//if the var already has a sample, release it
			variation *v = tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->getVariation(creaSelectedVar);
			sample *s = v->getSample();
			//try to load the file
			if(v->setSample(tap->addSample(audioFile,audioFileR)))
			{
				v->setSample(s);
				displayVar(creaSelectedVar);
			}
			else
			{
				setStatus("Sample loaded: "+audioFileStr);
				creaVarList->text(creaSelectedVar+1,audioFileStr.c_str());
				displayVar(creaSelectedVar);
				if(s)
				{
					tap->removeSample(s);
				}
			}
			delete [] audioFile;
		}
		delete fb;
	}
}

//-----------------------------------------AUDIO TAB--------------------------------------------------------
void execWindow::cbCreaVolume(Fl_Widget*)
{
	fileSaved=false;
	switch(creaActiveList)
	{
		case 1:
			{
				tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->setVolume(creaVolume->value());
				//recreate the tap list
				tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());

			}
			break;
		case 2:
			{
				tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setVolume(creaVolume->value());
				//recreate the tap list
				tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
			}
			break;
		default:break;
	}

}

void execWindow::cbCreaPan(Fl_Widget*)
{
	fileSaved=false;
	if(creaSelectedInst!=-1)
	{
		tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setPan(creaPan->value());
		//recreate the tap list
		tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
	}
}

void execWindow::cbCreaAudioOutput(Fl_Widget*)
{
	fileSaved=false;
	if(creaSelectedInst!=-1)
	{
		tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setJackStereoChannel(creaAudioOutput->value());
	}
}

//---------------------------------------------MIDI TAB-----------------------------------------------

void execWindow::cbCreaMidiSel(Fl_Widget*)
{
	fileSaved=false;
	//changer le creaMidiType
}

void execWindow::cbCreaMidiAct(Fl_Widget*)
{
	fileSaved=false;
	if(creaMidiAct->value())
	{
		creaMidiCC->activate();
		creaMidiChan->activate();
		creaMidiLearn->activate();
	}
	else
	{
		creaMidiCC->deactivate();
		creaMidiChan->deactivate();
		creaMidiLearn->deactivate();
		creaMidiCC->value(-1);
		creaMidiChan->value(-1);
	}

	updateMidi();

	this->redraw();
	Fl::check();
}

void execWindow::cbCreaMidiCC(Fl_Widget*)
{
	updateMidi();
}

void execWindow::cbCreaMidiChan(Fl_Widget*)
{
	updateMidi();
}

void execWindow::cbCreaMidiLearn(Fl_Widget*)
{
	if(creaMidiLearn->value())
	{
		creaMidiCnt=creaMidiType;
		switch(creaMidiType)
		{
			case 0: //channel,control
				{
					setStatus("Midi Learn: Tweak the controller you want to use");
				}
				break;
			case 1: //min,max
				{
					setStatus("Midi Learn: Play the Minimum Velocity");
				}
				break;
			case 2: //min,root,max
				{
					setStatus("Midi Learn: Play the Minimum Note");
				}
				break;
			default:break;
		}
		tap->startMidiLearn();
	}
	else
	{
		setStatus("Midi Learn: Stopped");
		tap->stopMidiLearn();
	}

}

void execWindow::learnMidi(int ch,int ccn,int vel)
{
	//tester
	switch(creaMidiType)
	{
		case 0: //channel,control
			{
				creaMidiLearn->value(0);
				creaMidiCC->value(ccn);
				creaMidiChan->value(ch);
				creaMidiCC->redraw();
				creaMidiChan->redraw();
				setStatus("Midi Learn: Done");
			}
			break;
		case 1: //min,max
			{
				if(creaMidiCnt>0)
				{
					creaMidiMin->value(vel);
					creaMidiCnt=0;
					setStatus("Midi Learn: Play the Maximum Velocity");
					tap->startMidiLearn();
				}
				else
				{
					creaMidiLearn->value(0);
					creaMidiMax->value(vel);
					setStatus("Midi Learn: Done");
					creaMidiMin->redraw();
					Fl::check();
				}
			}
			break;
		case 2: //min,root,max
			{
				switch(creaMidiCnt)
				{
					case 2:
					{
						creaMidiMin->value(ccn);
						creaMidiPiano->setLSelect(ccn);
						creaMidiChan->value(ch);
						creaMidiCnt=1;
						setStatus("Midi Learn: Play the Root Note");
						tap->startMidiLearn();
					}
					break;
					case 1:
					{
						creaMidiRoot->value(ccn);
						creaMidiPiano->setMSelect(ccn);
						creaMidiCnt=0;
						setStatus("Midi Learn: Play the Maximum Note");
						tap->startMidiLearn();
					}
					break;
					case 0:
					{
						creaMidiMax->value(ccn);
						creaMidiPiano->setRSelect(ccn);
						setStatus("Midi Learn: Done");
						creaMidiLearn->value(0);
					}
					break;
					default:break;
				}
			}
			break;
		default:break;
	}

	updateMidi();
}

void execWindow::setGlobalVolume(double gv)
{
	setVolume->value(gv);
}

void execWindow::cbCreaMidiMin(Fl_Widget*)
{
	creaMidiPiano->setLSelect((int)(creaMidiMin->value()));
	updateMidi();
}

void execWindow::cbCreaMidiRoot(Fl_Widget*)
{
	creaMidiPiano->setMSelect((int)(creaMidiRoot->value()));
	updateMidi();
}

void execWindow::cbCreaMidiRootFine(Fl_Widget*)
{
	updateMidi();
}

void execWindow::cbCreaMidiMax(Fl_Widget*)
{
	creaMidiPiano->setRSelect((int)(creaMidiMax->value()));
	updateMidi();
}

void execWindow::cbCreaMidiPiano(Fl_Widget*)
{
	creaMidiMin->value(creaMidiPiano->getLSelect());
	creaMidiMax->value(creaMidiPiano->getRSelect());
	creaMidiRoot->value(creaMidiPiano->getMSelect());
	updateMidi();
}

void execWindow::updateMidi()
{
	fileSaved=false;
	switch(creaActiveList)
	{
		case 0://setups list
			{
				if(creaSelectedSetup!=-1)
				{
					switch((int)(creaMidiSel->value()))
					{
						case 0://change kit
							{
								tap->getSetup(creaSelectedSetup)->setCC((short)(creaMidiChan->value()),(short)(creaMidiCC->value()));
							}
							break;
						default:break;
					}
				}
			}
			break;
		case 1://kits list
			{
				if(creaSelectedKit!=-1)
				{
					switch((int)(creaMidiSel->value()))
					{
						case 0://change
							{
							}
							break;
					}
				}
			}
			break;
		case 2://instruments list
			{
				if(creaSelectedInst!=-1)
				{
					switch((int)(creaMidiSel->value()))
					{
						case 0://play sample
							{
								tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->setMidi(0,(short)(creaMidiChan->value()),(short)(creaMidiMin->value()),(short)(creaMidiRoot->value()),(short)(creaMidiMax->value()),creaMidiRootFine->value());
								//recreate the tap list
								tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
							}
							break;
						default: //else
							break;
					}
				}
			}
			break;
		case 3://variations list
			{
				if(creaSelectedVar!=-1)
				{
					switch((int)(creaMidiSel->value()))
					{
						case 0://velocity range
							{
								tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->getVariation(creaSelectedVar)->setMidi(0,(short)(creaMidiChan->value()),(short)(creaMidiMin->value()),(short)(creaMidiMax->value()));
								//update the variations
								tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->getInstrument(creaSelectedInst)->updateVariations(creaSelectedVar);
								//recreate the tap list
								tap->getSetup(creaSelectedSetup)->getKit(creaSelectedKit)->exec(tap->getGlobalVolume());
							}
							break;
						default: //else
							break;
					}
				}
			}
			break;
		default:break;
	}
}

//------------------------------EXECUTION-------------------------------------
void execWindow::updateExecTab()
{
	//erase the execution  lists
	if(setupLists)
	{
		for(int i=0;i<tap->getNbSetups();++i)
		{
			execTab->remove(setupLists[i]);
			delete setupLists[i];
		}
		delete [] setupLists;
		setupLists=NULL;
	}

	//create the lists of setups int the exec Tab
	if(tap->getNbSetups()>0)
	{
		setupLists = new Flat_Hold_Browser*[tap->getNbSetups()];
		int width = w() / tap->getNbSetups();
		int y = MENUBAR_HEIGHT+TAB_HEIGHT+30;
		std::string name;
		for(int i=0;i<tap->getNbSetups();++i)
		{
			name=tap->getSetup(i)->getName();
			setupLists[i] = new Flat_Hold_Browser(i*width,y,width,h()-y-MENUBAR_HEIGHT,"");
			setupLists[i]->align(FL_ALIGN_TOP|FL_ALIGN_CENTER);
			setupLists[i]->callback(statExecSetupList,this);
			setupLists[i]->copy_label(name.c_str());
			for(int j=0;j<tap->getSetup(i)->getNbKits();++j)
			{
				std::string kitName = tap->getSetup(i)->getKit(j)->getName();
				setupLists[i]->add(kitName.c_str());
			}
			setupLists[i]->select(1);
			execPack->insert(*setupLists[i],i+1);
		}

	}
	Fl::check();
}

void execWindow::cbExecSetupList(Fl_Widget* w)
{
	//get the number of the setup which list was clicked
	int s = execTab->find(w);

	if(s>=0 && s<tap->getNbSetups())
	{
		Flat_Hold_Browser* hb = (Flat_Hold_Browser*)w;
		hb->value(tap->getSetup(s)->changeKit(hb->value()-1)+1);
	}
}



void execWindow::changeKit(int setup,int kit)
{
	for(int i=1;i<setupLists[setup]->size();++i)
		setupLists[setup]->deselect();
	setupLists[setup]->select(kit);
	setupLists[setup]->redraw();
	Fl::check();
}


void execWindow::setVisible(bool v)
{
	if (v != visible) {
		visible = v;

		if (visible) {
			show();
		} else {
			while ( Fl::first_window() ) Fl::first_window()->hide();
		}
	}
}
