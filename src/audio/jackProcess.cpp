/***************************************************************************
 *            jackProcess.cpp
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

#include "jackProcess.h"
#include "../base/tapeutape.h"
#include <iostream>
#include <cmath>

#include "../nsm/nsm.h"
extern nsm_client_t *nsm;

using namespace std;

int process(jack_nframes_t nframes, void *arg)
{
    jackProcess* proc = (jackProcess*) arg;
    int nbOutputs=proc->nbJackStereoChannels;

    //WE GET THE OUTPUT PORTS
    jack_default_audio_sample_t *out[2][nbOutputs];
    for(int i=0;i<nbOutputs;++i) {
        out[0][i] = (jack_default_audio_sample_t *)jack_port_get_buffer (proc->outputPorts[0][i], nframes);
        out[1][i] = (jack_default_audio_sample_t *)jack_port_get_buffer (proc->outputPorts[1][i], nframes);
    }

    //WE GET ALL THE NEW AUDIO EVENTS
    while(proc->m_processRingBufferFilledCount>0) {
        proc->m_processRingBufferFilledCount--;
        //get the new event
        audioEvent& newEvent = proc->m_processRingBuffer[proc->m_processRingBufferReadPosition];

        if(newEvent.getEvent()==true) {
            bool stoppingLoop=false;

            //if we re in trigger mode and looping, we go through the events and stop the corresponding one
            if(newEvent.getInstrument()->getPlayMode()==1 && newEvent.getInstrument()->getPlayLoop()==1) {
                for(unsigned int a=0;a<proc->audioEvents.size();++a) {
                    if(proc->audioEvents[a].getID() == newEvent.getID()) {
                        proc->audioEvents[a].fadeOut(nframes);
                        stoppingLoop=true;
                    }
                }
            }
            if(!stoppingLoop) {
                //if size left
                if((int)(proc->audioEvents.size()) < proc->polyphonie) {
                    //we inform the sample that it's going to be played, and if possible (not cutting)
                    if(newEvent.getInstrument()->addPlaying()==0) {
                        //we add it to the list of audioEvents of its jack Channel
                        proc->audioEvents.push_back(newEvent);
                    }
                }
            }
        }
        else { //if it's a noteOff
            //we go through all the events and stop the one with the same id , of course if the play mode is not "trigger"
            for(unsigned int a=0;a<proc->audioEvents.size();++a) {
                if(proc->audioEvents[a].getID() == newEvent.getID() && newEvent.getInstrument()->getPlayMode()==0) {
                    proc->audioEvents[a].fadeOut(nframes);
                }
            }
        }


        proc->m_processRingBufferReadPosition = (proc->m_processRingBufferReadPosition+1)%proc->m_processRingBuffer.size();
        proc->m_processRingBufferEmptyCount++;
    }

/*
    while(jack_ringbuffer_read_space(proc->EventsRingBuffer)) { //while there are still events waiting
        //we get the next event
        audioEvent* ae = (audioEvent*) malloc(sizeof(audioEvent));
        jack_ringbuffer_read(proc->EventsRingBuffer,(char*)ae,sizeof(audioEvent));

        //if it's a noteon
        if((bool)((audioEvent*)ae->getEvent())==true) {
            bool stoppingLoop=false;

            //if we re in trigger mode and looping, we go through the events and stop the corresponding one
            if(((audioEvent*)ae)->getInstrument()->getPlayMode()==1 && ((audioEvent*)ae)->getInstrument()->getPlayLoop()==1) {
                for(unsigned int a=0;a<proc->audioEvents.size();++a) {
                    if(proc->audioEvents[a]->getID() == ((audioEvent*)ae)->getID()) {
                        proc->audioEvents[a]->fadeOut(nframes);
                        stoppingLoop=true;
                    }
                }
            }
            if(!stoppingLoop) {
                //if size left
                if((int)(proc->audioEvents.size()) < proc->polyphonie) {
                    //we inform the sample that it's going to be played, and if possible (not cutting)
                    if((audioEvent*)ae->getInstrument()->addPlaying()==0)
                    {
                        //we add it to the list of audioEvents of its jack Channel
                        proc->audioEvents.push_back(audioEvent(((audioEvent*)ae)->getVariation(),((audioEvent*)ae)->getInstrument(),((audioEvent*)ae)->getID(),((audioEvent*)ae)->getPitch(),((audioEvent*)ae)->getJackStereoChannel(),((audioEvent*)ae)->getVolume(),((audioEvent*)ae)->getPanLeft(),((audioEvent*)ae)->getPanRight(),((audioEvent*)ae)->getEvent()));

                    }
                }
            }
        }
        else//if it's a noteOff {
            //we go through all the events and stop the one with the same id , of course if the play mode is not "trigger"
            for(unsigned int a=0;a<proc->audioEvents.size();++a) {
                if(proc->audioEvents[a].getID() == ((audioEvent*)ae)->getID() && ((audioEvent*)ae)->getInstrument()->getPlayMode()==0) {
                    proc->audioEvents[a].fadeOut(nframes);
                }
            }
        }

        delete(ae);
    }
*/


    //WE TEST THE CUTS

    //for every audioEvent
    for(unsigned int ae=0;ae<proc->audioEvents.size();++ae) {
        //test the cut by and start to fade out if needed
        if(proc->audioEvents[ae].getInstrument()->isCut()) {
            proc->audioEvents[ae].fadeOut(nframes);
        }
    }

    //WE PUT THE CHANNELS BUFFERS TO 0

    //for each stereo channel
    for(int chan=0;chan<proc->nbJackStereoChannels;++chan) {
        memset(out[0][chan],0.0,nframes*sizeof(jack_default_audio_sample_t));
        memset(out[1][chan],0.0,nframes*sizeof(jack_default_audio_sample_t));
    }

    //THEN WE PLAY THE SAMPLES IN THE LIST

    //for every audioEvent
    for(unsigned int ae=0;ae<proc->audioEvents.size();++ae) {
        //we test if the sound finishes here and we then get its length
        jack_nframes_t length = proc->audioEvents[ae].getPlayingLength(nframes);

        //then for the remaining length
        for(jack_nframes_t ind=0;ind<length;++ind) {

            //FIXME interpolation des offsets
            //we play the sample (in the corresponding channels)
            jack_default_audio_sample_t frame;
            proc->audioEvents[ae].getVariation()->getFrame(0,proc->audioEvents[ae].getOffset(), frame);
            out[0][proc->audioEvents[ae].getJackStereoChannel()][ind]+= frame* proc->audioEvents[ae].getVolume() * proc->audioEvents[ae].getPanLeft() ;
            proc->audioEvents[ae].getVariation()->getFrame(1,proc->audioEvents[ae].getOffset(), frame);
            out[1][proc->audioEvents[ae].getJackStereoChannel()][ind]+=frame * proc->audioEvents[ae].getVolume() * proc->audioEvents[ae].getPanRight() ;

            //and we ++ the offset
            proc->audioEvents[ae].addOffset();
        }

        if(length < nframes || proc->audioEvents[ae].getVolume()<=0) { // if the sound is finished
            proc->audioEvents[ae].getInstrument()->removePlaying();
            proc->audioEvents.erase(proc->audioEvents.begin()+ae);
            --ae;
        }
    }
    return 0;
}

