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
* `/set/global_volume` <double_or_float_or_int: volume>:<br />
    Set tapeutape global volume.

* `/get/global_volume` (optional: <string: osc_address>):<br />
    Send tapeutape global volume on the specified osc_address. If osc_address is not defined, tapeutape will send the global volume back to the address from which it received the osc message.

* `/get/setups_list` (optional: <string: osc_address>):<br />
    Send current setups list on the specified osc_address. If osc_address is not defined, tapeutape will send the setups list back to the address from which it received the osc message.

* `/setup/get/kits_list` <string: setup> (optional: <string: osc_address>):<br />
    Send setup's kits list on the specified osc_address. If osc_address is not defined, tapeutape will send the kits list back to the address from which it received the osc message.

**Kit**
* `/kit/select` (optional <int_or_string: setup>) <int_or_string: kit>:<br />
    Select specified kit as current kit (Execution Window).<br />
    ID or name can be used to defined setup and kit.<br />
    If the setup is not defined, matching kit(s) will be selected in every setup(s) in which there is a matching kit.

* `/kit/set/selected` (optional <int_or_string: setup>) <int_or_string: kit>:<br />
    Same as `/kit/select`.

* `/kit/get/selected` (optional: <string: osc_address>):
    Send selected kit ID for each setup on the specified osc_address. If osc_address is not defined, tapeutape will send the kits list back to the address from which it received the osc message.<br />
    Received message should look like this:<br />
    `/tapeutape/get/selected` <int: SETUP_ID_1> <int: SELECTED_KIT_ID> <int: SETUP_ID_2> <int: SELECTED_KIT_ID> ...

* `/kit/get/selected/by_name` (optional: <string: osc_address>):<br />
    Send selected kit name for each setup on the specified osc_address. If osc_address is not defined, tapeutape will send the kits list back to the address from which it received the osc message.<br />
    Received message should look like this:<br />
    `/tapeutape/get/selected` <string: SETUP_NAME_1> <string: SELECTED_KIT_NAME> <string: SETUP_NAME_2> <string: SELECTED_KIT_NAME> ...

* `/kit/set/volume` (optional <int_or_string: setup>) <int_or_string: kit> <double_or_float_or_int: volume>:<br />
    Set kit volume.<br />
    ID or name can be used to defined setup and kit.<br />
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

## AUTHORS

see README.md

## COPYRIGHT

see README.md

## LINKS

Sources: <a href="https://github.com/PlagiatBros/tapeutape">https://github.com/PlagiatBros/tapeutape</a>

<style type='text/css' media='all'>
/* style: toc */
.man-navigation {display:block !important;position:fixed;top:0;left:113ex;height:100%;width:100%;padding:48px 0 0 0;border-left:1px solid #dbdbdb;background:#eee}
.man-navigation a,.man-navigation a:hover,.man-navigation a:link,.man-navigation a:visited {display:block;margin:0;padding:5px 2px 5px 30px;color:#999;text-decoration:none}
.man-navigation a:hover {color:#111;text-decoration:underline}
</style>
