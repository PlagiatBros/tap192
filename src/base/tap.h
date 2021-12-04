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

#ifndef _TAP_H
#define _TAP_H

#include <iostream>
#include "instrument.h"

class tap
{
    public:
        tap(variation*,instrument*,double,double,double,double);
        ~tap();
        inline double getPitch(){return pitch;};
        inline double getVolume(){return volume;};
        inline double getPanLeft(){return panLeft;};
        inline double getPanRight(){return panRight;};
        std::string getSampleName();
        inline variation* getVariation(){return var;};
        inline instrument* getInstrument(){return instru;};
        inline int getJackStereoChannel(){return instru->getJackStereoChannel();};
    private:
        variation* var;
        instrument* instru;
        double volume;
        double panLeft;
        double panRight;
        double pitch;
};
#endif