int sRate (jack_nframes_t nframes, void *arg)
{
    return 0;
}

void shutdown(void *arg)
{
    jackProcess* proc = (jackProcess*) arg;
    proc->tap->showMessage(true,"Jack ... killed us");
    proc->jackClient=NULL;
}



jackProcess::jackProcess(tapeutape* t,jack_ringbuffer_t* rb, int poly):tap(t),EventsRingBuffer(rb),polyphonie(poly)
{
}

jackProcess::~jackProcess()
{
    jack_deactivate(jackClient);
    for(int i=0;i<nbJackStereoChannels;++i)
    {
            jack_port_unregister (jackClient,outputPorts[0][i]);
            jack_port_unregister (jackClient,outputPorts[1][i]);
    }
    outputPorts[0].clear();
    outputPorts[1].clear();
    jack_client_close(jackClient);
}

void jackProcess::addAudioEvent(const audioEvent& newEvent)
{
    if(m_processRingBufferEmptyCount>0) {
        m_processRingBufferEmptyCount--;
        m_processRingBuffer[m_processRingBufferWritePosition]=newEvent;
        m_processRingBufferWritePosition=(m_processRingBufferWritePosition+1)%m_processRingBuffer.size();
        m_processRingBufferFilledCount++;
    }
}

void jackProcess::addPort(std::string st)
{
        outputPorts[0].push_back(jack_port_register (jackClient, (st+"-L").c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0));
        outputPorts[1].push_back(jack_port_register (jackClient, (st+"-R").c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0));
}

