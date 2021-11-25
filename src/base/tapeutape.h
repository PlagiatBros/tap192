/***************************************************************************
 *            tapeutape.h
 *
 *  Copyright  2006  Florent Berthaut
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


#ifndef tapeutape_h
#define tapeutape_h

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <cstring>
#include <jack/jack.h>
#include <jack/ringbuffer.h>
#include <limits>

#include "tapParser.h"
#include "kit.h"
#include "instrument.h"
#include "variation.h"
#include "setup.h"
#include "../midi/midiProcess.h"
#include "../audio/sample.h"
#include "../audio/jackProcess.h"

#ifdef WITH_GUI
	#include "../ui/execWindow.h"
#endif

#ifdef WITH_LASH
	#include "../lash/lashClient.h"
#endif


#define RING_BUFFER_SIZE 1024
#define MIDI_PRIORITY 60

#define TAPEUTAPE_VERSION "0.1.2"

class tapeutape
{
	public:
		tapeutape(int,char**);
		~tapeutape();
		void showMessage(bool,std::string);

		int load(char*);
		int start();
		void save(char*);
		void saveWithSamples(char*,const char*);
		void stop();
		void import(char*);

		int getNbKits();
		kit *getKit(int);
		void addKit(kit*);
		void removeKit(int);
		kit* getKitByName(char*);
		int getNbSetups();
		setup *getSetup(int);
		void addSetup(setup*);
		void removeSetup(int);
		void setSetup(int ,setup* );
		int getNbSamples();
		sample *getSample(int);
		sample* addSample(char*,char*);
		void removeSample(sample*);
		void setSetupCC(short,short);
		short getSetupChannel();
		short getSetupCC();
		void setGlobalVolume(double);
		double getGlobalVolume();
		void setPolyphony(int);
		int getPolyphony();
		std::string getFileName();
		std::string getCompleteFileName();
		void createTaps();


		//Jack Part
		int getNbJackStereoChannels();
		void addJackStereoChannel(std::string);
		void addCreateJackStereoChannel(std::string);
		void removeJackStereoChannel(int );
		std::string getJackStereoChannelName(int);
		void setJackStereoChannelName(int,std::string);
		int getJackStereoChannel(std::string);
		jackProcess* getJackProcess();
		void setSampleRate(int);

		//Midi Part
		midiProcess* getMidiProcess();
		void startMidiLearn();
		void stopMidiLearn();
		void processMidiLearn(int,int,int);
		void processCC(unsigned short, unsigned short,unsigned short);
		void processPC(unsigned short, unsigned short);
		inline void addAudioEvent(const audioEvent& newEvent){jack->addAudioEvent(newEvent);}

		// nsm
		bool isDirty();
		bool isVisible();
		void setVisible(bool v);

	private:
		bool loop;

		//Tap File
		std::string fileName;

		#ifdef WITH_GUI
			//Gui
			execWindow *execWin;
		#endif

		//Stuctures
		std::vector<sample*> samples;
		std::vector<setup*> setups;

		//Audio
		std::vector<std::string> jackStereoChannelsNames;
		jackProcess *jack;
		double globalVolume;

		//Midi
		midiProcess* midi;
		short setupChannel;
		short setupCC;
		int polyphony;

		//Events
		jack_ringbuffer_t* eventsRingBuffer;

		#ifdef WITH_LASH
			//Lash
			lashClient *lashC;
		#endif
};

#endif
