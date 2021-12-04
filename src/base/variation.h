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
