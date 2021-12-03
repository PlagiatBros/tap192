/***************************************************************************
 *            variation.cpp
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "variation.h"
#include <alsa/asoundlib.h>
#include "tapeutape.h"
#include <math.h>

using namespace std;

variation::variation():minVeloc(0),maxVeloc(127),sound(NULL)
{
}

variation::variation(const variation &var):sound(var.sound),minVeloc(var.minVeloc),maxVeloc(var.maxVeloc)
{
    if (sound != NULL) sound->addUser();
}

variation::~variation()
{
}


void variation::setMidi(short com,short chan,short min,short max)
{
    switch(com)
    {
        case 0://velocity range
        {
            minVeloc=min;
            maxVeloc=max;
        }
        break;
        default:break;
    }
}

void variation::setMaxVeloc(unsigned short vmax)
{
     maxVeloc=vmax;
}

void variation::setMinVeloc(unsigned short vmin)
{
     minVeloc=vmin;
}

unsigned short variation::getMaxVeloc()
{
     return maxVeloc;
}

unsigned short variation::getMinVeloc()
{
     return minVeloc;
}

int variation::setSample(sample* s)
{
    sound = s;
    if(!s)
    {
        return -1;
    }
    return 0;
}

sample* variation::getSample()
{
    return sound;
}

void variation::getFrame(const int& channel, const double& offset, jack_default_audio_sample_t& frame)
{
    if(sound) {
        return sound->getFrame(channel,offset, frame);
    }
    else {
        frame=0;
    }
}
