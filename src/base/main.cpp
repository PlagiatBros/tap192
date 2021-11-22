/***************************************************************************
 *            main.cc
 *
 *  Mon Sep  4 10:12:59 2006
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "tapeutape.h"

#include "../nsm/nsm.h"

using namespace std;

string global_filename = "";

// nsm
bool global_nsm_gui = false;
bool nsm_opional_gui_support = true;
nsm_client_t *nsm = 0;
bool nsm_wait = true;
string nsm_folder = "";

int
nsm_save_cb(char **,  void *userdata)
{
    return ERR_OK;
}
void
nsm_hide_cb(void *userdata)
{
    global_nsm_gui = false;
}
void
nsm_show_cb(void *userdata)
{
    global_nsm_gui = true;
}
int
nsm_open_cb(const char *name, const char *display_name, const char *client_id, char **out_msg, void *userdata)
{
    nsm_wait = false;
    nsm_folder = name;
//    global_client_name = client_id;
    // NSM API 1.1.0: check if server supports optional-gui
//    nsm_opional_gui_support = strstr(nsm_get_session_manager_features(nsm), "optional-gui");
    mkdir(nsm_folder.c_str(), 0777);
    // make sure nsm server doesn't override cached visibility state
    nsm_send_is_shown(nsm);
    return ERR_OK;
}

int main(int argc, char** argv)
{
    // nsm
    const char *nsm_url = getenv( "NSM_URL" );
    if (nsm_url) {
        nsm = nsm_new();
        nsm_set_open_callback(nsm, nsm_open_cb, 0);
        if (nsm_init(nsm, nsm_url) == 0) {
            nsm_send_announce(nsm, "Tapeutape", ":dirty:", argv[0]);
        }
        int timeout = 0;
        while (nsm_wait) {
            nsm_check_wait(nsm, 500);
            timeout += 1;
            if (timeout > 200) exit(1);
        }

        tapeutape tap(argc,argv);
				global_filename = nsm_folder + "sampler.tap";
        std::ifstream infile(global_filename);
        if(!infile.good())
				    tap.save((char *)global_filename.c_str());
        else
            tap.load((char *)global_filename.c_str());
    }
		else
				tapeutape tap(argc,argv);

	#ifdef WITH_GUI
		Fl::lock();
		return Fl::run();
	#else
		return EXIT_SUCCESS;
	#endif
}
