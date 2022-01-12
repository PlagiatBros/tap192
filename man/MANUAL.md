tap192(1) -- MIDI- & OSC-controlable audiosampler
=============================

## SYNOPSIS

`tap192` [OPTION...] filename.tap

## DESCRIPTION

tap192 is a lightweight, MIDI- and OSC-controlable, NSM-compatible audiosampler.

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
    Set tap192 global volume.

* `/get/global_volume` (optional: <string: osc_address>):<br />
    Send tap192 global volume to the specified osc_address. If osc_address is not defined, tap192 will send the global volume back to the address from which it received the osc message.

* `/get/setups_list` (optional: <string: osc_address>):<br />
    Send current setups list to the specified osc_address. If osc_address is not defined, tap192 will send the setups list back to the address from which it received the osc message.

* `/setup/get/kits_list` <string: setup> (optional: <string: osc_address>):<br />
    Send setup's kits list to the specified osc_address. If osc_address is not defined, tap192 will send the kits list back to the address from which it received the osc message.

**Kit**
* `/kit/select` (optional <int_or_string: setup>) <int_or_string: kit>:<br />
    Select specified kit as current kit (Execution Window).<br />
    ID or name can be used to defined setup and kit.<br />
    If the setup is not defined, matching kit(s) will be selected in every setup(s) in which there is a matching kit.

* `/kit/set/selected` (optional <int_or_string: setup>) <int_or_string: kit>:<br />
    Same as `/kit/select`.

* `/kit/get/selected` (optional: <string: osc_address>):<br />
    Send selected kit ID for each setup to the specified osc_address. If osc_address is not defined, tap192 will send the kits list back to the address from which it received the osc message.<br />
    Received message should look like this:<br />
    `/tap192/get/selected` <int: SETUP_ID_1> <int: SELECTED_KIT_ID> <int: SETUP_ID_2> <int: SELECTED_KIT_ID> ...

* `/kit/get/selected/by_name` (optional: <string: osc_address>):<br />
    Send selected kit name for each setup to the specified osc_address. If osc_address is not defined, tap192 will send the kits list back to the address from which it received the osc message.<br />
    Received message should look like this:<br />
    `/tap192/get/selected` <string: SETUP_NAME_1> <string: SELECTED_KIT_NAME> <string: SETUP_NAME_2> <string: SELECTED_KIT_NAME> ...

* `/kit/set/volume` (optional <int_or_string: setup>) <int_or_string: kit> <double_or_float_or_int: volume>:<br />
    Set kit volume.<br />
    ID or name can be used to defined setup and kit.<br />
    If the setup is not defined, volume will be set in any matching kit(s) in every setup(s).

* `/kit/get/volume` <int_or_string: setup> <int_or_string kit> (optional <string: address>):<br />
    Send kit volume to the specified osc_address. If osc_address is not defined, tap192 will send the kit volume back to the address from which it received the osc message.

* `/kit/get/volume/by_name` <int_or_string: setup> <int_or_string kit> (optional <string: address>):<br />
    Same as `/kit/get/volume`, except setup and kit names are used instead of IDs in the feedback message.

* `/kit/get/instruments_list` <int_or_string: setup> <int_or_string kit> (optional <string: address>):<br />
    Send kit instruments list to the specified osc_address. If osc_address is not defined, tap192 will send the instruments list back to the address from which it received the osc message.
    Not implemented yet.

**Instrument**

* `/instrument/set/volume` (optional <int_or_string: setup>) (<int_or_string: kit>) <int_or_string: instrument> <double_or_float_or_int: volume>:<br />
    Set instrument volume.<br />
    ID or name can be used to defined setup, kit and instrument.<br />
    Setup and kit are optional. If setup and/or kit is not defined, volume will be set for any matching instrument in every kit(s) and/or in every setup(s).

* `/instrument/get/volume` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Send instrument volume to the specified osc_address. If osc_address is not defined, tap192 will send the instrument volume back to the address from which it received the osc message.

* `/instrument/get/volume/by_name` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Same as `/instrument/get/volume`, except setup, kit and instrument names are used instead of IDs in the feedback message.

* `/instrument/set/pan`  (optional <int_or_string: setup>) (<int_or_string: kit>) <int_or_string: instrument> <double_or_float_or_int: pan>:<br />
    Set instrument pan.<br />
    ID or name can be used to defined setup, kit and instrument.<br />
    Setup and kit are optional. If setup and/or kit is not defined, pan will be set for any matching instrument in every kit(s) and/or in every setup(s).

* `/instrument/get/pan` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Send instrument pan to the specified osc_address. If osc_address is not defined, tap192 will send the instrument pan back to the address from which it received the osc message.

* `/instrument/get/miditune/by_name` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Same as `/instrument/get/miditune`, except setup, kit and instrument names are used instead of IDs in the feedback message.

