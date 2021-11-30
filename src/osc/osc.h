/***************************************************************************
 *            osc.h
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


#include <lo/lo.h>
#include <string>
#include <map>


class OSCServer
{

public:

    OSCServer(const char* port);
    ~OSCServer();

    int protocol;


    void start();
    void stop();

    void add_method (const char* path, const char* types, lo_method_handler h, void* user_data = NULL);

    lo_server_thread serverThread;
    lo_server server;

    static void error(int num, const char *msg, const char *path)
    {
        printf("liblo server error %d in path %s: %s\n", num, path, msg);
        fflush(stdout);
    }

};