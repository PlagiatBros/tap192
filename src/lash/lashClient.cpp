/***************************************************************************
 *            lashClient.cpp
 *
 *  Copyright  2007  Florent Berthaut
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

#include "lashClient.h"
#include "../base/tapeutape.h"
#include <iostream>
#include <math.h>

using namespace std;

void* process(void *arg)
{

	lashClient* lashC = (lashClient*)arg;

	while(1)
	{
		lash_event_t*  event  = NULL;
		lash_config_t* config = NULL;
		const char*    str    = NULL;
    
		while ((event = lash_get_event(lashC->client)))
		{
			if (lash_event_get_type(event) == LASH_Quit)
			{
				//quit
				lashC->tap->stop();
				exit(0);
			}
			else if (lash_event_get_type(event) == LASH_Save_File)
			{
				//get the path of the file
				str = lash_event_get_string(event);
				std::string path(str);
				path+="/config.tap";
				size_t size = path.size() + 1;
				char * cpath = new char[ size ];
				strncpy( cpath, path.c_str(), size );

				//save the file
				lashC->tap->showMessage(false,"Saved As "+path);
				lashC->tap->saveWithSamples(cpath,str);
				delete [] cpath;
				
				//send the confirmation
				lash_event_t* conf = lash_event_new_with_type (LASH_Save_File);
				lash_send_event (lashC->client, conf);

			}
			else if (lash_event_get_type(event) == LASH_Restore_File)
			{
				//get the path of the file
				str = lash_event_get_string(event);
				std::string path(str);
				path+="/config.tap";
				size_t size = path.size() + 1;
				char * cpath = new char[ size ];
				strncpy( cpath, path.c_str(), size );
				lashC->tap->showMessage(false,"Restored from "+path);

				//open the file
				lashC->tap->load(cpath);
				lashC->tap->start();
				delete [] cpath;
				
				//send the confirmation
				lash_event_t* conf = lash_event_new_with_type (LASH_Restore_File);
				lash_send_event (lashC->client, conf);
			}
			lash_event_destroy(event);
		}
		sleep(1);
	}
}

lashClient::lashClient(tapeutape* t,int argc,char** argv):tap(t)
{
	lash_args = lash_extract_args(&argc, &argv);
}

lashClient::~lashClient()
{
}

int lashClient::init()
{


	client = lash_init(lash_args, "Tapeutape", LASH_Config_File, LASH_PROTOCOL(2, 0));
	if (!client)
		return -1;

	//tell lash the client name
	lash_event_t* event = lash_event_new_with_type(LASH_Client_Name);
	lash_event_set_string(event, "Tapeutape");
	lash_send_event(client, event);

	return 0 ;
}

void lashClient::setAlsa(int id)
{
	//tell lash the alsa Client id
	lash_alsa_client_id(client,id);
}

void lashClient::setJack()
{
	//tell lash the jack client name
	lash_jack_client_name(client,"Tapeutape");
}


int lashClient::start()
{
	pthread_create(&lashThread,0,process,this);
	return 0;
}

int lashClient::stop()
{
	return 0;
}



