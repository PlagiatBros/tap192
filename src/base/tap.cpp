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

#include "tap.h"

tap::tap(variation* va,instrument* i,double ps,double v,double pl,double pr):
    var(va),
    instru(i),
    volume(v),
    panLeft(pl),
    panRight(pr),
    pitch(ps)
{
}

tap::~tap()
{

}

std::string tap::getSampleName()
{
    sample* s = var->getSample();
    if (s) {
        return s->getName();
    }
    else {
        return "no sample yet";
    }
}
