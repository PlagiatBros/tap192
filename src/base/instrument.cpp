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

#include "instrument.h"

using namespace std;

instrument::instrument():
    minNote(60),
    maxNote(60),
    rootNote(60),
    midiChannel(0),
    rootNoteFine(0),
    jackStereoChannel(0),
    volume(1.0),
    pan(0),
    pitchOverRange(true),
    m_playMode(0),
    m_playLoop(0),
    m_playReverse(0),
    playingCount(0),
    count(0),
    polyphony(0),
    cut(-1)
{
}

instrument::instrument(const instrument &inst):
    name(inst.name),
    minNote(inst.minNote),
    maxNote(inst.maxNote),
    rootNote(inst.rootNote),
    midiChannel(inst.midiChannel),
    rootNoteFine(inst.rootNoteFine),
    jackStereoChannel(inst.jackStereoChannel),
    volume(inst.volume),
    pan(inst.pan),
    pitchOverRange(inst.pitchOverRange),
    m_playMode(inst.m_playMode),
    m_playLoop(inst.m_playLoop),
    m_playReverse(inst.m_playReverse),
    playingCount(0),
    count(0),
    polyphony(inst.polyphony),
    cut(inst.cut)
{
    for(unsigned int v=0;v<inst.variations.size();++v) {
        variations.push_back(new variation(*(inst.variations[v])));
    }
};

instrument::~instrument()
{
    for(unsigned int i=0;i<variations.size();++i)
        delete variations[i];
    variations.clear();
}

variation* instrument::getVariation(int ind)
{
    if (ind<(int)variations.size() && ind>=0)
        return variations[ind];
    else
        return NULL;
}

int instrument::getNbVariations()
{
    return variations.size();
}

void instrument::addVariation(variation* v)
{
    variations.push_back(v);
    //if first variation, make sure it begins at 0
    if (variations.size()==1)
        variations[0]->setMinVeloc(0);
}

void instrument::moveVariation(bool down, int var)
{
    variation* swapVar;
    unsigned short swapVal;

    if (down) { //moving towards the end (->127)
        //if not the last var
        if (var<(int)variations.size()-1) {
            swapVal = variations[var+1]->getMaxVeloc();
            variations[var+1]->setMaxVeloc(variations[var]->getMaxVeloc());
            variations[var]->setMaxVeloc(swapVal);
            swapVal = variations[var+1]->getMinVeloc();
            variations[var+1]->setMinVeloc(variations[var]->getMinVeloc());
            variations[var]->setMinVeloc(swapVal);

            swapVar = variations[var+1];
            variations[var+1] = variations[var];
            variations[var] = swapVar;
        }
    }
    else { //moving towards the beginning (->0)
        if (var>0) {
            //if not the first var
            swapVal = variations[var-1]->getMaxVeloc();
            variations[var-1]->setMaxVeloc(variations[var]->getMaxVeloc());
            variations[var]->setMaxVeloc(swapVal);
            swapVal = variations[var-1]->getMinVeloc();
            variations[var-1]->setMinVeloc(variations[var]->getMinVeloc());
            variations[var]->setMinVeloc(swapVal);

            swapVar = variations[var-1];
            variations[var-1] = variations[var];
            variations[var] = swapVar;
        }
    }
}

void instrument::updateVariations(int var)
{
    //arrange according to one var
    if (var>-1 && var < (int)variations.size()) {
        //remap the min and max veloc according to the variation given
        if (var==0)
            variations[0]->setMinVeloc(0);
        else
            variations[var-1]->setMaxVeloc(variations[var]->getMinVeloc()+1);

        if (var==(int)variations.size()-1)
            variations[var]->setMaxVeloc(127);
        else
            variations[var+1]->setMinVeloc(variations[var]->getMaxVeloc());
    }
    else { //rearrange all the vars
        int step = (int)(ceil(127.0/(double)(variations.size())));
        for(unsigned int v=0;v<variations.size();++v) {
            int min = (v==0)?0:v*step+1;
            int max = ((v+1)*step>127)?127:(v+1)*step;
            variations[v]->setMinVeloc(min);
            variations[v]->setMaxVeloc(max);
        }
    }
}

void instrument::removeVariation(int v)
{
    delete    variations[v];
    variations.erase(variations.begin()+v);
}

std::string instrument::getName()
{
    return name;
}

void instrument::setName(std::string n)
{
    name=n;
}

void instrument::setMidi(short com,short chan,short min,short root,short max,float fine)
{
    switch(com) {
        case 0:
        {
            minNote=min;
            rootNote=root;
            rootNoteFine=fine;
            maxNote=max;
            midiChannel=(chan>0)?chan-1:0;
        }
        break;
        default:break;
    }
}

void instrument::setMinNote(unsigned short mn)
{
    minNote=mn;
}

void instrument::setMaxNote(unsigned short mn)
{
    maxNote=mn;
}

void instrument::setMidiChannel(unsigned short c)
{
    midiChannel=(c>0)?c-1:0;
}

void instrument::setRootNote(unsigned short rn)
{
    rootNote = rn;
}

void instrument::setRootNoteFine(float rnf)
{
    rootNoteFine = rnf;
}

void instrument::setVolume(double v)
{
    volume = v;
}

void instrument::setPan(double p)
{
    pan = p;
}

void instrument::setPitchOverRange(bool por)
{
    pitchOverRange = por;
}

unsigned short instrument::getMinNote()
{
    return minNote;
}

unsigned short instrument::getMaxNote()
{
    return maxNote;
}

unsigned short instrument::getMidiChannel()
{
    return midiChannel+1;
}

unsigned short instrument::getRootNote()
{
    return rootNote;
}

float instrument::getRootNoteFine()
{
    return rootNoteFine;
}

void instrument::setJackStereoChannel(int j)
{
    jackStereoChannel=j;
}

bool instrument::getPitchOverRange()
{
    return pitchOverRange;
}
