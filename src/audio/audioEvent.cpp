/***************************************************************************
 *            audioEvent.cpp
 *
 *  Copyright  2006  Florent Berthaut
 *  florentberthaut@no-log.org
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


#include "audioEvent.h"
using namespace std;


audioEvent::audioEvent()
{

}

audioEvent::audioEvent(variation* va,instrument* i,unsigned long d,double ps,int j,double v,double pl,double pr,bool ev)
:var(va),instru(i),id(d),pitch(ps),event(ev),offset(0),jackStereoChannel(j),volume(v),panLeft(pl),panRight(pr),fade(1.0),
m_playTrigger(instru->getPlayMode()),
m_playLoop(instru->getPlayLoop()),
m_playReverse(instru->getPlayReverse())
{
	if(instru->getPlayReverse()) {
		offset=var->getSample()->getFramesCount()-1;
	}
}
