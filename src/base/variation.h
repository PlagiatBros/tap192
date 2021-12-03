/***************************************************************************
 *            variation.h
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

#ifndef variation_h
#define variation_h

#include <iostream>
#include <string.h>
#include "../audio/sample.h"


class variation
{

    public:
        variation();
        variation(const variation &);
        ~variation();
        void setMidi(short,short,short,short);
        void setMaxVeloc(unsigned short);
        void setMinVeloc(unsigned short);
        unsigned short getMaxVeloc();
        unsigned short getMinVeloc();
        int setSample(sample*);
        sample* getSample();
        void getFrame(const int&, const double&, jack_default_audio_sample_t&);

    private:
        unsigned short maxVeloc,minVeloc;
        sample *sound;
};
#endif
