/***************************************************************************
 *            tapeutape.cpp
 *
 *  Copyright  2005  Florent Berthaut
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "tapeutape.h"

#include "../nsm/nsm.h"
extern nsm_client_t *nsm;

using namespace std;

tapeutape::tapeutape(int argc,char** argv):polyphony(100),globalVolume(1.0),fileName(""),jack(NULL),midi(NULL),eventsRingBuffer(NULL),loop(true)
{
	fileName="";

	#ifdef WITH_GUI
		//start the ui
		execWin = new execWindow(350,400,"Tapeutape",this);
		execWin->size_range(350,400,0,0,0,0);
		execWin->resizable(execWin);
	#endif

	//ring buffer
	eventsRingBuffer = jack_ringbuffer_create(RING_BUFFER_SIZE);

	//jackProcess , initialised here to get the samplerate to load the files
	jack = new jackProcess(this,eventsRingBuffer,polyphony);
	if(jack->init())
	{
		showMessage(true,"Error Initialiasing Jack Client");
	}


	//midiProcess init and start
	midi = new midiProcess(this,jack->getClient(),MIDI_PRIORITY,eventsRingBuffer);
	showMessage(false,"Starting MIDI");
	if(midi->midiInit())
	{
		showMessage(true,"Error Starting Midi ");
	}

	//if a file name was specified
	if(argc==2)
		load(argv[1]);

	//start
	start();
	midi->startThread();

	//if everything's ok
	showMessage(false,"Let's play !! ");

	#ifndef WITH_GUI
	while(loop)
	{
		showMessage(false,"Menu :");
		showMessage(false,"1 : Open File");
		showMessage(false,"2 : Reset/Reload File");
		showMessage(false,"3 : Quit");
		int choice;
		while(!(cin>>choice) || choice<1 || choice>3)
		{
			if ( cin.fail() )
			{
			    cin.clear();
			    cin.ignore( numeric_limits<streamsize>::max(), '\n' );
			}
			showMessage(false,"Incorrect Choice");
			showMessage(false,"Menu :");
			showMessage(false,"1 : Open File");
			showMessage(false,"2 : Reset/Reload File");
			showMessage(false,"3 : Quit");
		}
		if(choice==1)
		{
			std::string f;
			showMessage(false,"Enter the file name:");
			cin>>f;
			size_t size = f.size() + 1;
			char * cf = new char[ size ];
			strncpy( cf, f.c_str(), size );
			load(cf);
			delete [] cf;
			start();
		}
		else if(choice==2)
		{
			std::string f = getCompleteFileName();
			size_t size = f.size() + 1;
			char * cf = new char[ size ];
			strncpy( cf, f.c_str(), size );
			load(cf);
			start();
			delete [] cf;
		}
		else if(choice==3)
		{
			stop();
			loop=false;
		}
	}
	#endif
}


tapeutape::~tapeutape()
{
	jack_ringbuffer_free(eventsRingBuffer);
}



void tapeutape::showMessage(bool t,std::string mess)
{
	#ifdef WITH_GUI
		Fl::lock();
		execWin->showMessage(t,mess);
		Fl::unlock();
	#else
		cout<<"<Tapeutape> "<<mess<<endl;
	#endif
}

std::string tapeutape::getFileName()
{
	size_t found = fileName.find_last_of("/\\");
	return fileName.substr(found+1);
}

std::string tapeutape::getCompleteFileName()
{
	return fileName;
}

int tapeutape::load(char * nomfic)
{

	execWin->reset();

	//delete the setups/kits/instruments/variations
	for(unsigned int i=0;i<setups.size();++i)
	{
		delete setups[i];
	}
	setups.clear();

	//delete the samples
	for(unsigned int i=0;i<samples.size();++i)
	{
		delete samples[i];
	}
	samples.clear();

	//delete the outputs
	jackStereoChannelsNames.clear();

	fileName=nomfic;

	// File Open
	tapParser parse(this);
	if(parse.createTree(nomfic))
		return -1;
	showMessage(false,"Loading "+getFileName());
	if(parse.parseTree())
	{
		showMessage(true,"Error Parsing "+getFileName());
		return -1;
	}

	return 0;
}

void tapeutape::import(char* dir)
{
	/*
	std::string dirStr(dir);

	//test if it is a directory
	if(fl_filename_isdir(dirStr.c_str())) {
		//create a default setup/kit/instru ...
		setup* setup1 = new setup();
		addSetup(setup1);
		kit* kit1=NULL;
		instrument* instru1=NULL;
		//parse
		ostringstream oss,oss2;
		DIR *dp2;
		struct dirent *dirp2;
		if((dp2  = opendir((dirStr).c_str())) != NULL) {
			while ((dirp2 = readdir(dp2)) != NULL) {
				if(dirp2->d_name[0]!='.') { //if not a hidden file
					if(!fl_filename_isdir(dirp2->d_name)) { //if it's a file, try to add it as an instrument
						sample* newsample = addSample(dirp2->d_name);
						kit* kit1 = new kit();
						setup1->addKit(kit1);
						instrument* instru1 = new instrument();
						kit1->addInstrument(instru1);

					}
					else { //if it is a directory, create a new kit


					}
				}
			}
		}

	}
*/
}

