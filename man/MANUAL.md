tapeutape(1) -- MIDI- & OSC-controlable audiosampler
=============================

## SYNOPSIS

`tapeutape` [OPTION...] filename.tap

## DESCRIPTION

tapeutape is a lightweight, MIDI- and OSC-controlable, NSM-compatible audiosampler.

## OPTIONS

* `-h, --help`:
    Show available options

* `-p, --osc-port` <port>:
    OSC input port (udp port number or unix socket path)

* `-n, --no-gui`:
    Enable headless mode

* `-v, --version`:
    Show version and exit

## USER INTERFACE

**Setup**

**Kit**

**Instrument**

**Variation**

## OSC CONTROLS

**Setup**
* `/set/global_volume` <double_or_float_or_int: volume>:
    Set tapeutape global volume.

* `/get/global_volume` (optional: <string: osc_address>):
    Send tapeutape global volume on the specified osc_address. If osc_address is not defined, tapeutape will send the global volume back to the address from which it received the osc message.

* `/get/setups_list` (optional: <string: osc_address>):
    Send current setups list on the specified osc_address. If osc_address is not defined, tapeutape will send the setups list back to the address from which it received the osc message.

* `/setup/get/kits_list` <string: setup> (optional: <string: osc_address>):
    Send setup's kits list on the specified osc_address. If osc_address is not defined, tapeutape will send the kits list back to the address from which it received the osc message.

**Kit**
* `/kit/select` (optional <int_or_string: setup>) <int_or_string: kit>:         
    Select specified kit as current kit (Execution Window)
    ID or name can be used to defined setup and kit.
    If the setup is not defined, matching kit(s) will be selected in every setup(s) in which there is a matching kit.

* `/kit/set/selected` (optional <int_or_string: setup>) <int_or_string: kit>:         
    Same as `/kit/select`.

* `/kit/get/selected` (optional: <string: osc_address>):
    Send selected kit ID for each setup on the specified osc_address. If osc_address is not defined, tapeutape will send the kits list back to the address from which it received the osc message.
    Received message should look like:
    /tapeutape/get/selected SETUP_ID_1 SELECTED_KIT_ID SETUP_ID_2 SELECTED_KIT_ID ...

* `/kit/get/selected/by_name` (optional: <string: osc_address>):
    Send selected kit name for each setup on the specified osc_address. If osc_address is not defined, tapeutape will send the kits list back to the address from which it received the osc message.
    Received message should look like:
    /tapeutape/get/selected SETUP_NAME_1 SELECTED_KIT_NAME SETUP_NAME_2 SELECTED_KIT_NAME ...

* `/kit/set/volume` (optional <int_or_string: setup>) <int_or_string: kit> <double_or_float_or_int: volume>:
    Set kit volume.
    ID or name can be used to defined setup and kit.
    If the setup is not defined, volume will be set in any matching kit(s) in every setup(s).

{"/kit/get/volume",        KIT_GET_VOLUME},
{"/kit/get/volume/by_name",        KIT_GET_VOLUME_BYNAME},
{"/kit/get/instruments_list", KIT_GET_INSTRUMENTS_LIST},

{"/instrument/set/volume",            INSTRUMENT_SET_VOLUME},
{"/instrument/get/volume",            INSTRUMENT_GET_VOLUME},
{"/instrument/get/volume/by_name",            INSTRUMENT_GET_VOLUME_BYNAME},
{"/instrument/set/pan",                INSTRUMENT_SET_PAN},
{"/instrument/get/pan",                INSTRUMENT_GET_PAN},
{"/instrument/get/pan/by_name",                INSTRUMENT_GET_PAN_BYNAME},
{"/instrument/set/miditune",        INSTRUMENT_SET_MIDITUNE},
{"/instrument/get/miditune",        INSTRUMENT_GET_MIDITUNE},
{"/instrument/get/miditune/by_name",        INSTRUMENT_GET_MIDITUNE_BYNAME},
/*            {"/instrument/set/output",            INSTRUMENT_SET_OUTPUT},
            {"/instrument/get/output",            INSTRUMENT_GET_OUTPUT},
            {"/instrument/get/output/by_name",            INSTRUMENT_GET_OUTPUT_BYNAME},*/
{"/instrument/set/playmode",        INSTRUMENT_SET_PLAYMODE},
{"/instrument/get/playmode",        INSTRUMENT_GET_PLAYMODE},
{"/instrument/get/playmode/by_name",        INSTRUMENT_GET_PLAYMODE_BYNAME},
{"/instrument/set/playloop",        INSTRUMENT_SET_PLAYLOOP},
{"/instrument/get/playloop",        INSTRUMENT_GET_PLAYLOOP},
{"/instrument/get/playloop/by_name",        INSTRUMENT_GET_PLAYLOOP_BYNAME},
{"/instrument/set/playreverse",        INSTRUMENT_SET_PLAYREVERSE},
{"/instrument/get/playreverse",        INSTRUMENT_GET_PLAYREVERSE},
{"/instrument/get/playreverse/by_name",        INSTRUMENT_GET_PLAYREVERSE_BYNAME},
{"/instrument/set/pitchoverrange",    INSTRUMENT_SET_PITCHOVERRANGE},
{"/instrument/get/pitchoverrange",    INSTRUMENT_GET_PITCHOVERRANGE},
{"/instrument/get/pitchoverrange/by_name",    INSTRUMENT_GET_PITCHOVERRANGE_BYNAME},
{"/instrument/play",                INSTRUMENT_PLAY},
{"/instrument/stop",                INSTRUMENT_STOP},

