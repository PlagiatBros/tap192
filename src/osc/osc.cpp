/***************************************************************************
 *            osc.cpp
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



#include "osc.h"

OSCServer::OSCServer( const char* port )
{
    protocol = std::string(port).find(std::string("osc.unix")) != std::string::npos ? LO_UNIX : LO_DEFAULT;

    if (protocol == LO_UNIX) {
        serverThread = lo_server_thread_new_from_url(port, error);
    } else {
        serverThread = lo_server_thread_new(port, error);
    }

    if (!serverThread) {
        exit(1);
    }

	server = lo_server_thread_get_server( serverThread );
}

OSCServer::~OSCServer()
{
    if (serverThread) {
        stop();
        lo_server_thread_free( serverThread );
    }
}

void OSCServer::start()
{
    lo_server_thread_start( serverThread );
}


void OSCServer::stop()
{
    lo_server_thread_stop( serverThread );
}


void OSCServer::add_method( const char* path, const char* types, lo_method_handler h, void* user_data)
{
    lo_server_thread_add_method( serverThread, path, types, h, user_data );
}
