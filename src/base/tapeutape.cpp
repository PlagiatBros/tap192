/***************************************************************************
 *            tapeutape.cpp
 *
 *  Copyright  2005 - 2013 Florent Berthaut, 2019 Jean-Emmanuel Doucet & Aurélien Roux
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include "tapeutape.h"
#include "../nsm/nsm.h"
extern nsm_client_t *nsm;


extern char *global_oscport;

enum O_TYPES {
	SETUP = 0,
	KIT,
	INSTRUMENT
};

enum X_TYPES {
	VOLUME = 0,
	PAN,
	MIDITUNE,
	//OUTPUT,
	PLAYMODE,
	PLAYLOOP,
	PLAYREVERSE,
	PITCHOVERRANGE,

	TRIG,
	PLAY,
	STOP
};

enum P_TYPES {
	P_DOUBLE = 0,
	P_FLOAT,
	P_INT,
	P_STRING
};

using namespace std;

tapeutape::tapeutape(char *fn):polyphony(100),globalVolume(1.0),fileName(""),jack(NULL),midi(NULL),eventsRingBuffer(NULL),loop(true)
{
	fileName="";

	#ifdef WITH_GUI
		//start the ui
		execWin = new execWindow("Tapeutape",this);
	#endif

	//ring buffer
	eventsRingBuffer = jack_ringbuffer_create(RING_BUFFER_SIZE);

	//jackProcess , initialised here to get the samplerate to load the files
	jack = new jackProcess(this,eventsRingBuffer,polyphony);
	if(jack->init())
	{
		showMessage(true,"Error Initialiasing Jack Client");
	}


	//midiProcess init
	midi = new midiProcess(this,jack->getClient(),MIDI_PRIORITY,eventsRingBuffer);
	showMessage(false,"Starting MIDI");
	if(midi->midiInit())
	{
		showMessage(true,"Error Starting Midi ");
	}

	//OSCServer init and start
  if (global_oscport != 0) {
        oscServer = new OSCServer(global_oscport);
        oscServer->start();
        oscServer->add_method(NULL, NULL, &tapeutape::oscCallback, this);
  }



	//if a file name was specified
	if(((string) fn).compare("")){
		load(fn);
	}

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


int tapeutape::oscCallback(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data)
{

    tapeutape *t = (tapeutape *)user_data;

    int command = t->oscCommands[(std::string) path];
    if (!command) return 0;



	//int sn=-1, kn=-1, in=-1, whatset = 0;
	int sn=-1, kn=-1, in=-1;
	int o_what, x_what, param_type=P_DOUBLE;
	int get=0, playstop=0;
	string snc="", knc="", inc="";
	char *address;
	//string spath, by, xwhat="miditune";
	string spath, by;
	lo_address lo_add;
	lo_message lo_msg;

    switch (command) {
// General OSC methods
		case SET_GLOBAL_VOLUME:
		// 'd' : global_volume
			if (argc > 0)
			{
					double gv;
					if (types[0] == 'i')	gv = (double)(argv[0]->i);
					else if (types[0] == 'f') gv = argv[0]->f;
					else break;
					t->setGlobalVolume(gv);
			}
			break;
		case GET_GLOBAL_VOLUME:
		// noargs : message will be sent to sender's address
		// 's' : address
			if (argc == 1) {
    			address = &argv[0]->s;
			} else {
    			address = lo_address_get_url(lo_message_get_source(data));
			}

			lo_add = lo_address_new_from_url(address);
			if (lo_add != NULL) {
				spath = path;
				spath.replace(spath.find("get"), 3, "tapeutape");
				lo_msg = lo_message_new();
				lo_message_add_double(lo_msg, t->getGlobalVolume());
				lo_send_message(lo_add, spath.c_str(), lo_msg);
				lo_address_free(lo_add);
			}
			break;


// Kit OSC methods
		case KIT_SET_SELECTED:
		case KIT_SELECT:
		// 'ii', 'is', 'si', 'ss' : Setup, Kit
			sn=-1; kn=-1;
			snc=""; knc="";
			if (argc > 1) {
				if (types[0] == 'i')
				{
					sn = argv[0]->i;
				 	if (types[1] == 'i') kn = argv[1]->i;
					else if(types[1] == 's') knc = &argv[1]->s;
				}
				else if (types[0] == 's')
				{
					snc = &argv[0]->s;
				 	if (types[1] == 'i') kn = argv[1]->i;
					else if(types[1] == 's') knc = &argv[1]->s;
				}
				else break;
			} else if (argc > 0) {
					if (types[0] == 'i') kn = argv[0]->i;
					else if (types[0] == 's')	knc = &argv[0]->s;
					else break;
			}

			if (snc != "") // setup(s) defined by name
			{
				sn = t->getSetupIdByName(snc);
				if (kn !=-1 ) t->changeKit(sn, kn); // kit defined by number
				else if (knc !="") { // kit(s) defined by name
					kn = t->getKitIdByName(knc, snc);
					if( kn != -1) t->changeKit(sn, kn);
				}
			} else if (sn !=-1) { // setup defined by number
				if (kn != -1) t->changeKit(sn,kn); // kit(s) defined by number
				else if (knc != "") { // kit defined by name
					kn = t->getKitIdByName(knc,snc,sn);
					if (kn != -1) t->changeKit(sn, kn);
				}
			}
			else if (sn == -1 && argc == 1) // setup not defined (one arg only)
			{
				for (unsigned int i=0;i<t->setups.size();++i)
				{
					if (kn != -1 && kn < t->setups[i]->getNbKits()) {
						t->changeKit(i,kn); // kit(s) defined by number
					} else if (knc != "") { // kit defined by name
						kn = t->getKitIdByName(knc, snc, i);
						if(kn != -1) t->changeKit(i,kn);
					}
				}
			}
			break;
		case KIT_GET_SELECTED_BYNAME:
			by = "by_name";
		case KIT_GET_SELECTED:
		// noargs : message will be sent to the sender's address
		// 's' : address
			if (argc == 1) {
				address = &argv[0]->s;
			} else {
				address = lo_address_get_url(lo_message_get_source(data));
			}

			lo_add = lo_address_new_from_url(address);
			if (lo_add != NULL) {
				spath = path;
				spath.replace(spath.find("kit/get"), 7, "tapeutape/kit");
				lo_msg = lo_message_new();
				for (int i=0;i<t->setups.size();i++)
				{
					if (!by.compare("by_name"))
					{
						lo_message_add_string(lo_msg, t->setups[i]->getName().c_str());
						lo_message_add_string(lo_msg, t->setups[i]->getKit(t->setups[i]->getCurrentKit())->getName().c_str());
					} else {
						lo_message_add_int32(lo_msg, i);
						lo_message_add_int32(lo_msg, t->setups[i]->getCurrentKit());
					}
				}
				lo_send_message(lo_add, spath.c_str(), lo_msg);
				lo_address_free(lo_add);
				by = "";
			}
			break;

// Trig OSC Set Parameters Methods
		case KIT_SET_VOLUME:
		// Setup (s,i), Kit (s,i), Volume (d,f,i)
		// Kit (s,i), Volume (d,f,i)
			o_what = KIT; x_what = VOLUME;
			break;
		case INSTRUMENT_SET_VOLUME:
		// Setup (s,i), Kit (s,i), Instrument (s,i), Volume (d,f,i)
		// Kit (s,i), Instrument (s,i), Volume (d,f,i)
		// Instrument (s,i), Volume (d,f,i)
			o_what = INSTRUMENT; x_what = VOLUME;
			break;
		case INSTRUMENT_SET_PAN:
		// Setup (s,i), Kit (s,i), Instrument (s,i), Pan (d,f,i)
		// Kit (s,i), Instrument (s,i), Pan (d,f,i)
		// Instrument (s,i), Pan (d,f,i)
			o_what = INSTRUMENT; x_what = PAN;
			break;
		case INSTRUMENT_SET_MIDITUNE:
		// Setup (s,i), Kit (s,i), Instrument (s,i), Miditune (d,f,i)
		// Kit (s,i), Instrument (s,i), Miditune (d,f,i)
		// Instrument (s,i), Miditune (d,f,i)
			o_what = INSTRUMENT; x_what = MIDITUNE;
			break;
		case INSTRUMENT_SET_PLAYMODE:
		// Setup (s,i), Kit (s,i), Instrument (s,i), Playmode (d,f,i)
		// Kit (s,i), Instrument (s,i), Playmode (d,f,i)
		// Instrument (s,i), Playmode (d,f,i)
			o_what = INSTRUMENT; x_what = PLAYMODE;
			break;


		case INSTRUMENT_PLAY:
		// Setup (s,i), Kit (s,i), Instrument (s,i), Velocity (i), (Pitch (f))
			o_what = INSTRUMENT; x_what = PLAY;
			playstop = 1;
			break;
		case INSTRUMENT_STOP:
		// Setup (s,i), Kit (s,i), Instrument (s,i)
			o_what = INSTRUMENT; x_what = STOP;
			playstop = 1;
			break;



// Trig OSC Get Parameters Methods
		case KIT_GET_VOLUME_BYNAME:
			by = "by_name";
		case KIT_GET_VOLUME:
		// Setup (s, i), Kit (s, i)
		// Setup (s, i), Kit (s, i), Address (s)
			o_what = KIT; x_what = VOLUME;
			param_type = P_DOUBLE; get = 1;
			break;

		case INSTRUMENT_GET_VOLUME_BYNAME:
			by = "by_name";
		case INSTRUMENT_GET_VOLUME:
		// Setup (s, i), Kit (s, i), Instrument (s, i)
		// Setup (s, i), Kit (s, i), Instrument (s, i), Address (s)
			o_what = INSTRUMENT; x_what = VOLUME;
			param_type = P_DOUBLE; get = 1;
			break;
		case INSTRUMENT_GET_PAN_BYNAME:
			by = "by_name";
		case INSTRUMENT_GET_PAN:
		// Setup (s, i), Kit (s, i), Instrument (s, i)
		// Setup (s, i), Kit (s, i), Instrument (s, i), Address (s)
			o_what = INSTRUMENT; x_what = PAN;
			param_type = P_DOUBLE; get = 1;
			break;
		case INSTRUMENT_GET_MIDITUNE_BYNAME:
			by = "by_name";
		case INSTRUMENT_GET_MIDITUNE:
		// Setup (s, i), Kit (s, i), Instrument (s, i)
		// Setup (s, i), Kit (s, i), Instrument (s, i), Address (s)
			o_what = INSTRUMENT; x_what = MIDITUNE;
			param_type = P_DOUBLE; get = 1;
			break;

/*		case INSTRUMENT_GET_OUTPUT_BYNAME:
			by = "by_name";
		case INSTRUMENT_GET_OUTPUT:
		// Setup (s, i), Kit (s, i), Instrument (s, i)
		// Setup (s, i), Kit (s, i), Instrument (s, i), Address (s)
			o_what = INSTRUMENT; x_what = OUTPUT;
			param_type = P_INT; get = 1;
			break;*/
		case INSTRUMENT_GET_PLAYMODE_BYNAME:
			by = "by_name";
		case INSTRUMENT_GET_PLAYMODE:
		// Setup (s, i), Kit (s, i), Instrument (s, i)
		// Setup (s, i), Kit (s, i), Instrument (s, i), Address (s)
			o_what = INSTRUMENT; x_what = PLAYMODE;
			param_type = P_INT; get = 1;
			break;
		case INSTRUMENT_GET_PLAYLOOP_BYNAME:
			by = "by_name";
		case INSTRUMENT_GET_PLAYLOOP:
		// Setup (s, i), Kit (s, i), Instrument (s, i)
		// Setup (s, i), Kit (s, i), Instrument (s, i), Address (s)
			o_what = INSTRUMENT; x_what = PLAYLOOP;
			param_type = P_INT; get = 1;
			break;
		case INSTRUMENT_GET_PLAYREVERSE_BYNAME:
			by = "by_name";
		case INSTRUMENT_GET_PLAYREVERSE:
		// Setup (s, i), Kit (s, i), Instrument (s, i)
		// Setup (s, i), Kit (s, i), Instrument (s, i), Address (s)
			o_what = INSTRUMENT; x_what = PLAYREVERSE;
			param_type = P_INT; get = 1;
			break;
		case INSTRUMENT_GET_PITCHOVERRANGE_BYNAME:
			by = "by_name";
		case INSTRUMENT_GET_PITCHOVERRANGE:
		// Setup (s, i), Kit (s, i), Instrument (s, i)
		// Setup (s, i), Kit (s, i), Instrument (s, i), Address (s)
			o_what = INSTRUMENT; x_what = PITCHOVERRANGE;
			param_type = P_INT; get = 1;
			break;
    }

