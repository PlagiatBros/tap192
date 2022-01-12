// This file is part of tap192
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "../package.h"
#include "tapeutape.h"
#include "../nsm/nsm.h"
extern nsm_client_t *nsm;

extern char *global_oscport;

enum O_TYPES
{
    SETUP = 0,
    KIT,
    INSTRUMENT
};

enum X_TYPES
{
    SELECT = 0,

    VOLUME,
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

enum P_TYPES
{
    P_DOUBLE = 0,
    P_FLOAT,
    P_INT,
    P_STRING
};

int sn=-1, kn=-1, in=-1;
string snc="", knc="", inc="";


using namespace std;

tapeutape::tapeutape(char *fn):
    loop(true),
    fileName(""),
    jack(NULL),
    globalVolume(1.0),
    midi(NULL),
    polyphony(100),
    eventsRingBuffer(NULL)
{
    fileName="";

    //start the ui
    execWin = new execWindow(PACKAGE_NAME,this);

    //ring buffer
    eventsRingBuffer = jack_ringbuffer_create(RING_BUFFER_SIZE);

    //jackProcess , initialised here to get the samplerate to load the files
    jack = new jackProcess(this,eventsRingBuffer,polyphony);
    if (jack->init()) {
        showMessage(true,"Error Initialiasing Jack Client");
    }

    //midiProcess init
    midi = new midiProcess(this,jack->getClient(),MIDI_PRIORITY,eventsRingBuffer);
    showMessage(false,"Starting MIDI");
    if (midi->midiInit()) {
        showMessage(true,"Error Starting Midi ");
    }

    //OSCServer init and start
    if (global_oscport != 0) {
        oscServer = new OSCServer(global_oscport);
        oscServer->start();
        oscServer->add_method(NULL, NULL, &tapeutape::oscCallback, this);
    }

    //if a file name was specified
    if (((string) fn).compare("")) {
        load(fn);
    }

    //start
    start();
    midi->startThread();

    //if everything's ok
    showMessage(false,"Let's play !! ");

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


    int o_what, x_what, param_type=P_DOUBLE;
    int get=0, playstop=0;
    char *address = NULL;
    string spath, by;
    lo_address lo_add;
    lo_message lo_msg;

    sn=-1; kn=-1; in=-1;
    snc=""; knc=""; inc="";

    switch (command) {
// General OSC methods
        case SET_GLOBAL_VOLUME:
            // global_volume (d, f, i)
            if (argc > 0) {
                double gv;
                if (types[0] == 'i')    gv = (double)(argv[0]->i);
                else if (types[0] == 'f') gv = (double) argv[0]->f;
                else if (types[0] == 'd') gv = argv[0]->d;
                else break;
                t->setGlobalVolume(gv);
            }
            break;
        case GET_GLOBAL_VOLUME:
            // noargs : message will be sent to sender's address
            // address (s)
            if (argc == 1) {
                address = &argv[0]->s;
            }
            else {
                address = lo_address_get_url(lo_message_get_source(data));
            }

            lo_add = lo_address_new_from_url(address);
            if (lo_add != NULL) {
                spath = path;
                spath.replace(spath.find("get"), 3, BINARY_NAME);
                lo_msg = lo_message_new();
                lo_message_add_double(lo_msg, t->getGlobalVolume());
                lo_send_message(lo_add, spath.c_str(), lo_msg);
                lo_address_free(lo_add);
            }
            break;

// Kit OSC methods
// Set
        case KIT_SET_SELECTED:
        case KIT_SELECT:
            o_what = KIT; x_what = SELECT;
            break;
        case KIT_SET_VOLUME:
            // "s:Setup/Kit" (s), Volume (d,f,i) || "i:Setup_id/Kit_id" (s), Volume (d,f,i)
            // "s:Kit" (s), Volume (d,f,i) || "i:Kit_id" (s), Volume (d,f,i)
            o_what = KIT; x_what = VOLUME;
            break;
// Get
        case KIT_GET_SELECTED_BYNAME:
            by = "by_name";
        case KIT_GET_SELECTED:
            // noargs : message will be sent to sender's address
            // address (s)
            o_what = KIT; x_what = SELECT;
            get = 1;
            break;
        case KIT_GET_VOLUME_BYNAME:
            by = "by_name";
        case KIT_GET_VOLUME:
            // "s:Setup/Kit" (s), (address (s)) || "i:Setup_id/Kit_id" (s), (address (s))
            o_what = KIT; x_what = VOLUME;
            param_type = P_DOUBLE; get = 1;
            break;


// Instruments OSC Methods
// Set
        case INSTRUMENT_SET_VOLUME:
            // "s:Setup/Kit/Instrument" (s), Volume (d,f,i) || "i:Setup_id/Kit_id/Instrument_id" (s), Volume (d,f,i)
            // "s:Kit/Instrument" (s), Volume (d,f,i) || "i:Kit_id/Instrument_id" (s), Volume (d,f,i)
            // "s:Instrument" (s), Volume (d,f,i) || "i:Instrument_id" (s), Volume (d,f,i)
            o_what = INSTRUMENT; x_what = VOLUME;
            break;
        case INSTRUMENT_SET_PAN:
            // "s:Setup/Kit/Instrument" (s), Pan (d,f,i) || "i:Setup_id/Kit_id/Instrument_id" (s), Pan (d,f,i)
            // "s:Kit/Instrument" (s), Pan (d,f,i) || "i:Kit_id/Instrument_id" (s), Pan (d,f,i)
            // "s:Instrument" (s), Pan (d,f,i) || "i:Instrument_id" (s), Pan (d,f,i)
            o_what = INSTRUMENT; x_what = PAN;
            break;
        case INSTRUMENT_SET_MIDITUNE:
            // "s:Setup/Kit/Instrument" (s), Miditune (d,f,i) || "i:Setup_id/Kit_id/Instrument_id" (s), Miditune (d,f,i)
            // "s:Kit/Instrument" (s), Miditune (d,f,i) || "i:Kit_id/Instrument_id" (s), Miditune (d,f,i)
            // "s:Instrument" (s), Miditune (d,f,i) || "i:Instrument_id" (s), Miditune (d,f,i)
            o_what = INSTRUMENT; x_what = MIDITUNE;
            break;
        case INSTRUMENT_SET_PLAYMODE:
            // "s:Setup/Kit/Instrument" (s), Playmode (i) || "i:Setup_id/Kit_id/Instrument_id" (s), Playmode (i)
            // "s:Kit/Instrument" (s), Playmode (i) || "i:Kit_id/Instrument_id" (s), Playmode (i)
            // "s:Instrument" (s), Playmode (i) || "i:Instrument_id" (s), Playmode (i)
            o_what = INSTRUMENT; x_what = PLAYMODE;
            break;
        case INSTRUMENT_SET_PLAYLOOP:
            // "s:Setup/Kit/Instrument" (s), Playloop (i) || "i:Setup_id/Kit_id/Instrument_id" (s), Playloop (i)
            // "s:Kit/Instrument" (s), Playloop (i) || "i:Kit_id/Instrument_id" (s), Playloop (i)
            // "s:Instrument" (s), Playloop (i) || "i:Instrument_id" (s), Playloop (i)
            o_what = INSTRUMENT; x_what = PLAYLOOP;
            break;
        case INSTRUMENT_SET_PLAYREVERSE:
            // "s:Setup/Kit/Instrument" (s), Playreverse (i) || "i:Setup_id/Kit_id/Instrument_id" (s), Playreverse (i)
            // "s:Kit/Instrument" (s), Playreverse (i) || "i:Kit_id/Instrument_id" (s), Playreverse (i)
            // "s:Instrument" (s), Playreverse (i) || "i:Instrument_id" (s), Playreverse (i)
            o_what = INSTRUMENT; x_what = PLAYREVERSE;
            break;
        case INSTRUMENT_SET_PITCHOVERRANGE:
            // "s:Setup/Kit/Instrument" (s), PitchOverRange (i) || "i:Setup_id/Kit_id/Instrument_id" (s), PitchOverRange (i)
            // "s:Kit/Instrument" (s), PitchOverRange (i) || "i:Kit_id/Instrument_id" (s), PitchOverRange (i)
            // "s:Instrument" (s), PitchOverRange (i) || "i:Instrument_id" (s), PitchOverRange (i)
            o_what = INSTRUMENT; x_what = PITCHOVERRANGE;
            break;

        case INSTRUMENT_PLAY:
            // "s:Setup/Kit/Instrument" (s), (Velocity (i), Pitch (d||f||i)) || "i:Setup_id/Kit_id/Instrument_id" (s), (Velocity (i), Pitch (d||f||i))
            // "s:Kit/Instrument" (s), Play (i) || "i:Kit_id/Instrument_id" (s), (Velocity (i), Pitch (d||f||i))
            // "s:Instrument" (s), (Velocity (i), Pitch (d||f||i)) || "i:Instrument_id" (s), (Velocity (i), Pitch (d||f||i))
            o_what = INSTRUMENT; x_what = PLAY;
            playstop = 1;
            break;
        case INSTRUMENT_STOP:
            // "s:Setup/Kit/Instrument" (s) || "i:Setup_id/Kit_id/Instrument_id" (s)
            // "s:Kit/Instrument" (s) || "i:Kit_id/Instrument_id" (s)
            // "s:Instrument" (s) || "i:Instrument_id" (s)
            o_what = INSTRUMENT; x_what = STOP;
            playstop = 1;
            break;

// Get
        case INSTRUMENT_GET_VOLUME_BYNAME:
            by = "by_name";
        case INSTRUMENT_GET_VOLUME:
            // "s:Setup/Kit/Instrument" (s), (address (s)) || "i:Setup_id/Kit_id/Instrument_id" (s), (address (s))
            o_what = INSTRUMENT; x_what = VOLUME;
            param_type = P_DOUBLE; get = 1;
            break;
        case INSTRUMENT_GET_PAN_BYNAME:
            by = "by_name";
        case INSTRUMENT_GET_PAN:
            // "s:Setup/Kit/Instrument" (s), (address (s)) || "i:Setup_id/Kit_id/Instrument_id" (s), (address (s))
            o_what = INSTRUMENT; x_what = PAN;
            param_type = P_DOUBLE; get = 1;
            break;
        case INSTRUMENT_GET_MIDITUNE_BYNAME:
            by = "by_name";
        case INSTRUMENT_GET_MIDITUNE:
            // "s:Setup/Kit/Instrument" (s), (address (s)) || "i:Setup_id/Kit_id/Instrument_id" (s), (address (s))
            o_what = INSTRUMENT; x_what = MIDITUNE;
            param_type = P_DOUBLE; get = 1;
            break;
        case INSTRUMENT_GET_PLAYMODE_BYNAME:
            by = "by_name";
        case INSTRUMENT_GET_PLAYMODE:
            // "s:Setup/Kit/Instrument" (s), (address (s)) || "i:Setup_id/Kit_id/Instrument_id" (s), (address (s))
            o_what = INSTRUMENT; x_what = PLAYMODE;
            param_type = P_INT; get = 1;
            break;
        case INSTRUMENT_GET_PLAYLOOP_BYNAME:
            by = "by_name";
        case INSTRUMENT_GET_PLAYLOOP:
            // "s:Setup/Kit/Instrument" (s), (address (s)) || "i:Setup_id/Kit_id/Instrument_id" (s), (address (s))
            o_what = INSTRUMENT; x_what = PLAYLOOP;
            param_type = P_INT; get = 1;
            break;
        case INSTRUMENT_GET_PLAYREVERSE_BYNAME:
            by = "by_name";
        case INSTRUMENT_GET_PLAYREVERSE:
            // "s:Setup/Kit/Instrument" (s), (address (s)) || "i:Setup_id/Kit_id/Instrument_id" (s), (address (s))
            o_what = INSTRUMENT; x_what = PLAYREVERSE;
            param_type = P_INT; get = 1;
            break;
        case INSTRUMENT_GET_PITCHOVERRANGE_BYNAME:
            by = "by_name";
        case INSTRUMENT_GET_PITCHOVERRANGE:
            // "s:Setup/Kit/Instrument" (s), (address (s)) || "i:Setup_id/Kit_id/Instrument_id" (s), (address (s))
            o_what = INSTRUMENT; x_what = PITCHOVERRANGE;
            param_type = P_INT; get = 1;
            break;
    }

    // OSC Set and Play/Stop Methods
    if (argc > 0 && !get) {
        double x;
        float p=1;

        t->parseOscSKI(&argv[0]->s, o_what);

        if (argc > 1) {
            if (types[1] == 'd') x = argv[1]->d;
            else if (types[1] == 'f') x = (double) argv[1]->f;
            else if (types[1] == 'i') x = (double) argv[1]->i;
            else return -1;
        }

        if (playstop) {
            if (argc < 2) x = 127; // if no velocity defined
            if (x < 0) x = 127;  // if velocity below 0
            if (argc > 2) {
                if (types[2] == 'd') p = (float) argv[2]->d;
                else if (types[2] == 'f') p = argv[2]->f;
                else if (types[2] == 'i') p = (float) argv[2]->i;
                else return -1;
            }
            if (p < 0) p = 1;    // if pitch below 0
        }

        // Setup by name
        if (snc != "") sn = t->getSetupIdByName(snc);

        switch(o_what) {
            case INSTRUMENT:
                if (knc != "") kn = t->getKitIdByName(knc, snc="", sn);
                if (sn == -1) {
                    for(int i=0; i<(int)t->setups.size(); i++) {
                        if (kn == -1) {
                            for (int j=0; j<t->setups[i]->getNbKits(); j++) {
                                if (inc != "") {
                                    for (int k=0; k<t->setups[i]->getKit(j)->getNbInstruments(); k++){
                                        if (!inc.compare(t->setups[i]->getKit(j)->getInstrument(k)->getName())) {
                                            in = k;
                                        } //else in = -1;
                                    }
                                }
                                if (in != -1 && in < t->setups[i]->getKit(j)->getNbInstruments()) {
                                    if (playstop) t->playstopInstrument(i, j, in, x_what, (unsigned short) x, p);
                                    else t->setInstrumentParameter(i, j, in, x_what, (double) x);
                                }
                            }
                        }
                        else {
                            if (inc != "") in = t->getInstrumentIdByName(inc, snc="", i, knc="", kn);
                            if (in != -1 && in < t->setups[i]->getKit(kn)->getNbInstruments()) {
                                if (playstop) t->playstopInstrument(i, kn, in, x_what, (unsigned short) x, p);
                                else t->setInstrumentParameter(i, kn, in, x_what, (double) x);
                            } else return -1;
                        }
                    }
                }
                else {
                    if (inc != "") in = t->getInstrumentIdByName(inc, snc="", sn, knc="", kn);
                    if (in != -1 && in < t->setups[sn]->getKit(kn)->getNbInstruments()) {
                        if (playstop) t->playstopInstrument(sn, kn, in, x_what, (unsigned short) x, p);
                        else t->setInstrumentParameter(sn, kn, in, x_what, (double) x);
                    } else return -1;
                }
                break;
            case KIT:
                if (sn == -1) {
                    for(int i=0; i<(int)t->setups.size(); i++) {
                        if (knc != "") kn = t->getKitIdByName(knc, snc="", i);
                        if(kn != -1 && kn < t->setups[i]->getNbKits()) {
                            if (x_what == SELECT) t->changeKit(i, kn);
                            if (x_what == VOLUME) t->setups[i]->getKit(kn)->setVolume(x);
                        }
                    }
                }
                else {
                    if (knc != "") kn = t->getKitIdByName(knc, snc="", sn);
                    if(kn != -1 && kn < t->setups[sn]->getNbKits()) {
                        if (x_what == SELECT) t->changeKit(sn, kn);
                        if (x_what == VOLUME) t->setups[sn]->getKit(kn)->setVolume(x);
                    }
                }
                break;
            case SETUP:
                break;
        }
    }
    // OSC Get Methods
    else if (get) {

        if (argc > 1) {
            t->parseOscSKI(&argv[0]->s, o_what);
            address = &argv[1]->s;
        }
        else if (argc > 0 && x_what == SELECT) {
            address = &argv[0]->s;
        }

        if (address == NULL) address = lo_address_get_url(lo_message_get_source(data));

        lo_add = lo_address_new_from_url(address);
        spath = path;
        if (o_what == KIT) spath.replace(spath.find("kit/get"), 7, (string)BINARY_NAME + "/kit");
        else if (o_what == INSTRUMENT) spath.replace(spath.find("instrument/get"), 14, (string)BINARY_NAME + "/instrument");
        if (by == "by_name") spath.erase(spath.find("/by_name"));
        lo_msg = lo_message_new();

        if (lo_add != NULL) {
            if (x_what == SELECT) { // If get/selected
                for (int i=0;i<(int)t->setups.size();i++) {
                    if (!by.compare("by_name")) {
                        lo_message_add_string(lo_msg, t->setups[i]->getName().c_str());
                        lo_message_add_string(lo_msg, t->setups[i]->getKit(t->setups[i]->getCurrentKit())->getName().c_str());
                    }
                    else {
                        lo_message_add_int32(lo_msg, i);
                        lo_message_add_int32(lo_msg, t->setups[i]->getCurrentKit());
                    }
                }
            } else { // If get parameters
                if (snc != "") {
                    // setup by name
                    sn = t->getSetupIdByName(snc);
                }
                if (sn < (int)t->setups.size() && sn != -1) {
                    snc = "";
                    if (knc != "") {
                        // kit by name
                        kn = t->getKitIdByName(knc, snc, sn);
                    }
                    if (o_what == INSTRUMENT) {
                        if (kn < t->setups[sn]->getNbKits() && kn != -1) {
                            knc = "";
                            // instrument by name
                            if (inc !="") {
                                in = t->getInstrumentIdByName(inc, snc, sn, knc, kn);
                            }
                        } else return -1;
                    }
                } else return -1;

                if (o_what == INSTRUMENT && sn == -1) return -1;

                if (!by.compare("by_name")) {
                    lo_message_add_string(lo_msg, t->setups[sn]->getName().c_str());
                    lo_message_add_string(lo_msg, t->setups[sn]->getKit(kn)->getName().c_str());
                    if (o_what == INSTRUMENT) lo_message_add_string(lo_msg, t->setups[sn]->getKit(kn)->getInstrument(in)->getName().c_str());
                }
                else {
                    lo_message_add_int32(lo_msg, sn);
                    lo_message_add_int32(lo_msg, kn);
                    if (o_what == INSTRUMENT) lo_message_add_int32(lo_msg, in);
                }
                if (o_what == KIT) lo_message_add_double(lo_msg, t->setups[sn]->getKit(kn)->getVolume());
                else if (o_what == INSTRUMENT) {
                    if (param_type == P_DOUBLE)
                        lo_message_add_double(lo_msg, t->getInstrumentParameter(sn, kn, in, x_what));
                    else if (param_type == P_INT) {
                        lo_message_add_int32(lo_msg, (int) t->getInstrumentParameter(sn, kn, in, x_what));
                    }
                }
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
    Fl::lock();
    execWin->showMessage(t,mess);
    Fl::unlock();
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
    for(unsigned int i=0;i<setups.size();++i) {
        delete setups[i];
    }
    setups.clear();

    //delete the samples
    for(unsigned int i=0;i<samples.size();++i) {
        delete samples[i];
    }
    samples.clear();

    //delete the outputs
    jackStereoChannelsNames.clear();

    fileName=nomfic;

    // File Open
    tapParser parse(this);
    if (parse.createTree(nomfic))
        return -1;
    showMessage(false,"Loading "+getFileName());
    if (parse.parseTree()) {
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
    if (fl_filename_isdir(dirStr.c_str())) {
        //create a default setup/kit/instru ...
        setup* setup1 = new setup();
        addSetup(setup1);
        kit* kit1=NULL;
        instrument* instru1=NULL;
        //parse
        ostringstream oss,oss2;
        DIR *dp2;
        struct dirent *dirp2;
        if ((dp2  = opendir((dirStr).c_str())) != NULL) {
            while ((dirp2 = readdir(dp2)) != NULL) {
                if (dirp2->d_name[0]!='.') { //if not a hidden file
                    if (!fl_filename_isdir(dirp2->d_name)) { //if it's a file, try to add it as an instrument
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
    if (jackStereoChannelsNames.size()==0) {
        jackStereoChannelsNames.push_back("output");
    }

    //jackProcess , real start
    if (jack->start()) {
        showMessage(true,"Error starting jack client");
    }

    //create the array of taps for each setup
    createTaps();

    //(re)init the gui
    Fl::lock();
    execWin->init();
    execWin->setTitle(PACKAGE_NAME+" : "+getFileName());
    Fl::unlock();

    return 0;
}

void tapeutape::save(char* f)
{
    showMessage(false,"Now saving...");
    fileName=f;
    tapParser tap(this);
    if (!tap.saveToFile(f)) {

        Fl::lock();
        execWin->setTitle(PACKAGE_NAME+" : "+getFileName());
        Fl::unlock();
        for(unsigned int s=0;s<samples.size();++s) {
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
    for(unsigned int s=0;s<samples.size();++s) {
        //get the new filename, according to the tap path
        std::string sampleName;
        std::ostringstream oss;
        oss << s;
        sampleName ="sample"+oss.str();
        size_t found = samples[s]->getAbsoluteName().find_last_of(".\\");
        std::string sampleExt = samples[s]->getAbsoluteName().substr(found+1);
        std::string sampleCompleteName = path+"/"+sampleName+"."+sampleExt;
        //if the sample wasn't already saved with the tap
        if (samples[s]->getAbsoluteName()!=sampleCompleteName) {
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
    if (!tap.saveToFile(f)) {
        //set the new name
        Fl::lock();
        execWin->setTitle("Tapeutape : "+getFileName());
        Fl::unlock();
    }
}

void tapeutape::stop()
{
    //stops the midi thread
    if (midi) {
        midi->stopThread();
        delete midi;
        midi=NULL;
    }

    //stops the jack client
    if (jack) {
        delete jack;
        jack=NULL;
    }

    if (eventsRingBuffer) {
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
    for(unsigned int i=0;i<setups.size();++i) {
        for(int j=0;j<setups[i]->getNbKits();++j) {
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
    if (ind>=0 && ind<(int)(setups.size()))
        return setups[ind];
    else
        return NULL;
}

setup* tapeutape::getSetupByName(string name)
{
    for (int i=0;i<(int)setups.size();i++) {
        if (!name.compare(setups[i]->getName())) {
            return setups[i];
        }
    }
    return NULL;
}

int tapeutape::getSetupIdByName(string name)
{
    for (int i=0;i<(int)setups.size();i++) {
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
    if (ind>=0 && ind<(int)(setups.size())) {
                                 // kit number ind in setup by name
        if (snc != "" && sn == -1) {
            for (int i;i<(int)setups.size();i++) {
                if (!snc.compare(setups[i]->getName())) {
                    return setups[i]->getKit(ind);
                }
            }
        }                        // kit number ind in setup by id
        else if (snc =="" && sn != -1 && sn < (int)setups.size()) {
            return setups[sn]->getKit(ind);
        } else return NULL;
    }
    return NULL;
}

kit* tapeutape::getKitByName(string name, string snc, int sn)
{
    if (snc != "" && sn == -1) { // kit name in setup by name
        for (int i=0;i<(int)setups.size();i++) {
            if (!snc.compare(setups[i]->getName())) {
                for (int j;j<setups[i]->getNbKits();j++) {
                    if (!name.compare(setups[i]->getKit(j)->getName())) {
                        return setups[i]->getKit(j);
                    }
                }
            }
        }
    }                            // kit name in setup by id
    else if (snc == "" && sn != -1) {
        if (sn < (int)setups.size()) {
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
        for (int i=0;i<(int)setups.size();i++) {
            if (!snc.compare(setups[i]->getName())) {
                for (int j=0;j<setups[i]->getNbKits();j++) {
                    if (!name.compare(setups[i]->getKit(j)->getName())) {
                        return j;
                    }
                }
            }
        }
    }                            // kit name in setup by id
    else if (snc == "" && sn != -1) {
        if (sn < (int)setups.size()) {
            for (int j=0;j<setups[sn]->getNbKits();j++) {
                if (!name.compare(setups[sn]->getKit(j)->getName())) {
                    return j;
                }
            }
        }
    }
    return -1;
}

instrument* tapeutape::getInstrumentByName(string name, string snc, int sn, string knc, int kn)
{
    return NULL;
}

int tapeutape::getInstrumentIdByName(string name, string snc, int sn, string knc, int kn)
{
    if (snc != "" && sn == -1) { // if setup by name
        for (int i=0;i<(int)setups.size();i++) {
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

    if (sn < (int)setups.size() && sn != -1) {
        if (kn < setups[sn]->getNbKits() && kn != -1) {
            for (int i=0;i<setups[sn]->getKit(kn)->getNbInstruments(); i++) {
                if (!name.compare(setups[sn]->getKit(kn)->getInstrument(i)->getName())) {
                    return i;
                }
            }
        }
    }

    return -1;
}

void tapeutape::setInstrumentParameter(int sn, int kn, int in, int x_what, double x)
{
    if (x_what == VOLUME) setups[sn]->getKit(kn)->getInstrument(in)->setVolume(x);
    else if (x_what == PAN) setups[sn]->getKit(kn)->getInstrument(in)->setPan(x);
    else if (x_what == MIDITUNE) setups[sn]->getKit(kn)->getInstrument(in)->setRootNoteFine(x);
    else if (x_what == PLAYMODE) setups[sn]->getKit(kn)->getInstrument(in)->setPlayMode((int) x);
    else if (x_what == PLAYLOOP) setups[sn]->getKit(kn)->getInstrument(in)->setPlayLoop((int) x);
    else if (x_what == PLAYREVERSE) setups[sn]->getKit(kn)->getInstrument(in)->setPlayReverse((int) x);
    else if (x_what == PITCHOVERRANGE) setups[sn]->getKit(kn)->getInstrument(in)->setPitchOverRange((bool) x);

}

double tapeutape::getInstrumentParameter(int sn, int kn, int in, int x_what)
{
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

void tapeutape::playstopInstrument(int sn, int kn, int in, int x_what, const unsigned short t_velocity, float pitch)
{

    instrument* ins = setups[sn]->getKit(kn)->getInstrument(in);
    int noteOn = 0, vn;

    for (int i=0; i<ins->getNbVariations(); i++) {
        if ((ins->getVariation(i)->getMinVeloc() <= t_velocity)
        && (ins->getVariation(i)->getMaxVeloc() >= t_velocity)) {
            vn = i;
        }
    }

    if (x_what == PLAY) noteOn = 1;
    if (x_what == STOP) noteOn = 0;

    if (ins->getPlayReverse()) {
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
    if (ind>=0 && ind<(int)(samples.size()))
        return samples[ind];
    else
        return NULL;
}

sample* tapeutape::addSample(char* n,char *rn)
{
    std::string sampleName(n);
    sample *addedSample=NULL;
    //if there is a sample
    if (sampleName!="") {
        //test if the sample has already been added
        for(unsigned int i=0;i<samples.size();++i) {
            if (samples[i]->getAbsoluteName()==sampleName) {
                addedSample=samples[i];
                samples[i]->addUser();
            }
        }
        //if no, add it
        if (addedSample==NULL) {
            addedSample = new sample();
            if (addedSample->tryLoad(n,rn,fileName)) {
                delete addedSample;
                addedSample=NULL;
                showMessage(true,"Problem loading sample "+sampleName);
            }
            else {
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
    if (s->getNbUsers()>1) {
        s->removeUser();
    }
    else {
        //else look for the sample in the vector and remove it completely
        for(unsigned int i=0;i<samples.size();++i) {
            if (samples[i]->getAbsoluteName()==s->getAbsoluteName()) {
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

    for(unsigned int s=0;s<samples.size();++s) {
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
    if (jackStereoChannelsNames.size()>1) {
        jackStereoChannelsNames.erase(jackStereoChannelsNames.begin()+ind);
        //for each instrument, check if the number of the output is still available
        //if it isn't , give it the last output
        for(unsigned int i=0;i<setups.size();++i) {
            for(int j=0;j<setups[i]->getNbKits();++j) {
                for(int k=0;k<setups[i]->getKit(j)->getNbInstruments();++k) {
                    int outNum = setups[i]->getKit(j)->getInstrument(k)->getJackStereoChannel();
                    if (outNum>ind) {
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
    for(unsigned int i=0;i<jackStereoChannelsNames.size();++i) {
        if (n == getJackStereoChannelName(i))
            return i;
    }
    return -1;
}

void tapeutape::changeKit(int sn, int kn)
{
    int kit = setups[sn]->changeKit(kn);

    Fl::lock();
    execWin->changeKit(sn,kit+1);
    Fl::unlock();

    string mes = "Kit changed to " + to_string(kn) + ":'"  + setups[sn]->getKit(kn)->getName() + "' in Setup " + to_string(sn) + ":'" + setups[sn]->getName() + "'";
    showMessage(false, mes);
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
    Fl::lock();
    execWin->learnMidi(ch,ccn,vel);
    Fl::unlock();
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
    for(unsigned int i=0;i<setups.size();++i) {
        int cK = setups[i]->getCurrentKit();

        if (cc > 125) {
            // CC 126 set the instrument(s) bound to note "val" in Reverse mode
            // CC 127 set the instrument(s) bound to note "val" in Forward mode
            for(int j=0;j<setups[i]->getKit(cK)->getNbInstruments();j++) {
                int iChan = setups[i]->getKit(cK)->getInstrument(j)->getMidiChannel();
                int iMaxN = setups[i]->getKit(cK)->getInstrument(j)->getMaxNote();
                int iMinN = setups[i]->getKit(cK)->getInstrument(j)->getMinNote();
                if (iChan-1 == chan && val <= iMaxN && val >= iMinN) {
                    setups[i]->getKit(cK)->getInstrument(j)->setPlayReverse(127-cc);
                }
            }
        }
        else
        if (cc > 123) {
            // CC 125 set the instrument(s) bound to note "val" in Loop mode
            // CC 124 set the instrument(s) bound to note "val" in SingleShot mode

            for(int j=0;j<setups[i]->getKit(cK)->getNbInstruments();j++) {
                int iChan = setups[i]->getKit(cK)->getInstrument(j)->getMidiChannel();
                int iMaxN = setups[i]->getKit(cK)->getInstrument(j)->getMaxNote();
                int iMinN = setups[i]->getKit(cK)->getInstrument(j)->getMinNote();
                if (iChan-1 == chan && val <= iMaxN && val >= iMinN) {
                    setups[i]->getKit(cK)->getInstrument(j)->setPlayLoop(125-cc);
                }
            }
        }
        else
        if (cc > 121) {
            // CC 123 set the instrument(s) bound to note "val" in Normal mode
            // CC 122 set the instrument(s) bound to note "val" in Trigger mode

            for(int j=0;j<setups[i]->getKit(cK)->getNbInstruments();j++) {
                int iChan = setups[i]->getKit(cK)->getInstrument(j)->getMidiChannel();
                int iMaxN = setups[i]->getKit(cK)->getInstrument(j)->getMaxNote();
                int iMinN = setups[i]->getKit(cK)->getInstrument(j)->getMinNote();
                if (iChan-1 == chan && val <= iMaxN && val >= iMinN) {
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
        if (setups[i]->getChannel()-1 == chan && ipc < setups[i]->getNbKits()+1) changeKit(i, pc);
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

void tapeutape::parseOscSKI(string a, int ow)
{
    int pos = -1, nb_args=1;
    bool t;
    char sep = '/';
    if(a.find(':',pos+1) == 1) {
        t = a.substr(0,1).compare("s");
        pos = 2;
    }

    while (a.find(sep, pos+1) != string::npos) {
        pos = a.find(sep, pos+1);
        nb_args ++;
    }
    pos = 2;

    if (nb_args > 2) {
        snc = a.substr(pos,a.find(sep)-pos);
        pos = a.find(sep) + 1;
    }
    if (nb_args > 1) {
        if (ow == KIT) snc = a.substr(pos,a.find(sep, pos)-pos);
        else knc = a.substr(pos,a.find(sep, pos)-pos);
        pos = a.find(sep, pos) + 1;
    }
    if (nb_args > 0) {
        if (ow == KIT) knc = a.substr(pos,a.size()-pos);
        else inc = a.substr(pos,a.size()-pos);
    }

    if (t) {
        try { sn = stoi(snc); }
        catch (std::exception& e) { sn = -1; }
        try { kn = stoi(knc); }
        catch (std::exception& e) { kn = -1; }
        try { in = stoi(inc); }
        catch (std::exception& e) { in = -1; }
        snc = ""; knc = ""; inc = "";
    }
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
    if (v) execWin->setTitle(PACKAGE_NAME+" : "+getFileName());
}
