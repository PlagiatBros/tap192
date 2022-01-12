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

#include "audioEvent.h"
using namespace std;

audioEvent::audioEvent()
{

}

audioEvent::audioEvent(variation* va,instrument* i,unsigned long d,double ps,int j,double v,double pl,double pr,bool ev):
    var(va),
    instru(i),
    pitch(ps),
    id(d),
    event(ev),
    volume(v),
    panLeft(pl),
    panRight(pr),
    fade(1.0),
    jackStereoChannel(j),
    offset(0),
    m_playTrigger(instru->getPlayMode()),
    m_playLoop(instru->getPlayLoop()),
    m_playReverse(instru->getPlayReverse())
{
    if (instru->getPlayReverse()) {
        offset=var->getSample()->getFramesCount()-1;
    }
}
