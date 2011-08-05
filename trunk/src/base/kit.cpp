/***************************************************************************
 *            kit.cpp
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

#include "kit.h"
#include <math.h>

using namespace std;

kit::kit():m_volume(1),m_globalVolume(1)
{

}

kit::kit(const kit &k):name(k.name),m_volume(k.m_volume),m_globalVolume(k.m_globalVolume)
{
	//copy the instruments
	for(unsigned int i=0;i<k.instruments.size();++i) {
		instruments.push_back(new instrument(*(k.instruments[i])));
	}
	//recreate the taps
	exec(m_globalVolume);
}

kit::~kit()
{
	for(unsigned int i=0; i< instruments.size();++i)
		delete instruments[i];
	instruments.clear(); 
}

void kit::setName(std::string n)
{
	name = n;
}

std::string kit::getName()
{
	return name;
}

int kit::getNbInstruments()
{
	return instruments.size();
}

instrument* kit::getInstrument(int ind)
{
	if(ind<instruments.size() && ind>=0)
		return instruments[ind];
	else
		return NULL;
}

void kit::addInstrument(instrument * inst)
{
	instruments.push_back(inst);
}

void kit::removeInstrument(int inst)
{
	instrument* instru = instruments[inst];
	instruments.erase(instruments.begin()+inst);
	exec(m_globalVolume);
	delete instru;
}


void kit::exec(double globalVolume)
{
	m_globalVolume=globalVolume;
	//clear the taps
	for(int c=0;c<16;++c)
		for(int n=0;n<128;++n)
			for(int v=0;v<128;++v)
			{
				for(int t=0;t<taps[c][n][v].size();++t)
					delete taps[c][n][v][t];
				taps[c][n][v].clear();
			}

	for(int j=0;j<instruments.size();++j) //for each instrument in the kit
	{
		//compute the panning (Thanks Vincent Verfaille)
		double panLeft = sqrt(2)/2.0*(cos(instruments[j]->getPan()*(M_PI/4.0)) - sin(instruments[j]->getPan()*(M_PI/4.0)));
		double panRight = sqrt(2)/2.0*(cos(instruments[j]->getPan()*(M_PI/4.0)) + sin(instruments[j]->getPan()*(M_PI/4.0)));
		panRight=(panRight<0.0001)? 0 : panRight;
		panLeft=(panLeft<0.0001)? 0 : panLeft;
	
		for(unsigned short n=instruments[j]->getMinNote();  
			n<=instruments[j]->getMaxNote();n++)  //for each note of the instrument
		{
			//compute the offset to pitchShift (Thanks pete bessman) (temporary solution) 
			double pitch = 1;
			if(instruments[j]->getPitchOverRange())	
			 	pitch=pow(2.0, ((double)(n - instruments[j]->getRootNote()) + instruments[j]->getRootNoteFine())/12.0); 
			//FIXME TESTER FINE , voir dans SPECIMEN patch.c pour la fonction interpolate.


			for( int k=0;k<instruments[j]->getNbVariations();++k) //for each variation of the instrument
			{
				for(unsigned short v=instruments[j]->getVariation(k)->getMinVeloc(); //for each velocity of the variation
					v<=instruments[j]->getVariation(k)->getMaxVeloc();v++)
				{
					//we compute the volume (with the velocity, the instrument's volume and the global volume)
					double vol = m_globalVolume * compVolume(instruments[j]->getVolume(),v,instruments[j]->getVariation(k)->getMinVeloc(),instruments[j]->getVariation(k)->getMaxVeloc());
					//we create the corresponding tap in the array
					taps[instruments[j]->getMidiChannel()-1][n][v].push_back(new tap(instruments[j]->getVariation(k),instruments[j],pitch,vol,panLeft,panRight));
				}
			}
		}
	}
}

void kit::showTaps(int c)
{
		cout<<"channel "<<c<<endl<<endl;
		for(int n=0;n<11;++n)
		{
			cout<<"note "<<n<<endl;	
			for(int v=0;v<128;++v)
			{
				cout<<"veloc "<<v;
				for(int t=0;t<taps[c-1][n][v].size();++t)
					cout<<((vector<tap*>)taps[c-1][n][v])[t]->getVariation()->getSample()->getName()<<",";
				cout<<"|";		
			}
			cout<<endl;
		}
}

const vector<tap*>& kit::getTap(const unsigned short& channel, const unsigned short& note, const unsigned short& vel)
{
	return taps[channel][note][vel];
}

float kit::compVolume(float instruVol, unsigned short veloc, unsigned short minVeloc, unsigned short maxVeloc)
{
	return ((127.0 - ((float)maxVeloc - (float)veloc))/127.0)*instruVol*m_volume;
}