int tapeutape::start()
{

	//test if we created at least one jack-output
	if(jackStereoChannelsNames.size()==0)
	{
		jackStereoChannelsNames.push_back("output");
	}

	//jackProcess , real start
	if(jack->start())
	{
		showMessage(true,"Error starting jack client");
	}

	//create the array of taps for each setup
	createTaps();

	//(re)init lash
	#ifdef WITH_LASH
		sleep(2);
		lashC->setAlsa(midi->getAlsaID());
		lashC->setJack();
	#endif

	//(re)init the gui
	#ifdef WITH_GUI
		Fl::lock();
		execWin->init();
		execWin->setTitle("Tapeutape : "+getFileName());
		Fl::unlock();
	#endif

	return 0;
}

void tapeutape::save(char* f)
{
	showMessage(false,"Now saving...");
	fileName=f;
	tapParser tap(this);
	if(!tap.saveToFile(f))
	{
		#ifdef WITH_GUI
			Fl::lock();
			execWin->setTitle("Tapeutape : "+getFileName());
			Fl::unlock();
		#endif
		for(unsigned int s=0;s<samples.size();++s)
		{
			samples[s]->processFileName(samples[s]->getAbsoluteName(),fileName);
		}
	}
	showMessage(false,"Finished!");
}

void tapeutape::saveWithSamples(char* f,const char* cpath)
{
	fileName=f;
	std::string path = cpath;

	//save the samples
	for(unsigned int s=0;s<samples.size();++s)
	{
		//get the new filename, according to the tap path
		std::string sampleName;
		std::ostringstream oss;
		oss << s;
		sampleName ="sample"+oss.str();
		size_t found = samples[s]->getAbsoluteName().find_last_of(".\\");
		std::string sampleExt = samples[s]->getAbsoluteName().substr(found+1);
		std::string sampleCompleteName = path+"/"+sampleName+"."+sampleExt;
		//if the sample wasn't already saved with the tap
		if(samples[s]->getAbsoluteName()!=sampleCompleteName)
		{
			//copy and rename the sample
			std::string cmd = "cp ";
			cmd += samples[s]->getAbsoluteName();
			cmd +=" ";
			cmd +=sampleCompleteName;
			std::system(cmd.c_str());

			//change the fileName
			samples[s]->processFileName(sampleCompleteName,fileName);
		}
	}

	//save the tap file
	tapParser tap(this);
	if(!tap.saveToFile(f))
	{
		//set the new name
		#ifdef WITH_GUI
			Fl::lock();
			execWin->setTitle("Tapeutape : "+getFileName());
			Fl::unlock();
		#endif
	}
}

void tapeutape::stop()
{
	//stops the midi thread
	if(midi)
	{
		midi->stopThread();
		delete midi;
		midi=NULL;
	}

	//stops the jack client
	if(jack)
	{
		delete jack;
		jack=NULL;
	}

	if(eventsRingBuffer)
	{
		delete eventsRingBuffer;
		eventsRingBuffer=NULL;
	}

	//delete the jack stereo channels
	jackStereoChannelsNames.clear();

	//delete the setups
	for(unsigned int i=0;i<setups.size();++i)
		delete setups[i];
	setups.clear();

	//delete the samples
	for(unsigned int i=0;i<samples.size();++i)
		delete samples[i];
	samples.clear();
}