// OSC Set and Play/Stop Methods
	if (argc > 0 && !get) {
		double x;
		float p=1;
		int argind=0;

		cout << argind << endl;
		if ((argc > 3 && o_what == INSTRUMENT) || x_what == STOP) {
			if (types[argind] == 's') snc = &argv[argind]->s;
			else if (types[argind] == 'i') sn = argv[argind]->i;
			else return -1;
			argind++;
		}
		cout << argind << endl;
		if (argc > 2) {
			if (o_what == KIT) {
				if (types[argind] == 's') snc = &argv[argind]->s;
				else if (types[argind] == 'i') sn = argv[argind]->i;
				else return -1;
			} else if (o_what == INSTRUMENT) {
				if (types[argind] == 's') knc = &argv[argind]->s;
				else if (types[argind] == 'i') kn = argv[argind]->i;
				else return -1;
			}
			argind++;
		}
		cout << argind << endl;
		if (argc > 1) {
			if(o_what == SETUP) {
				if (types[argind] == 's') snc = &argv[argind]->s;
				else if (types[argind] == 'i') sn = argv[argind]->i;
				else return -1;
			} else if(o_what == KIT) {
				if (types[argind] == 's') knc = &argv[argind]->s;
				else if (types[argind] == 'i') kn = argv[argind]->i;
				else return -1;
			} else if(o_what == INSTRUMENT) {
				if (types[argind] == 's') inc = &argv[argind]->s;
				else if (types[argind] == 'i') in = argv[argind]->i;
				else return -1;
			}
			argind++;
		}
		cout << argind << endl;
		if (argc > argind) {
			if (types[argind] == 'd') x = argv[argind]->d;
			else if (types[argind] == 'f') x = (double) argv[argind]->f;
			else if (types[argind] == 'i') x = (double) argv[argind]->i;
			else return -1;
		}
		cout << "avant playstop" << endl;
		if (playstop) {
			if (argc < 4) x = 127; // if no velocity defined
			if (x < 0) x = 127; // if velocity below 0
			if (argc > 4) {
				if (types[argind+1] == 'd') p = (float) argv[argind+1]->d;
				else if (types[argind+1] == 'f') p = argv[argind+1]->f;
				else if (types[argind+1] == 'i') p = (float) argv[argind+1]->i;
				else return -1;
			}
			if (p < 0) p = 1; // if pitch below 0
		}
		cout << "avant détermination" << endl;
		if (snc != "") sn = t->getSetupIdByName(snc); // Setup by name
		cout << "SN OK: " << sn << endl;
		if (knc != "" && (o_what == KIT || o_what == INSTRUMENT)) kn = t->getKitIdByName(knc, snc="", sn); // Kit by name
		cout << "KN OK: knc: " << knc << ", kn: " << kn << endl;
		if (inc != "" && o_what == INSTRUMENT) in = t->getInstrumentIdByName(inc, snc="", sn, knc="", kn); // Instrument by name

		cout << "MSG" << endl;
		cout << "snc :" << snc << ", knc: " << knc << ", inc: " << inc << ", x: " << x << endl;
		cout << "sn :" << sn << ", kn: " << kn << ", in: " << in << ", x: " << x << endl;

		switch(o_what){
			case INSTRUMENT:
				if(sn == -1) {
					for(int i=0; i<t->setups.size(); i++){
						if (kn == -1) {
							for (int j=0; j<t->setups[i]->getNbKits(); j++){
								if(in != -1 && in < t->setups[i]->getKit(j)->getNbInstruments()){
									if(playstop) t->playstopInstrument(i, j, in, x_what, (unsigned short) x, p);
									else t->setInstrumentParameter(i, j, in, x_what, (double) x);
								} else return -1;
							}
						} else {
							if(in != -1 && in < t->setups[i]->getKit(kn)->getNbInstruments()){
								if(playstop) t->playstopInstrument(i, kn, in, x_what, (unsigned short) x, p);
								else t->setInstrumentParameter(i, kn, in, x_what, (double) x);
							} else return -1;
						}
					}
				} else {
					if(in != -1 && in < t->setups[sn]->getKit(kn)->getNbInstruments()){
						if(playstop) t->playstopInstrument(sn, kn, in, x_what, (unsigned short) x, p);
						else t->setInstrumentParameter(sn, kn, in, x_what, (double) x);
					} else return -1;
				}
				break;
			case KIT:
				if(sn == -1) {
					for(int i=0; i<t->setups.size(); i++){
						t->setups[i]->getKit(kn)->setVolume(x);
					}
				} else {
					t->setups[sn]->getKit(kn)->setVolume(x);
				}
				break;
			case SETUP:
				break;
		}

// OSC Get Methods
	} else if(get) {
		// Setup
		if (types[0] == 's') snc = &argv[0]->s;
		else if (types[0] == 'i') sn = argv[0]->i;

		// Kit
		if (types[1] == 's') knc = &argv[1]->s;
		else if (types[1] == 'i') kn = argv[1]->i;

		if (argc > 2) {
			// Instrument
			if (o_what == INSTRUMENT) {
				if (types[2] == 's') inc = &argv[2]->s;
				else if (types[2] == 'i') in = argv[2]->i;
			} else {
				address = &argv[2]->s;
			}
		}
		if (argc > 3) {
			address = &argv[3]->s;
		}

		if (address == "") {
   			address = lo_address_get_url(lo_message_get_source(data));
		}

		lo_add = lo_address_new_from_url(address);
		spath = path;
		if (o_what == KIT) spath.replace(spath.find("kit/get"), 7, "tapeutape/kit");
		else if (o_what == INSTRUMENT) spath.replace(spath.find("instrument/get"), 14, "tapeutape/instrument");
		lo_msg = lo_message_new();

		if (snc != "") { // setup by name
			sn = t->getSetupIdByName(snc);
		}
		if (sn < t->setups.size() && sn != -1) {
			snc = "";
			if (knc != "") { // kit by name
				kn = t->getKitIdByName(knc, snc, sn);
			}
			if (o_what == INSTRUMENT) {
				if (kn < t->setups[sn]->getNbKits() && kn != -1) {
					knc = "";
					if (inc !="") { // instrument by name
						in = t->getInstrumentIdByName(inc, snc, sn, knc, kn);
					}
				} else return -1;
			}
		} else return -1;

		if (o_what == INSTRUMENT && sn == -1) return -1;

		if (lo_add != NULL) {
			if (!by.compare("by_name")){
				lo_message_add_string(lo_msg, t->setups[sn]->getName().c_str());
				lo_message_add_string(lo_msg, t->setups[sn]->getKit(kn)->getName().c_str());
				if (o_what == INSTRUMENT) lo_message_add_string(lo_msg, t->setups[sn]->getKit(kn)->getInstrument(in)->getName().c_str());
			} else {
				lo_message_add_int32(lo_msg, sn);
				lo_message_add_int32(lo_msg, kn);
				if (o_what == INSTRUMENT) lo_message_add_int32(lo_msg, in);
			}

			if (o_what == KIT) lo_message_add_double(lo_msg, t->setups[sn]->getKit(kn)->getVolume());
			else if (o_what == INSTRUMENT)
				if(param_type == P_DOUBLE)
					lo_message_add_double(lo_msg, t->getInstrumentParameter(sn, kn, in, x_what));
				else if(param_type == P_INT) {
					lo_message_add_int32(lo_msg, (int) t->getInstrumentParameter(sn, kn, in, x_what));
				}

			lo_send_message(lo_add, spath.c_str(), lo_msg);
			lo_address_free(lo_add);
			lo_message_free(lo_msg);

			by = "";
		} else return -1;
		get = 0;
	}
    return 0;
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

