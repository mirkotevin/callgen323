H.323 Call Generator

v 2.1.0, Nov 2017
=================

What you can do with this call generator: 
 - spawning an exact number of calls. 
 - receiving an exact number of calls. 
 - adjust the delay between each batch of calls. 
 - set the number of batches to repeat. 
 - The capability supported are G.711 ULaw 64k, user
   indication and plugin video codecs.

HOW TO GET
==========
https://github.com/willamowius/callgen323


HOW TO COMPILE
==============
  In Win32 platform
  - Follow the instructions on how to build H323Plus.
  - When changing active build configuration, make sure you specify
    the correct H323Plus library directory in Tools/Options.
  - You are on your own now.

  In Linux platform:
  - Follow the instructions on how to build openh323/voxilla.
  - Unpack the callgen distribution under the openh323 directory. 
    Example: 
      $ cd ~/h323plus
      $ tar -xzf callgen323.tar.gz
  - Run make in the callgen323 directory:
     $ cd callgen323
     $ make optnoshared
  - Run the executable to get the runtime options.
  - You're on your own now.


HOW TO RUN
==========
In normal circumstances, you need to run two instances of callgen,
one to receive the calls (passive), and another to make the
calls (active). For both instances, usually you need to specify
the same number of calls that you want to receive/make with -n option.

Example:
in host 1, run:
  callgen -m 5 -n -l
meaning: start in passive mode with 5 number of lines/calls to receive.

in host 2, run:
  callgen -m 5 host1
meaning: make 5 calls to host1

You can run both instances in a single host if you want, as long as
you have two IP interfaces on your host. All you need to do is to
specify different IP interface to listen for each callgen (with
the -i option).

Audio files for OGM messages must be 16bit Microsoft PCM files
in WAV format at 8000 Hz.


COMMAND LINE OPTIONS (SELECTED)
===============================
-h
   Show usage with all command line options
-g <host>
   Manually specifies the host/address of the gatekeeper.
-i <IP addr>
   Specifies the IP interface to which the callgen will bind its
   listener.
-l
   Start in passive mode. Set this option for the receiving/callee
   callgen.
-m <number>
   Number of calls/lines. For receiving callgen, this specifies the
   maximum number of calls it can receive simultaneously. For the
   calling callgen, this specifies the number of calls that the
   callgen must make simultaneously.
-n
   Doesn't require to register with gatekeeper. Means that if the
   gatekeeper registration fails, the callgen will still continue
   its execution.
-r <number>
   Number of batches to repeat. A batch is one sequence of making
   calls and hanging up calls.
-u <username>
   Specifies local username to assign to the callgen.


USING CALLGEN WITH GATEKEEPER IN A SINGLE HOST
==============================================
Normally, I use the callgen on a single host on my Linux machine,
using the IP aliasing. Apart from loopback interface, I'll set up
two other IP alias:

# ifconfig lo:0 192.1.4.1
# ifconfig lo:1 192.1.4.2

Then I run the GK on IP address 127.0.0.1:

# ./gk -h 127.0.0.1

Then I run the callee callgen on one of the IP alias, and assign it
with a username:

# ./callgen -i 192.1.4.1 -u foo -g 127.0.0.1 -l

Then I run the caller callgen on the other IP alias, assign it with
other username, and specify username and host of the callee callgen:

# ./callgen -i 192.1.4.2 -u bar -g 127.0.0.1 foo@192.1.4.1



CHANGELOG
=========
[Nov 2017] Version 2.1.0
- support H.460.x
- bug fixes

[26 Jan 2000] Version 0.1.3
- Compatibility with openh323 v1 alpha2
- Fix gatekeeper functionality.
- Added options to set username, interface, and various delay.

[31 Oct 99] Version 0.1.2
- It will now register to gatekeeper (hopefully)
- Drop the STL thingy (list, string) and assert(). It should be more
  compatible to openh323. Also cleanup some warnings with Level 4
  Warning in MSVC, it should compile cleaner now.

[24 Oct 99] Version 0.1.1
- Linux compatibility. Thanks to Jan Willamowius.

[22 Oct 99]
- With openh323-0.9alpha2, expect it to have crashes and memory 
  leaks between call batches. 


Benny LP
seventhson@theseventhson.freeserve.co.uk

