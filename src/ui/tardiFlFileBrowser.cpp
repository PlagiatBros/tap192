/***************************************************************************
 *            tardiFlFileBrowser.cpp
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

#include "tardiFlFileBrowser.h"
#include <iostream>

using namespace std;


tardiFlFileBrowser::tardiFlFileBrowser():Fl_Window(330,290,"Open File"),showHidden(false),currentDir(""),currentPattern(""),result(NULL)
{
	//better look
	Fl::scheme("gtk+");
	//modal window
	set_modal();

	//navigation buttons
	prevButton = new Fl_Button(10,10,15,25,"@<");
	prevButton->callback(statPrevButton,this);
	prevButton->deactivate();
	this->insert(*prevButton,1);
	nextButton = new Fl_Button(270,10,15,25,"@>");
	nextButton->callback(statNextButton,this);
	nextButton->deactivate();
	this->insert(*nextButton,2);
	pathButtonsGroup = new Fl_Group(30,5,235,30);
	this->insert(*pathButtonsGroup,0);

	//file list
	filesList = new Fl_Hold_Browser(10,40,280,180);
	filesList->callback(statList,this);
	this->insert(*filesList,3);
	
	//add / hide dirs
	hiddenButton = new Fl_Toggle_Button(300,40,20,20,"h");
	hiddenButton->callback(statHidden,this);
	hiddenButton->tooltip("Show/Hide hidden files and directories");
	this->insert(*hiddenButton,4);
	addButton = new Fl_Button(300,70,20,20,"@+");
	addButton->tooltip("Create a directory");
	addButton->callback(statAdd,this);
	this->insert(*addButton,5);
	
	//name input
	fileName = new Fl_Input(90,230,200,20,"Filename ");
	fileName->callback(statFileName,this);
	fileName->when(FL_WHEN_CHANGED);
	this->insert(*fileName,6);

	//ok cancel buttons
	okButton = new Fl_Return_Button(190,260,60,25,"OK");
	okButton->callback(statOk,this);
	okButton->deactivate();
	this->insert(*okButton,7);
	cancelButton = new Fl_Button(260,260,60,25,"Cancel");
	cancelButton->callback(statCancel,this);
	this->insert(*cancelButton,8);
}

tardiFlFileBrowser::~tardiFlFileBrowser()
{
	delete filesList;
	delete nextButton;
	delete prevButton;
	delete hiddenButton;
	delete addButton;
	delete okButton;
	delete cancelButton;
	delete fileName;
	
}

void tardiFlFileBrowser::cbPrevButton(Fl_Widget*)
{
	displayCurrentPath(-1);
}

void tardiFlFileBrowser::cbNextButton(Fl_Widget*)
{
	displayCurrentPath(1);
}

void tardiFlFileBrowser::cbPathButton(Fl_Widget* w)
{
	//keep only this button down
	int buttonID = pathButtonsGroup->find(w);
	for(unsigned int i=0;i<pathButtons.size();++i)
	{
		pathButtons[i]->value(0);
	}
	pathButtons[buttonID]->value(1);


	//find to the corresponding directory
	std::string newDir="/";
	for(unsigned int i=1;i<buttonID+1;++i)
	{
		newDir+=std::string(pathButtons[i]->label());
		newDir+="/";
	}
	//and display it
	currentDir=newDir;
	displayCurrentDir();
}

void tardiFlFileBrowser::cbList(Fl_Widget*)
{
	//deactivates the ok button
	okButton->deactivate();
	fileName->value("");				
	if(filesList->value()>0)
	{
		//get the absolute name
		std::string absFn = currentDir+std::string(filesList->text(filesList->value()));
		//if it's a directory
		if(fl_filename_isdir(absFn.c_str()))
		{
			//if double-click
			if((Fl::event()==FL_PUSH || Fl::event()==FL_RELEASE)&& Fl::event_clicks()==1)
			{
				//open this directory
				currentDir=absFn;
				displayCurrentDir();
				//set the new path
				storeCurrentPath();
				displayCurrentPath(0);
				Fl::event_clicks(0);
			}
		}
		else //if it's a file
		{
			if(Fl::event()==FL_PUSH || Fl::event()==FL_RELEASE)
			{
				//if double-click
				if(Fl::event_clicks()==1)
				{
					//set the result and close the browser
					result=strdup(absFn.c_str());
					this->hide();	
					Fl::event_clicks(0);
				}
				else
				{
					//set the name in the input
					fileName->value(filesList->text(filesList->value()));				
					//activates the ok button
					okButton->activate();
				}
			}
		}
	}
}

void tardiFlFileBrowser::cbFileName(Fl_Widget*)
{
	okButton->deactivate();
	std::string fn = fileName->value();
	if(fl_filename_match((currentDir+fn).c_str(),pattern.c_str()))
	{
		okButton->activate();
	}
}

void tardiFlFileBrowser::cbOk(Fl_Widget*)
{
	std::string sResult = currentDir+std::string(fileName->value()); 
        result=strdup(sResult.c_str());
	this->hide();
}

void tardiFlFileBrowser::cbCancel(Fl_Widget*)
{
	result=NULL;
	this->hide();
}

void tardiFlFileBrowser::cbAdd(Fl_Widget*)
{
	const char * res = fl_input("Enter the new directory's name");
	if(res)
	{
		std::string dirN=res;
		std::system(("mkdir "+currentDir+"/"+dirN).c_str());
		displayCurrentDir();
	}
}

void tardiFlFileBrowser::cbHidden(Fl_Widget*)
{
	showHidden = hiddenButton->value();	
	displayCurrentDir();
}

void tardiFlFileBrowser::open(std::string browserT,std::string fileX,std::string previousFile)
{

	//sets the label
	label(browserT.c_str());

	//sets currentDir
	//if not null, go to the asked directory
	size_t found = previousFile.find_last_of("/");
	if(found!=previousFile.npos)
	{
		currentDir=previousFile.substr(0,found+1);
	}
	else//go to the user directory
	{
		currentDir="/home/";		
	}

	//sets currentPattern
	pattern=fileX;
	currentPattern=pattern;

	if(displayCurrentDir())
	{
		currentDir="/home/";
		displayCurrentDir();
	}

	storeCurrentPath();
	displayCurrentPath(0);

	show();
}

int tardiFlFileBrowser::displayCurrentDir()
{
	filesList->clear();

	dirent **files;
	int nbFiles = scandir(currentDir.c_str(),&files,NULL,alphasort);
	//if problem, retry with the user directory 
	if(nbFiles<0)
	{
		return -1;
	}
	else
	{
		for(int i=0;i<nbFiles;++i)
		{
			std::string fn = files[i]->d_name;
			//skip the current and parent dirs
			if(fn!="." && fn!="..")
			{
				//show/hide hidden files/dirs
				if(!fl_filename_match(fn.c_str(),".*") || showHidden)
				{
					//get the absolute name
					std::string absFn = currentDir+fn;
					//show dirs
					if(fl_filename_isdir(absFn.c_str()))
					{
						filesList->add((fn+"/").c_str());
					}
					else//for other files
					{
						//match patterns
						if(fl_filename_match(fn.c_str(),currentPattern.c_str()))
						{
							filesList->add(fn.c_str());
						}
					}
				}
			}
		}
	}

	return 0;	
}


void tardiFlFileBrowser::storeCurrentPath()
{
	//remove previous buttons
	for(unsigned int i=0;i<pathButtons.size();++i)
	{
		pathButtonsGroup->remove(pathButtons[i]);
		delete pathButtons[i];
	}
	pathButtons.clear();

	//create buttons for all the directories in the path
	std::string cutPath = currentDir;
	size_t found = cutPath.find_first_of("/");
	int cpt=0;
	while(found!=cutPath.npos)
	{
		std::string cut = cutPath.substr(0,found+1);
		if(cut!="/")
		{
			cut = cut.substr(0,cut.size()-1);
		}

		Fl_Toggle_Button *newPathButton = new Fl_Toggle_Button(0,25,cut.size()*15,20,"");
		newPathButton->copy_label(cut.c_str());
		newPathButton->callback(statPathButton,this);
		newPathButton->hide();
		pathButtons.push_back(newPathButton);
		pathButtonsGroup->insert(*newPathButton,cpt);
		cutPath = cutPath.substr(found+1);
		found = cutPath.find_first_of("/");
		++cpt;
	}
	//select the current directory
	pathButtons[cpt-1]->value(1);
}

void tardiFlFileBrowser::displayCurrentPath(int pos)
{
	
	int currentPathPosition=pathButtons.size()-1;
	//find the latest shown button
	for(int i=pathButtons.size()-1;i>=0;--i)
	{
		if(pathButtons[i]->visible())
		{
			currentPathPosition=i;
			break;
		}
	}


	//0:display the last button,-1:show from the previous button,+1:show from the next button
	if(currentPathPosition+pos<pathButtons.size() && currentPathPosition+pos>=0)
	{
		currentPathPosition+=pos;
	}
	//get the number of buttons we can display
	int cpt=0;
	int s=0;
	bool max=false;
	for(int i=currentPathPosition;i>=0 && (!max);--i)
	{
		if(s+pathButtons[i]->w()+5<pathButtonsGroup->w() || cpt<1)
		{
			s+=pathButtons[i]->w()+5;
			++cpt;
		}
		else
		{
			max=true;
		}
	}

	//the last button is currentPathPosition
	//the first one is
	int firstButton=currentPathPosition+1-cpt;

	//display them, hide the others
	for(int i=0;i<pathButtons.size();++i)
	{
		pathButtons[i]->hide();
	}
	s=0;
	for(int i=firstButton;i<=currentPathPosition;++i)
	{
		pathButtons[i]->position(pathButtonsGroup->x()+s,pathButtonsGroup->y()+7);
		pathButtons[i]->show();
		s+=pathButtons[i]->w()+5;
	}

	//(de)activate the navigation buttons
	if(firstButton>0)
	{
		prevButton->activate();
	}
	else
	{
		prevButton->deactivate();
	}

	if(currentPathPosition<pathButtons.size()-1)
	{
		nextButton->activate();
	}
	else
	{
		nextButton->deactivate();
	}


	this->redraw();
	Fl::check();
	
}

char* tardiFlFileBrowser::getResult()
{
	return result;
}