setup* tapeutape::getSetupByName(string name)
{
	for (int i=0;i<setups.size();i++)
	{
		if (!name.compare(setups[i]->getName())) {
			return setups[i];
		}
	}
	return NULL;
}

int tapeutape::getSetupIdByName(string name)
{
	for (int i=0;i<setups.size();i++)
	{
		if (!name.compare(setups[i]->getName())) {
			return i;
		}
	}
	return -1;
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

kit* tapeutape::getKit(int ind, string snc="", int sn=-1)
{
	if(ind>=0 && ind<(int)(setups.size())) {
		if (snc != "" && sn == -1) { // kit number ind in setup by name
			for (int i;i<setups.size();i++){
				if (!snc.compare(setups[i]->getName())) {
					return setups[i]->getKit(ind);
				}
			}
		} else if (snc =="" && sn != -1 && sn < setups.size()) { // kit number ind in setup by id
			return setups[sn]->getKit(ind);
		} else return NULL;
	}
	return NULL;
}

kit* tapeutape::getKitByName(string name, string snc, int sn)
{
	if (snc != "" && sn == -1) { // kit name in setup by name
		for (int i=0;i<setups.size();i++){
			if (!snc.compare(setups[i]->getName())) {
				for (int j;j<setups[i]->getNbKits();j++) {
					if (!name.compare(setups[i]->getKit(j)->getName())) {
						return setups[i]->getKit(j);
					}
				}
			}
		}
	} else if (snc == "" && sn != -1) { // kit name in setup by id
		if (sn < setups.size()){
			for (int j=0;j<setups[sn]->getNbKits();j++) {
				if (!name.compare(setups[sn]->getKit(j)->getName())) {
					return setups[sn]->getKit(j);
				}
			}
		}
	}
	return NULL;
}

int tapeutape::getKitIdByName(string name, string snc, int sn)
{
	if (snc != "" && sn == -1) { // kit id in setup by name
		for (int i=0;i<setups.size();i++){
			if (!snc.compare(setups[i]->getName())) {
				for (int j=0;j<setups[i]->getNbKits();j++) {
					if (!name.compare(setups[i]->getKit(j)->getName())) {
						return j;
					}
				}
			}
		}
	} else if (snc == "" && sn != -1) { // kit name in setup by id
		if (sn < setups.size()){
			for (int j=0;j<setups[sn]->getNbKits();j++) {
				if (!name.compare(setups[sn]->getKit(j)->getName())) {
					return j;
				}
			}
		}
	}
	return -1;
}

instrument* tapeutape::getInstrumentByName(string name, string snc, int sn, string knc, int kn){
	return NULL;
}

int tapeutape::getInstrumentIdByName(string name, string snc, int sn, string knc, int kn){
	if (snc != "" && sn == -1) { // if setup by name
		for (int i=0;i<setups.size();i++){
			if (!snc.compare(setups[i]->getName())) {
				sn = i;
			}
		}
	}

	if (knc != "" && kn == -1) { // if kit by name
		for (int i=0;i<setups[sn]->getNbKits();i++) {
			if (!knc.compare(setups[sn]->getKit(i)->getName())) {
				kn = i;
			}
		}
	}


	if (sn < setups.size() && sn != -1) {
		if (kn < setups[sn]->getNbKits() && kn != -1){
			for (int i=0;i<setups[sn]->getKit(kn)->getNbInstruments(); i++){
				if (!name.compare(setups[sn]->getKit(kn)->getInstrument(i)->getName())){
					return i;
				}
			}
		}
	}

	return -1;
}

void tapeutape::setInstrumentParameter(int sn, int kn, int in, int x_what, double x){
	if (x_what == VOLUME) setups[sn]->getKit(kn)->getInstrument(in)->setVolume(x);
	else if (x_what == PAN) setups[sn]->getKit(kn)->getInstrument(in)->setPan(x);
	else if (x_what == MIDITUNE) setups[sn]->getKit(kn)->getInstrument(in)->setRootNoteFine(x);
	else if (x_what == PLAYMODE) setups[sn]->getKit(kn)->getInstrument(in)->setPlayMode((int) x);
	else if (x_what == PLAYLOOP) setups[sn]->getKit(kn)->getInstrument(in)->setPlayLoop((int) x);
	else if (x_what == PLAYREVERSE) setups[sn]->getKit(kn)->getInstrument(in)->setPlayReverse((int) x);
	else if (x_what == PITCHOVERRANGE) setups[sn]->getKit(kn)->getInstrument(in)->setPitchOverRange((bool) x);

}

double tapeutape::getInstrumentParameter(int sn, int kn, int in, int x_what){
	double x = -1;
	if (x_what == VOLUME) x = setups[sn]->getKit(kn)->getInstrument(in)->getVolume();
	else if (x_what == PAN) x = setups[sn]->getKit(kn)->getInstrument(in)->getPan();
	else if (x_what == MIDITUNE) x = setups[sn]->getKit(kn)->getInstrument(in)->getRootNoteFine();
	else if (x_what == PLAYMODE) x = (double) setups[sn]->getKit(kn)->getInstrument(in)->getPlayMode();
	else if (x_what == PLAYLOOP) x = (double) setups[sn]->getKit(kn)->getInstrument(in)->getPlayLoop();
	else if (x_what == PLAYREVERSE) x = (double) setups[sn]->getKit(kn)->getInstrument(in)->getPlayReverse();
	else if (x_what == PITCHOVERRANGE) x = (double) setups[sn]->getKit(kn)->getInstrument(in)->getPitchOverRange();
	return x;
}

void tapeutape::playstopInstrument(int sn, int kn, int in, int x_what, const unsigned short t_velocity, float pitch){

	instrument* ins = setups[sn]->getKit(kn)->getInstrument(in);
	int noteOn = 0, vn;

	for (int i=0; i<ins->getNbVariations(); i++){
		if((ins->getVariation(i)->getMinVeloc() <= t_velocity)
			 && (ins->getVariation(i)->getMaxVeloc() >= t_velocity)) {
			vn = i;
		}
	}

	if (x_what == PLAY) noteOn = 1;
	if (x_what == STOP) noteOn = 0;

	if(ins->getPlayReverse()) {
		pitch*=-1.0;
	}

	double vol = globalVolume * setups[sn]->getKit(kn)->compVolume(ins->getVolume(),t_velocity,ins->getVariation(vn)->getMinVeloc(),ins->getVariation(vn)->getMaxVeloc());

	unsigned long id = sn * 10000 + kn * 1000 + in;
	audioEvent ae(
		ins->getVariation(vn),
		ins,
		id,
		pitch,
		ins->getJackStereoChannel(),
		vol,
		sqrt(2)/2.0*(cos(ins->getPan()*(M_PI/4.0)) - sin(ins->getPan()*(M_PI/4.0))),
		sqrt(2)/2.0*(cos(ins->getPan()*(M_PI/4.0)) + sin(ins->getPan()*(M_PI/4.0))),
		noteOn);
	addAudioEvent(ae);
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

void tapeutape::changeKit(int sn, int kn)
{
			int kit = setups[sn]->changeKit(kn);

			#ifdef WITH_GUI
				Fl::lock();
				execWin->changeKit(sn,kit+1);
				Fl::unlock();

				string mes = "Kit changed to " + to_string(kn) + ":'"  + setups[sn]->getKit(kn)->getName() + "' in Setup " + to_string(sn) + ":'" + setups[sn]->getName() + "'";
				showMessage(false, mes);
			#endif
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
	for(unsigned int i=0;i<setups.size();++i)
	{
		int cK = setups[i]->getCurrentKit();

		if(cc > 125)
		{
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
		else
		if(cc > 123)
		{
			// CC 125 set the instrument(s) bound to note "val" in Loop mode
			// CC 124 set the instrument(s) bound to note "val" in SingleShot mode

			for(unsigned int j=0;j<setups[i]->getKit(cK)->getNbInstruments();j++)
			{
				int iChan = setups[i]->getKit(cK)->getInstrument(j)->getMidiChannel();
				int iMaxN = setups[i]->getKit(cK)->getInstrument(j)->getMaxNote();
				int iMinN = setups[i]->getKit(cK)->getInstrument(j)->getMinNote();
				if(iChan-1 == chan && val <= iMaxN && val >= iMinN)
				{
					setups[i]->getKit(cK)->getInstrument(j)->setPlayLoop(125-cc);
				}
			}
		}
		else
		if(cc > 121)
		{
			// CC 123 set the instrument(s) bound to note "val" in Normal mode
			// CC 122 set the instrument(s) bound to note "val" in Trigger mode

			for(unsigned int j=0;j<setups[i]->getKit(cK)->getNbInstruments();j++)
			{
				int iChan = setups[i]->getKit(cK)->getInstrument(j)->getMidiChannel();
				int iMaxN = setups[i]->getKit(cK)->getInstrument(j)->getMaxNote();
				int iMinN = setups[i]->getKit(cK)->getInstrument(j)->getMinNote();
				if(iChan-1 == chan && val <= iMaxN && val >= iMinN)
				{
					setups[i]->getKit(cK)->getInstrument(j)->setPlayMode(123-cc);
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
		if(setups[i]->getChannel()-1 == chan && ipc < setups[i]->getNbKits()+1) changeKit(i, pc);
}




void tapeutape::setGlobalVolume(double gv)
{
	globalVolume = gv;
	execWin->setGlobalVolume(gv);
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
