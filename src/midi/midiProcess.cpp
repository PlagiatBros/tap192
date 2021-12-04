// This file is part of tapeutape
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
#include "../base/tapeutape.h"
#include "../nsm/nsm.h"
#include "midiProcess.h"
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

extern nsm_client_t *nsm;

midiProcess::midiProcess(tapeutape *t,jack_client_t *j,int p,jack_ringbuffer_t* rb):tapeu(t),jackClient(j),midiPriority(p),EventsRingBuffer(rb),midiLearn(false)
{
}

midiProcess::~midiProcess()
{
}

int midiProcess::midiInit()
{
    //Open the ALSA midi sequencer handler
    if (snd_seq_open(&seqHandle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
        return(-1);
    }

    //Client's name
    if (nsm)
        snd_seq_set_client_name(seqHandle, nsm_get_client_id(nsm));
    else
        snd_seq_set_client_name(seqHandle, CLIENT_NAME);


    if ((midiInputPort = snd_seq_create_simple_port(seqHandle, "midi_in",
        SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
        return(-1);
    }

    return 0;
}

void midiProcess::startMidiLearn()
{
    midiLearn=true;
}

void midiProcess::stopMidiLearn()
{
    midiLearn=false;
}

void midiProcess::midiLoop()
{

    int npfd;
    struct pollfd *pfd;
    npfd = snd_seq_poll_descriptors_count(seqHandle, POLLIN);
    pfd = (struct pollfd *)alloca(npfd * sizeof(struct pollfd));
    snd_seq_poll_descriptors(seqHandle, pfd, npfd, POLLIN);
    while (1) {
        if (poll(pfd, npfd, 100000) > 0) {
            snd_seq_event_t *ev;

            do {
                //we get the event from the seqHandler
                snd_seq_event_input(seqHandle, &ev);
                snd_seq_ev_set_subs(ev);
                snd_seq_ev_set_direct(ev);

                //if learning midi control
                if (midiLearn) {
                    if (ev->type == SND_SEQ_EVENT_NOTEON && ev->data.note.velocity!=0) {
                        midiLearn=false;
                        tapeu->processMidiLearn(ev->data.note.channel+1,ev->data.note.note,ev->data.note.velocity);
                    }
                    else if (ev->type == SND_SEQ_EVENT_CONTROLLER) {
                        midiLearn=false;
                        tapeu->processMidiLearn(ev->data.control.channel+1,ev->data.control.param,0);
                    }
                }

                //if we have a note event, we put it in the ringbuffer
                if (ev->type == SND_SEQ_EVENT_NOTEON || ev->type==SND_SEQ_EVENT_NOTEOFF) {
                    //we get the corresponding vector of tap from each setup
                    for(int s=0;s<tapeu->getNbSetups();++s) {
                        const std::vector<tap*>& t= tapeu->getSetup(s)->getTap(ev->data.note.channel,ev->data.note.note,ev->data.note.velocity);
                                 //for each tap in it
                        for(unsigned int i=0;i<t.size();++i) {
                            //we create the audioEvent with the needed informations
                            bool noteOn;
                            if (ev->type == SND_SEQ_EVENT_NOTEON && ev->data.note.velocity!=0) {
                                noteOn=true;
                            }
                            else {
                                noteOn=false;
                            }
                            float pitch = t[i]->getPitch();
                            if (t[i]->getInstrument()->getPlayReverse()) {
                                pitch*=-1.0;
                            }
                            unsigned long id = ev->data.note.channel*1000+ev->data.note.note;
                            audioEvent ae(t[i]->getVariation(),t[i]->getInstrument(),id,pitch,t[i]->getJackStereoChannel(),t[i]->getVolume(),t[i]->getPanLeft(),t[i]->getPanRight(),noteOn);

                            tapeu->addAudioEvent(ae);
                            /*
                            //and put it in the ring buffer
                            jack_ringbuffer_write(EventsRingBuffer,(char*)ae,sizeof(audioEvent));
                            */

                        }
                    }
                }
                else
                //send to the different setups
                if (ev->type == SND_SEQ_EVENT_CONTROLLER) {
                    tapeu->processCC(ev->data.control.channel,ev->data.control.param,ev->data.control.value);
                }
                else
                //send to the different setups
                if (ev->type == SND_SEQ_EVENT_PGMCHANGE) {
                    tapeu->processPC(ev->data.control.channel, ev->data.control.value);
                }

                snd_seq_free_event(ev);
            } while (snd_seq_event_input_pending(seqHandle, 0) > 0);
        }
    }

}

void* midiProcess::threadExec(void* pthis)
{
    //gets a pointer to the current object
    midiProcess* pparent = (midiProcess*) pthis;
    pparent->midiLoop();         //starts the midi process loop
    pthread_exit(0);
}

int midiProcess::startThread()
{
    int res =jack_client_create_thread(jackClient,&midiThread,midiPriority,true,threadExec,this);
    if (res!=0) {
        return -1;
    }
    return 0;

}

void midiProcess::stopThread()
{
    pthread_cancel(midiThread);  //stops the midiThread
    snd_seq_close(seqHandle);
}

int midiProcess::getAlsaID()
{
    return snd_seq_client_id(seqHandle);
}