void tapeutape::createTaps()
{
	//create the array of taps for each setup
	for(unsigned int i=0;i<setups.size();++i)
	{
		for(unsigned int j=0;j<setups[i]->getNbKits();++j)
		{
			setups[i]->getKit(j)->exec(globalVolume);
		}
	}
}

void tapeutape::setPolyphony(int pol)
{
	polyphony = pol;
}

int tapeutape::getPolyphony()
{
	return polyphony;
}

int tapeutape::getNbSetups()
{
	return setups.size();
}

setup* tapeutape::getSetup(int ind)
{
	if(ind>=0 && ind<(int)(setups.size()))
		return setups[ind];
	else
		return NULL;
}

void tapeutape::setSetup(int ind,setup* s)
{
	setups[ind]=s;
}


void tapeutape::addSetup(setup* s)
{
	setups.push_back(s);
}

void tapeutape::removeSetup(int i)
{
	delete(setups[i]);
	setups.erase(setups.begin()+i);
}

int tapeutape::getNbSamples()
{
	return samples.size();
}

sample* tapeutape::getSample(int ind)
{
	if(ind>=0 && ind<(int)(samples.size()))
		return samples[ind];
	else
		return NULL;
}

sample* tapeutape::addSample(char* n,char *rn)
{
	std::string sampleName(n);
	sample *addedSample=NULL;
	//if there is a sample
	if(sampleName!="")
	{
		//test if the sample has already been added
		for(unsigned int i=0;i<samples.size();++i)
		{
			if(samples[i]->getAbsoluteName()==sampleName)
			{
				addedSample=samples[i];
				samples[i]->addUser();
			}
		}
		//if no, add it
		if(addedSample==NULL)
		{
			addedSample = new sample();
			if(addedSample->tryLoad(n,rn,fileName))
			{
				delete addedSample;
				addedSample=NULL;
				showMessage(true,"Problem loading sample "+sampleName);
			}
			else
			{
				//resample if needed
				addedSample->setSampleRate(jack->getSampleRate());
				//add user to current sample
				addedSample->addUser();
				samples.push_back(addedSample);
			}
		}
	}
	return addedSample;
}

void tapeutape::removeSample(sample* s)
{
	//if more than one user left, just remove the user
	if(s->getNbUsers()>1)
	{
		s->removeUser();
	}
	else //else look for the sample in the vector and remove it completely
	{
		for(unsigned int i=0;i<samples.size();++i)
		{
			if(samples[i]->getAbsoluteName()==s->getAbsoluteName())
			{
				delete(samples[i]);
				samples.erase(samples.begin()+i);
				break;
			}
		}
	}
}

void tapeutape::setSampleRate(int sr)
{
/*
	for(unsigned int i=0;i<setups.size();++i)
		for(unsigned int j=0;j<setups[i]->getNbKits();++j)
			for(unsigned int k=0;k<setups[i]->getKit(j)->getNbInstruments();++k)
				for(unsigned int l=0;l<setups[i]->getKit(j)->getInstrument(k)->getNbVariations();++l)
					setups[i]->getKit(j)->getInstrument(k)->getVariation(l)->getSample()->setSampleRate(sr);
*/

	for(unsigned int s=0;s<samples.size();++s)
	{
		samples[s]->setSampleRate(sr);
	}
}

jackProcess* tapeutape::getJackProcess()
{
	return jack;
}

midiProcess* tapeutape::getMidiProcess()
{
	return midi;
}

void tapeutape::addJackStereoChannel(std::string n)
{
	jackStereoChannelsNames.push_back(n);
}

void tapeutape::addCreateJackStereoChannel(std::string n)
{
	jackStereoChannelsNames.push_back(n);
	jack->addPort(n);
}

