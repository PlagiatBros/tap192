/***************************************************************************
 *            tap.cpp
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

#include "tap.h"


tap::tap(variation* va,instrument* i,double ps,double v,double pl,double pr):var(va),instru(i),pitch(ps),volume(v),panLeft(pl),panRight(pr)
{
}

tap::~tap()
{

}

std::string tap::getSampleName()
{
    sample* s = var->getSample();
    if(s)
    {
        return s->getName();
    }
    else
    {
        return "no sample yet";
    }
}