void jackProcess::removePort(int num)
{
    jack_port_unregister (jackClient,outputPorts[0][num]);
    jack_port_unregister (jackClient,outputPorts[1][num]);
    outputPorts[0].erase(outputPorts[0].begin()+num);
    outputPorts[1].erase(outputPorts[1].begin()+num);
}

void jackProcess::renamePort(int num,std::string st)
{
    jack_port_rename(jackClient, outputPorts[0][num],(st+"-L").c_str());
    jack_port_rename(jackClient, outputPorts[1][num],(st+"-R").c_str());
}

int jackProcess::init()
{
    //ring buffer
    m_processRingBuffer.resize(100,audioEvent());
    m_processRingBufferReadPosition=0;
    m_processRingBufferWritePosition=0;
    m_processRingBufferEmptyCount=100;
    m_processRingBufferFilledCount=0;

    //jack client creation
    jackClient=NULL;
    if(nsm)
        jackClient = jack_client_open(nsm_get_client_id(nsm),JackUseExactName,NULL);
    else
        jackClient = jack_client_open("Tapeutape",JackUseExactName,NULL);

    if(!jackClient)
    {
        tap->showMessage(true,"Error creating the jackClient");
        return -1;
    }
    sampleRate = jack_get_sample_rate(jackClient);
    tap->setSampleRate(sampleRate);

    return 0;
}


int jackProcess::start()
{

    if(outputPorts[0].size()>0) //if ports already created
    {
        //delete everything
        for(int i=0;i<outputPorts[0].size();++i)
        {
            jack_port_unregister (jackClient,outputPorts[0][i]);
            jack_port_unregister (jackClient,outputPorts[1][i]);
        }
        outputPorts[0].clear();
        outputPorts[1].clear();
    }

    jack_deactivate(jackClient);

    //we create the output ports
    nbJackStereoChannels = tap->getNbJackStereoChannels();
    for(int i=0;i<nbJackStereoChannels;++i)
    {
            std::string st = tap->getJackStereoChannelName(i);
            outputPorts[0].push_back(jack_port_register (jackClient, (st+"-L").c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0));
            outputPorts[1].push_back(jack_port_register (jackClient, (st+"-R").c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0));
    }

    //sample rate callback
    //jack_set_sample_rate_callback (jackClient, sRate, this);

    //process callback
    jack_set_process_callback (jackClient, process, this);

    //on shutdown
    jack_on_shutdown(jackClient,shutdown,this);

    //activate the client
    if(jack_activate(jackClient))
    {
        tap->showMessage(true,"Cannot activate jack client, jackd not running ?");
        return -1;
    }


    return 0 ;
}



jack_client_t* jackProcess::getClient()
{
    return jackClient;
}

int jackProcess::getSampleRate()
{
    return sampleRate;
}