* `/play`:
    Start playback or restart if already playing

* `/stop`:
    Stop playback

* `/bpm` <float_or_int: bpm>:
    Set bpm

* `/screenset` <int: screen>:
    Change active screen set

* `/panic`:
    Disable all sequences and cancel queued sequences

* `/sequence` <string: mode> <int: column> <int: row>:
    Set sequence(s) state<br/>
    _mode_: "solo", "on", "off", "toggle", "record", "record_on", "record_off", "clear"; only one sequence can be recording at a time; "record_off" mode doesn't require any argument<br/>
    _column_: column number on screen set (zero indexed)<br/>
    _row_: row number; if omitted, all rows are affected; multiple rows can be specified


* `/sequence` <string: mode> <string: name>:
    Set sequence(s) state<br/>
    _name_: sequence name or osc pattern (can match multiple sequence names); multiple names can be specified

* `/sequence/queue` <string: mode> <int: column> <int: row>:
    Same as /sequence but affected sequences will change state only on next cycle

* `/sequence/trig` <string: mode> <int: column> <int: row>:
    Same as /sequence and (re)start playback


* `/status` <string: address>:
    Send sequencer's status as json, without sequences informations<br/>
    _address_: *osc.udp://ip:port* or *osc.unix:///path/to/socket* ; if omitted the response will be sent to the sender

* `/status/extended` <string: address>:
    Send sequencer's status as json, including sequences informations<br/>

## OSC STATUS

<pre>
{
    "screenset": <int>,
    "screensetName": "<string>",
    "playing": <int>,
    "bpm": <int>,
    "tick": <int>,
    "sequences": [
        {
            "col": <int>,
            "row": <int>,
            "name": "<string>",
            "time": "<string>",
            "bars": <int>,
            "ticks": <int>,
            "queued": <int>,
            "playing": <int>,
            "timesPlayed": <int>,
            "recording": <int>
        },
        ...
    ]
}
</pre>


**Sequencer status**

    screenset: current screenset
    screensetName: current screenset's name
    playing: playback state
    bpm: current bpm
    tick: playback tick (192 ticks = 1 quarter note)

**Sequences statuses** (1 per active sequence in current screenset)

    col: column position
    row: row position
    name: sequence name
    time: sequence time signature (eg "4/4")
    bars: number of bars in sequence
    ticks: sequence length
    queued: sequence's queued state
    playing: sequence's playing state
    timesPlayed: number of times the sequence played since last enabled
    recording: sequence's recording state


## AUTHORS

seq192 is written by Jean-Emmanuel Doucet and based on

* seq24, written by:
    Rob C. Buse, Ivan Hernandez, Guido Scholz, Dana Olson, Jaakko Sipari,
    Peter Leigh, Anthony Green, Daniel Ellis, Sebastien Alaiwan, Kevin Meinert,
    Andrea delle Canne
* seq32, written by:
    Stazed



## COPYRIGHT

Copyright © 2021 Jean-Emmanuel Doucet <jean-emmanuel@ammd.net>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

## LINKS

Sources: <a href="https://github.com/jean-emmanuel/seq192">https://github.com/jean-emmanuel/seq192</a>

<style type='text/css' media='all'>
/* style: toc */
.man-navigation {display:block !important;position:fixed;top:0;left:113ex;height:100%;width:100%;padding:48px 0 0 0;border-left:1px solid #dbdbdb;background:#eee}
.man-navigation a,.man-navigation a:hover,.man-navigation a:link,.man-navigation a:visited {display:block;margin:0;padding:5px 2px 5px 30px;color:#999;text-decoration:none}
.man-navigation a:hover {color:#111;text-decoration:underline}
</style>
