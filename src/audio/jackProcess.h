/***************************************************************************
 *            jackProcess.h
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

#ifndef _JACKPROCESS_H
#define _JACKPROCESS_H

#include <iostream>
#include <jack/jack.h>
#include <jack/ringbuffer.h>
#include <jack/types.h>
#include "audioEvent.h"
#include <cstdlib>
#include <vector>

class tapeutape;

class jackProcess
{
	public:
		jackProcess(tapeutape*, jack_ringbuffer_t*,int);
		~jackProcess();
		int init();
		int start();
		void addPort(std::string);
		void removePort(int);
		void renamePort(int,std::string);
		jack_client_t* getClient();
		int getSampleRate();
		friend int process(jack_nframes_t , void *);
		friend int sRate(jack_nframes_t , void *);
		friend void shutdown(void*);

		void addAudioEvent(const audioEvent&);

	protected:
		tapeutape* tap;
		jack_ringbuffer_t* EventsRingBuffer;
		jack_client_t *jackClient;
		int sampleRate;
		std::vector<audioEvent> audioEvents;
		int polyphonie;
		int nbJackStereoChannels;
		std::vector<jack_port_t*> outputPorts[2];

		std::vector<audioEvent> m_processRingBuffer;
		int m_processRingBufferReadPosition;
		int m_processRingBufferFilledCount;
		int m_processRingBufferWritePosition;
		int m_processRingBufferEmptyCount;
};


#endif
