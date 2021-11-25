/***************************************************************************
 *           	lashClient.h
 *
 *  Copyright  2007 - 2013 Florent Berthaut, 2019 Jean-Emmanuel Doucet & Aurélien Roux
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

#ifndef _LASHCLIENT_H
#define _LASHCLIENT_H

#include <iostream>
#include <lash/lash.h>
#include <cstdlib>
#include <vector>

class tapeutape;

class lashClient
{
	public:
		lashClient(tapeutape*,int,char**);
		~lashClient();
		int init();
		int start();
		int stop();
		void setJack();
		void setAlsa(int);
		friend void* process(void *arg);
	protected:
		tapeutape* tap;
		int alsaID;
		lash_args_t* lash_args;
		lash_client_t* client;
		pthread_t lashThread;
};


#endif
