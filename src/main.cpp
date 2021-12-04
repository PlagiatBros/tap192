// This file is part of tapeutape
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <getopt.h>

#include "package.h"
#include "base/tapeutape.h"
#include "nsm/nsm.h"

using namespace std;

char *global_oscport;
bool global_no_gui = false;

// struct for command parsing
const char* optstring = "p:nhv";
static struct
option long_options[] = {
    {"help",     0, 0, 'h'},
    {"osc-port", 1, 0, 'p'},
    {"no-gui",   0, 0, 'n'},
    {"version",  0, 0, 'v'},
    {0, 0, 0, 0}
};
static void usage(char *argv0)
{
    fprintf(stderr, PACKAGE_NAME);
    fprintf(stderr, "\nMidi & OSC controlable Sample Player\n\n");
    fprintf(stderr, "Usage: %s [options...]\n", argv0);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -p <int> , --osc-port=<int>                          udp in port number for OSC server\n");
    fprintf(stderr, "  -n , --no-gui                                        headless mode\n");
    fprintf(stderr, "  -h , --help                                          this usage output\n");
    fprintf(stderr, "  -v , --version                                       show version only\n");
}


// nsm
bool global_nsm_visible = false;
bool global_nsm_dirty = false;
bool global_nsm_opional_gui = false;
nsm_client_t *nsm = 0;
bool nsm_replied = false;
string nsm_folder = "";
string global_filename ="";

tapeutape * tap_instance;

int
nsm_save_cb(char **, void *userdata)
{
    tap_instance->save((char*)global_filename.c_str());
    nsm_send_is_clean(nsm);
    return ERR_OK;
}

void
nsm_hide_cb(void *userdata)
{
    global_nsm_visible = false;
}

void
nsm_show_cb(void *userdata)
{
    global_nsm_visible = true;
}

int
nsm_open_cb(const char *name, const char *display_name, const char *client_id, char **out_msg, void *userdata)
{
    nsm_replied = true;
    nsm_folder = name;
    // global_client_name = client_id;
    // NSM API 1.1.0: check if server supports optional-gui
    global_nsm_opional_gui = strstr(nsm_get_session_manager_features(nsm), "optional-gui");
    mkdir(nsm_folder.c_str(), 0777);
    // make sure nsm server doesn't override cached visibility state
    // nsm_send_is_shown(nsm);
    return ERR_OK;
}

int main(int argc, char** argv)
{
    // opts
    while (1)
    {
        int c, option_index=0;
        c = getopt_long(argc, argv, optstring, long_options, &option_index);

        if (c == -1) break;

        switch (c)
        {
            case 'p':
                global_oscport = optarg;
                break;
            case 'n':
                global_no_gui = true;
                break;
            case '?':
            case 'h':
                usage(argv[0]);
                return 0;
        }
    }
    if (optind < argc) global_filename = argv[optind++];

    // nsm
    const char *nsm_url = getenv("NSM_URL" );
    if (nsm_url) {
        nsm = nsm_new();
        nsm_set_open_callback(nsm, nsm_open_cb, 0);

        if (nsm_init_thread(nsm, nsm_url) == 0) {
            nsm_send_announce(nsm, PACKAGE_NAME, ":optional-gui:dirty:", argv[0]);
        }
        nsm_thread_start(nsm);
        usleep(500000);
        if (!nsm_replied) exit(1);

        if (global_nsm_opional_gui) {
            nsm_set_show_callback(nsm, nsm_show_cb, 0);
            nsm_set_hide_callback(nsm, nsm_hide_cb, 0);
        }

        tap_instance = new tapeutape((char *)global_filename.c_str());
        global_filename = nsm_folder + "/sampler.tap";
        std::ifstream infile(global_filename);
        if (!infile.good())
            tap_instance->save((char *)global_filename.c_str());
        else {
            tap_instance->load((char *)global_filename.c_str());
            tap_instance->start();
        }

        // register callbacks
        nsm_set_save_callback(nsm, nsm_save_cb, 0);
    }
    else
        tap_instance = new tapeutape((char *)global_filename.c_str());

    int r;
    Fl::lock();
    if (nsm) {
        if (!global_nsm_opional_gui) tap_instance->setVisible(true);
        while (true) {
            Fl::wait(0.1);
            if (tap_instance->isDirty() != global_nsm_dirty) {
                global_nsm_dirty = tap_instance->isDirty();
                if (global_nsm_dirty) nsm_send_is_dirty(nsm);
                else nsm_send_is_clean(nsm);
            }
            if (global_nsm_opional_gui && tap_instance->isVisible() != global_nsm_visible) {
                tap_instance->setVisible(global_nsm_visible);
                if (global_nsm_visible)nsm_send_is_shown(nsm);
                else nsm_send_is_hidden(nsm);
            }
        }
        r = EXIT_SUCCESS;
    } else {
        if (global_no_gui) {
            while (true) Fl::wait(0.1);
            r = EXIT_SUCCESS;
        } else {
            tap_instance->setVisible(true);
            r = Fl::run();
        }
    }

    if (nsm) nsm_free(nsm);
    delete tap_instance;

    return r;
}
