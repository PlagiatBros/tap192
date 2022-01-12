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

#ifndef AUDIOEVENT_H
#define AUDIOEVENT_H

#include "../base/instrument.h"
#include <math.h>
#include <iostream>

class audioEvent                 //audio event triggered by midi
{
    public:
        audioEvent();
        audioEvent(variation*,instrument*,unsigned long,double,int,double,double,double,bool);
        inline variation* getVariation(){return var;}
        inline instrument* getInstrument(){return instru;}
        inline double getPitch(){return pitch;}
        inline double getPanLeft(){return panLeft;}
        inline double getPanRight(){return panRight;}
        inline unsigned long getID(){return id;}
        inline bool getEvent(){return event;}
        inline double getOffset(){return offset;}
        inline int getJackStereoChannel(){return jackStereoChannel;}
        //we add the pitch to the offset, if the play mode is repeat, we go back to the beginning of the sample
        inline void addOffset() {
            offset+=pitch;
            if(m_playLoop) {
                if(!m_playReverse && offset>=var->getSample()->getFramesCount()) {
                    //offset-=var->getSample()->getFramesCount();
                    offset=0;
                }
                else if(m_playReverse && offset<=0) {
                    offset=var->getSample()->getFramesCount()-1;
                }
            }
        }
        //we start fading out
        inline void fadeOut(int nframes) {
            fade=volume/1000.0;
        }

        inline int getPlayingLength(int nframes) {
            int length= (int)nframes;
            //if looping, always return the max nb of frames
            //if not looping
            if(!m_playLoop) {
                if(m_playReverse) {
                    length = (floor(((double)offset)/fabs(pitch)) < nframes) ? (int)floor(((double)offset)/fabs(pitch)) : (int)nframes;
                }
                else {
                    length = (floor(((double)(var->getSample()->getFramesCount()) - offset)/fabs(pitch)) < nframes )? (int)floor(((double)(var->getSample()->getFramesCount()) - offset)/fabs(pitch)) : (int)nframes;

                }
            }
            return length;
        }

        inline double getVolume() {
            //if we are fading out, we decrease the volume
            if(fade!=1.0)
                volume-=fade;
            return volume;
        }

    private:
        variation* var;
        instrument* instru;
        double pitch;
        unsigned long id;
        bool event;
        double volume;
        double panLeft;
        double panRight;
        double fade;
        int jackStereoChannel;
        double offset;
        bool m_playTrigger;
        bool m_playLoop;
        bool m_playReverse;
};
#endif
