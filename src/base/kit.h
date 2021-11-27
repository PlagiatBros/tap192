/***************************************************************************
 *            kit.h
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _KIT_H
#define _KIT_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "instrument.h"
#include "tap.h"


class kit
{
	public:
		kit();
		kit(const kit&);
		~kit();
		void setName(std::string);
		std::string getName();
		bool isFilled();
		int getNbInstruments();
		instrument* getInstrument(int);
		void addInstrument(instrument*);
		void removeInstrument(int);
		const std::vector<tap*>& getTap(const unsigned short&, const unsigned short&, const unsigned short&);
		void showTaps(int);
		float compVolume(float,unsigned short,unsigned short,unsigned short);
		void exec(double);

		inline void setVolume(double vol){m_volume=vol;}
		inline double getVolume(){return m_volume;}

	private:
		std::vector<instrument*> instruments;
		std::string name;
		std::vector<tap*> taps[16][128][128];
		double m_volume;
		double m_globalVolume;

};



#endif /* _KIT_H */
