/***************************************************************************
 *            tapeutape.h
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

#include "../osc/osc.h"

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
		tapeutape(char*);
		~tapeutape();
		void showMessage(bool,std::string);

		int load(char*);
		int start();
		void save(char*);
		void saveWithSamples(char*,const char*);
		void stop();
		void import(char*);

		int getNbKits();
		kit *getKit(int,string,int);
		void addKit(kit*);
		void removeKit(int);
		kit *getKitByName(string name,string snc="",int sn=-1);
		int getKitIdByName(string name,string snc="",int sn=-1);
		int getNbSetups();
		setup *getSetup(int);
		setup *getSetupByName(string);
		int getSetupIdByName(string);
		instrument *getInstrumentByName(string,string,int,string,int);
		int getInstrumentIdByName(string,string,int,string,int);
		void setInstrumentParameter(int,int,int,int,double);
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
		void changeKit(int, int);


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

		//OSC server
		OSCServer *oscServer;
	    static int oscCallback(const char *path, const char *types, lo_arg ** argv,
	                    int argc, void *data, void *user_data);
	    enum OSC_COMMANDS {
	        OSC_ZERO = 0,
	        SET_GLOBAL_VOLUME,
			GET_GLOBAL_VOLUME,
			GET_SETUPS_LIST,

			SETUP_GET_KITS_LIST,

			KIT_SELECT,
			KIT_SET_SELECTED,
			KIT_GET_SELECTED,
			KIT_GET_SELECTED_BYNAME,
			KIT_SET_VOLUME,
			KIT_GET_VOLUME,
			KIT_GET_VOLUME_BYNAME,
			KIT_GET_INSTRUMENTS_LIST,

			INSTRUMENT_SET_VOLUME,
			INSTRUMENT_GET_VOLUME,
			INSTRUMENT_SET_PAN,
			INSTRUMENT_GET_PAN,
			INSTRUMENT_SET_MIDITUNE,
			INSTRUMENT_GET_MIDITUNE,
			INSTRUMENT_SET_OUTPUT,
			INSTRUMENT_GET_OUTPUT,
			INSTRUMENT_SET_PLAYMODE,
			INSTRUMENT_GET_PLAYMODE,
			INSTRUMENT_SET_PLAYLOOP,
			INSTRUMENT_GET_PLAYLOOP,
			INSTRUMENT_SET_PLAYREVERSE,
			INSTRUMENT_GET_PLAYREVERSE,
			INSTRUMENT_SET_PITCHOVERRANGE,
			INSTRUMENT_GET_PITCHOVERRANGE,
			INSTRUMENT_TRIG,
			INSTRUMENT_TRIGREVERSE,
			INSTRUMENT_PLAY,
			INSTRUMENT_PLAYLOOP,
			INSTRUMENT_PLAYREVERSE,
			INSTRUMENT_STOP
	    };
	    std::map<std::string, int> oscCommands = {
	        {"/set/global_volume",  SET_GLOBAL_VOLUME},
			{"/get/global_volume",  GET_GLOBAL_VOLUME},
			{"/get/setups_list", GET_SETUPS_LIST},

			{"/setup/get/kit_lists", SETUP_GET_KITS_LIST},

			{"/kit/select", 		KIT_SELECT},
			{"/kit/set/selected", 	KIT_SET_SELECTED},
			{"/kit/get/selected", 	KIT_GET_SELECTED},
			{"/kit/get/selected/by_name", 	KIT_GET_SELECTED_BYNAME},
			{"/kit/set/volume",		KIT_SET_VOLUME},
			{"/kit/get/volume",		KIT_GET_VOLUME},
			{"/kit/get/volume/by_name",		KIT_GET_VOLUME_BYNAME},
			{"/kit/get/instruments_list", KIT_GET_INSTRUMENTS_LIST},

			{"/instrument/set/volume",			INSTRUMENT_SET_VOLUME},
			{"/instrument/get/volume",			INSTRUMENT_GET_VOLUME},
			{"/instrument/set/pan",				INSTRUMENT_SET_PAN},
			{"/instrument/get/pan",				INSTRUMENT_GET_PAN},
			{"/instrument/set/miditune",		INSTRUMENT_SET_MIDITUNE},
			{"/instrument/get/miditune",		INSTRUMENT_GET_MIDITUNE},
			{"/instrument/set/output",			INSTRUMENT_SET_OUTPUT},
			{"/instrument/get/output",			INSTRUMENT_GET_OUTPUT},
			{"/instrument/set/playmode",		INSTRUMENT_SET_PLAYMODE},
			{"/instrument/get/playmode",		INSTRUMENT_GET_PLAYMODE},
			{"/instrument/set/playloop",		INSTRUMENT_SET_PLAYLOOP},
			{"/instrument/get/playloop",		INSTRUMENT_GET_PLAYLOOP},
			{"/instrument/set/playreverse",		INSTRUMENT_SET_PLAYREVERSE},
			{"/instrument/get/playreverse",		INSTRUMENT_GET_PLAYREVERSE},
			{"/instrument/set/pitchoverrange",	INSTRUMENT_SET_PITCHOVERRANGE},
			{"/instrument/get/pitchoverrange",	INSTRUMENT_GET_PITCHOVERRANGE},
			{"/instrument/trig",				INSTRUMENT_TRIG},
			{"/instrument/trigreverse",			INSTRUMENT_TRIGREVERSE},
			{"/instrument/play",				INSTRUMENT_PLAY},
			{"/instrument/playreverse",			INSTRUMENT_PLAYREVERSE},
			{"/instrument/playloop",			INSTRUMENT_PLAYLOOP},
			{"/instrument/stop",				INSTRUMENT_STOP}

	    };


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