* `/instrument/set/miditune`  (optional <int_or_string: setup>) (<int_or_string: kit>) <int_or_string: instrument> <double_or_float_or_int: miditune>:<br />
    Set instrument miditune.<br />
    ID or name can be used to defined setup, kit and instrument.<br />
    Setup and kit are optional. If setup and/or kit is not defined, miditune will be set for any matching instrument in every kit(s) and/or in every setup(s).

* `/instrument/get/miditune` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Send instrument miditune to the specified osc_address. If osc_address is not defined, tap192 will send the instrument miditune back to the address from which it received the osc message.

* `/instrument/get/miditune/by_name` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Same as `/instrument/get/miditune`, except setup, kit and instrument names are used instead of IDs in the feedback message.

* `/instrument/set/playmode`  (optional <int_or_string: setup>) (<int_or_string: kit>) <int_or_string: instrument> <int: playmode>:<br />
    Set instrument playmode.<br />
    ID or name can be used to defined setup, kit and instrument.<br />
    Setup and kit are optional. If setup and/or kit is not defined, playmode will be set for any matching instrument in every kit(s) and/or in every setup(s).

* `/instrument/get/playmode` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Send instrument playmode to the specified osc_address. If osc_address is not defined, tap192 will send the instrument playmode back to the address from which it received the osc message.

* `/instrument/get/playmode/by_name` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Same as `/instrument/get/playmode`, except setup, kit and instrument names are used instead of IDs in the feedback message.

* `/instrument/set/playloop`  (optional <int_or_string: setup>) (<int_or_string: kit>) <int_or_string: instrument> <int: playloop>:<br />
    Set instrument playloop.<br />
    ID or name can be used to defined setup, kit and instrument.<br />
    Setup and kit are optional. If setup and/or kit is not defined, playloop will be set for any matching instrument in every kit(s) and/or in every setup(s).

* `/instrument/get/playloop` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Send instrument playloop to the specified osc_address. If osc_address is not defined, tap192 will send the instrument playloop back to the address from which it received the osc message.

* `/instrument/get/playloop/by_name` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Same as `/instrument/get/playloop`, except setup, kit and instrument names are used instead of IDs in the feedback message.

* `/instrument/set/playreverse`  (optional <int_or_string: setup>) (<int_or_string: kit>) <int_or_string: instrument> <int: playreverse>:<br />
    Set instrument playreverse.<br />
    ID or name can be used to defined setup, kit and instrument.<br />
    Setup and kit are optional. If setup and/or kit is not defined, playreverse will be set for any matching instrument in every kit(s) and/or in every setup(s).

* `/instrument/get/playreverse` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Send instrument playreverse to the specified osc_address. If osc_address is not defined, tap192 will send the instrument playreverse back to the address from which it received the osc message.

* `/instrument/get/playreverse/by_name` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Same as `/instrument/get/playreverse`, except setup, kit and instrument names are used instead of IDs in the feedback message.

* `/instrument/set/pitchoverrange`  (optional <int_or_string: setup>) (<int_or_string: kit>) <int_or_string: instrument> <int: pitchoverrange>:<br />
    Set instrument pitchoverrange.<br />
    ID or name can be used to defined setup, kit and instrument.<br />
    Setup and kit are optional. If setup and/or kit is not defined, pitchoverrange will be set for any matching instrument in every kit(s) and/or in every setup(s).

* `/instrument/get/pitchoverrange` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Send instrument pitchoverrange to the specified osc_address. If osc_address is not defined, tapeutape will send the instrument pitchoverrange back to the address from which it received the osc message.

* `/instrument/get/pitchoverrange/by_name` <int_or_string: setup> <int_or_string: kit> <int_or_string: instrument> (optional <string: address>):<br />
    Same as `/instrument/get/pitchoverrange`, except setup, kit and instrument names are used instead of IDs in the feedback message.

* `/instrument/play` (optional <int_or_string: setup>) (optional <int_or_string: kit>) <int_or_string: instrument> (optional <int: velocity> (0-127)) (optional <double_or_float_or_int: pitch>):<br />
    Play instrument @ velocity and pitch. If velocity is not defined, instrument will be played with velocity at 127. If pitch is not defined, instrument will be played with pitch at 1 (straight).

* `/instrument/stop` (optional <int_or_string: setup>) (optional <int_or_string: kit>) <int_or_string: instrument>: <br />
    Stop instrument (if playing).

## AUTHORS

see README.md

## COPYRIGHT

see README.md

## LINKS

Sources: <a href="https://github.com/PlagiatBros/tap192">https://github.com/PlagiatBros/tap192</a>

<style type='text/css' media='all'>
/* style: toc */
.man-navigation {display:block !important;position:fixed;top:0;left:113ex;height:100%;width:100%;padding:48px 0 0 0;border-left:1px solid #dbdbdb;background:#eee}
.man-navigation a,.man-navigation a:hover,.man-navigation a:link,.man-navigation a:visited {display:block;margin:0;padding:5px 2px 5px 30px;color:#999;text-decoration:none}
.man-navigation a:hover {color:#111;text-decoration:underline}
</style>
