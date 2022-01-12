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

#ifndef _INSTRUMENT_H
#define _INSTRUMENT_H

#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>
#include "variation.h"

class tapeutape;

class instrument
{
    public:
        instrument();
        instrument(const instrument &);
        ~instrument();
        variation* getVariation(int);
        int getNbVariations();
        void addVariation(variation*);
        void removeVariation(int);
        void moveVariation(bool,int);
        void updateVariations(int);
        std::string getName();
        void setName(std::string);
        void setMinNote(unsigned short);
        void setMidi(short,short,short,short,short,float);
        void setMaxNote(unsigned short);
        void setVolume(double);
        void setPan(double);
        void setMidiChannel(unsigned short);
        void setRootNote(unsigned short);
        void setRootNoteFine(float);
        void setJackStereoChannel(int);
        void setPitchOverRange(bool);
        inline void setPlayMode(int pm){m_playMode=pm;}
        inline void setPlayLoop(int pl){m_playLoop=pl;}
        inline void setPlayReverse(int pr){m_playReverse=pr;}
        unsigned short getMinNote();
        unsigned short getMaxNote();
        unsigned short getRootNote();
        float getRootNoteFine();
        unsigned short getMidiChannel();
        bool getPitchOverRange();
        inline int getPlayMode(){return m_playMode;}
        inline int getPlayLoop(){return m_playLoop;}
        inline int getPlayReverse(){return m_playReverse;}

        //used in the exec mode
        inline unsigned int getPolyphony(){return polyphony;};
        inline void setPolyphony(unsigned int p){polyphony=p;playingCount=0;};
        inline unsigned int getCut(){return cut;};
        inline void setCut(unsigned int c){cut=c;};
        inline int addPlaying() {
            if(polyphony>0) {
                //if we are cutting voices
                if(count) {
                    return -1;
                }
                //else
                ++playingCount;
                //if polyphony number reached
                if(playingCount>polyphony)
                    count=true;
            }
            return 0;
        };
        inline void removePlaying(){if(playingCount>0)--playingCount;};
        inline bool isCut() {
            bool ans=false;
            //if the cut process is started
            if(count) {
                ans=true;
                count=false;
            }

            return ans;
        };
        inline const int& getJackStereoChannel(){return jackStereoChannel;};
        inline double getVolume(){return volume;};
        inline double getPan(){return pan;};

    private:
        std::vector<variation*> variations;
        std::string name;
        unsigned short minNote,maxNote,rootNote,midiChannel;
        float rootNoteFine;
        int jackStereoChannel;
        double volume,pan;
        bool pitchOverRange;
        int m_playMode;
        int m_playLoop;
        int m_playReverse;

        //used in the exec mode
        unsigned int playingCount;
        bool count;
        unsigned int polyphony;
        int cut;
};
#endif