void tapeutape::removeJackStereoChannel(int ind)
{
	if(jackStereoChannelsNames.size()>1)
	{
		jackStereoChannelsNames.erase(jackStereoChannelsNames.begin()+ind);
		//for each instrument, check if the number of the output is still available
		//if it isn't , give it the last output
		for(unsigned int i=0;i<setups.size();++i)
		{
			for(unsigned int j=0;j<setups[i]->getNbKits();++j)
			{
				for(unsigned int k=0;k<setups[i]->getKit(j)->getNbInstruments();++k)
				{
					int outNum = setups[i]->getKit(j)->getInstrument(k)->getJackStereoChannel();
					if(outNum>ind)
					{
						setups[i]->getKit(j)->getInstrument(k)->setJackStereoChannel(outNum-1);
					}
				}
			}
		}
		jack->removePort(ind);
	}
}

int tapeutape::getNbJackStereoChannels()
{
	return jackStereoChannelsNames.size();
}

std::string tapeutape::getJackStereoChannelName(int ind)
{
	return jackStereoChannelsNames[ind];
}

void tapeutape::setJackStereoChannelName(int ind,std::string n)
{
	jackStereoChannelsNames[ind]=n;
	jack->renamePort(ind,n);
}

int tapeutape::getJackStereoChannel(std::string n)
{
	for(unsigned int i=0;i<jackStereoChannelsNames.size();++i)
	{
		if(n == getJackStereoChannelName(i))
			return i;
	}
	return -1;
}

void tapeutape::startMidiLearn()
{
	midi->startMidiLearn();
}

void tapeutape::stopMidiLearn()
{
	midi->stopMidiLearn();
}

void tapeutape::processMidiLearn(int ch,int ccn,int vel)
{
	#ifdef WITH_GUI
		Fl::lock();
		execWin->learnMidi(ch,ccn,vel);
		Fl::unlock();
	#endif
}

void tapeutape::setSetupCC(short chan,short cc)
{
	setupChannel = chan;
	setupCC = cc;
}

short tapeutape::getSetupChannel()
{
	return setupChannel;
}

short tapeutape::getSetupCC()
{
	return setupCC;
}

void tapeutape::processCC(unsigned short chan, unsigned short cc,unsigned short val)
{
	//test if it changes one of the setups
	for(unsigned int i=0;i<setups.size();++i)
	{
		if(setups[i]->getChannel()-1 == chan && setups[i]->getCC() == cc && cc < 126)
		{
			int kit = setups[i]->changeKit(val);


			#ifdef WITH_GUI
				Fl::lock();
				execWin->changeKit(i,kit+1);
				Fl::unlock();
			#endif
		}

		// CC 126 & 127 (whatever the channel) reserved for Reverse mode toggle
		if(cc > 125)
		{
			int cK = setups[i]->getCurrentKit();

			// CC 126 set the instrument(s) bound to note "val" in Reverse mode
			// CC 127 set the instrument(s) bound to note "val" in Forward mode
			for(unsigned int j=0;j<setups[i]->getKit(cK)->getNbInstruments();j++)
			{
				int iChan = setups[i]->getKit(cK)->getInstrument(j)->getMidiChannel();
				int iMaxN = setups[i]->getKit(cK)->getInstrument(j)->getMaxNote();
				int iMinN = setups[i]->getKit(cK)->getInstrument(j)->getMinNote();
				if(iChan-1 == chan && val <= iMaxN && val >= iMinN)
				{
					setups[i]->getKit(cK)->getInstrument(j)->setPlayReverse(127-cc);
				}
			}
		}

	}
}

void tapeutape::processPC(unsigned short chan, unsigned short pc)
{
	//test if it changes one of the setups
	int ipc = static_cast <int>(pc);
	for(unsigned int i=0;i<setups.size();++i)
	{
		if(setups[i]->getChannel()-1 == chan && ipc < setups[i]->getNbKits()+1)
		{
			int kit = setups[i]->changeKit(pc);

			#ifdef WITH_GUI
				Fl::lock();
				execWin->changeKit(i,kit+1);
				Fl::unlock();
			#endif
		}
	}
}


void tapeutape::setGlobalVolume(double gv)
{
	globalVolume = gv;
}

double tapeutape::getGlobalVolume()
{
	return globalVolume;
}

bool tapeutape::isDirty()
{
	return execWin->isDirty();
}

bool tapeutape::isVisible()
{
	return execWin->isVisible();
}

void tapeutape::setVisible(bool v)
{
	execWin->setVisible(v);
	if (v) execWin->setTitle("Tapeutape : "+getFileName());
}
