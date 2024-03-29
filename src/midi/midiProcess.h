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

#ifndef MIDIPROCESS_H
#define MIDIPROCESS_H

#include <alsa/asoundlib.h>
#include <jack/jack.h>
#include <jack/thread.h>
#include <jack/ringbuffer.h>
#include "../audio/audioEvent.h"

class tapeutape;

class midiProcess
{
    public:
        midiProcess(tapeutape*,jack_client_t*, int,jack_ringbuffer_t*);
        ~midiProcess();
                                 //midiThread method
        static void* threadExec(void* );
        int midiInit();          //midi init
        void midiLoop();         //midi process loop
        void stopThread();       //stops the midiThread
        int startThread();       //starts the midiThread
        void startMidiLearn();
        void stopMidiLearn();
        int getAlsaID();
    private:
        tapeutape* tapeu;
        pthread_t midiThread;    //midiThread ID
        snd_seq_t *seqHandle;    //sequencer handle
        int midiInputPort;       //midi input port
        jack_client_t* jackClient;
        int midiPriority;        //realtime priority for the thread
        jack_ringbuffer_t *EventsRingBuffer;
        bool midiLearn;
};
#endif
