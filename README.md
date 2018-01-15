# RFBridge
This Project uses an esp8266 board as a hub to control RF-Switches via Amazon Echo (Alexa) by emulating Wemos Network Devices 

Its basically a more stable reimplementation, enhanced version of fauxmoESP by https://bitbucket.org/xoseperez/

## This Project is not activly maintained. Please head over to the newer RFBridge2 Projekt.
https://github.com/Monarch73/RFBridge2

# Known Problems

 - Currently (as of 10/22/2017) HEAD Builds are not working. To successfully compile the Project esp8266/arduino needs to check out commit 2c2d6a3bad29249c6954c2c61a83a3e4ff847da6
and ESPAsyncTCP to 9b0cc37cd7bdf4b7d17a363141d2e988aa46652c

 - If the ESP8266 keeps loosing configuration data and constantly booting into "EasyAlexa"-Mode, you are probably using the wrong flashing tool. For windows users I recommend using the github flasher seen below..
 
 - Apparantly, the builtin WeMo device control is no longer available in the new **Amazon Echo Plus** devices. 

# Flashing the release.

The release file is designed to run on a D1-Mini-R2 compatible esp8266 board. 
Just download the esp8266 flash tool from [github](https://github.com/nodemcu/nodemcu-flasher)
and flash the latest released rfbridge.bin. 

# Wiring

Connect a [RF-Transmitter Module](https://www.amazon.com/gp/product/B017AYH5G0) to your esp8266 device.
The data-connector should be connected to D4 (GPIO2). If you are using a different board, you need to look up GPIO2 in the pinout chart of your board and recompile the project for that board.

# Usage

The first time rfbridge runs on your esp8266, it starts off acting as an AP named "EasyAlexa". Connect to it and open a browser to open http://192.168.4.1
You will be asked to enter a ESSID and Password. Click Save and reset the device.

The esp8266 will now connect to the network. To identify the IP of your device, You can either use a network scanner (ie "Net Scan" for Android Devices) or a usp port monitor.
Browse to the ip of your device.

Add a switch
In the "Switch Configuration" section, specify a name for the switch. This name will be reconzied by Alexa later on so it's best to specify a distinct name.

RF-Switches usaly have 10 dip-switches. The first 5 switches specify a "housecode" and the last 5 specify the device Code. The position of the switches need to be translated to 0 (=off position) and 1 (=on postion) IE.

Housecode    00100
Code         10011

Opionaly, if your RF Switches don't have dip-switches, you can specify so called "TriState Codes". These TriState Codes can be taken from the output of rc-switch/AdvancedReceiveDemo-example. You need a receiver-module for this procedure.

Another option, if you wan't to trigger other devices on your smarthome network, your can specify URLs that will be fired when alexa triggers a switch.
 
Hit Save and start the device recover function in the alexa app. The above configured device should be discovered.


# building sources in docker.

Head builds are currently broken! (see Known Problems)

Requirements for Windows users.

	- [Docker Toolbox](https://download.docker.com/win/stable/DockerToolbox.exe)
	- [git for windows](https://git-scm.com/download/win)
	
Launch the "Docker Quick Start Terminal".
```sh
Starting "default"...
(default) Check network to re-create if needed...
(default) Waiting for an IP...
Machine "default" was started.
Waiting for SSH to be available...
Detecting the provisioner...
Started machines may have new IP addresses. You may need to re-run the `docker-machine env` command.
Regenerate TLS machine certs?  Warning: this is irreversible. (y/n): Regenerating TLS certificates
Waiting for SSH to be available...
Detecting the provisioner...
Copying certs to the local machine directory...
Copying certs to the remote machine...
Setting Docker configuration on the remote daemon...



                        ##         .
                  ## ## ##        ==
               ## ## ## ## ##    ===
           /"""""""""""""""""\___/ ===
      ~~~ {~~ ~~~~ ~~~ ~~~~ ~~~ ~ /  ===- ~~~
           \______ o           __/
             \    \         __/
              \____\_______/

docker is configured to use the default machine with IP 192.168.99.100
For help getting started, check out the docs at https://docs.docker.com

Start interactive shell

niels@thinkpad MINGW64 /h
$ 
```

Create a work directory and clone the repository using git. 

```sh
$ cd /c/
$ mkdir work
$ cd work
$ git clone https://github.com/Monarch73/RFBridge.git
$ cd RFBridge
$ cd Docker
```

Inside the Docker-subdirectory launch the dockerbuild.sh-script

```sh
./dockerbuild.sh
```

Docker builds a minimal container that compiles the project and serves the flashable binary.

# Rebuild project for a different board

Inside the docker quickstart terminal
```sh
$ docker run -d -t --name rfbridge rfbridge
$ docker exec -i -t rfbridge /bin/bash
root@6be4938dda1c:/# apt-get install nano
nano /rebuild.sh
```

In the make-line replace the d1_mini-board with the one you own.

Available Boards are:
```
generic              Generic ESP8266 Module
esp8285              Generic ESP8285 Module
espduino             ESPDuino (ESP-13 Module)
huzzah               Adafruit HUZZAH ESP8266
espresso_lite_v1     ESPresso Lite 1.0
espresso_lite_v2     ESPresso Lite 2.0
phoenix_v1           Phoenix 1.0
phoenix_v2           Phoenix 2.0
nodemcu              NodeMCU 0.9 (ESP-12 Module)
nodemcuv2            NodeMCU 1.0 (ESP-12E Module)
modwifi              Olimex MOD-WIFI-ESP8266(-DEV)
thing                SparkFun ESP8266 Thing
thingdev             SparkFun ESP8266 Thing Dev
esp210               SweetPea ESP-210
d1_mini              WeMos D1 R2 & mini
d1                   WeMos D1(Retired)
espino               ESPino (ESP-12 Module)
espinotee            ThaiEasyElec's ESPino
wifinfo              WifInfo
coredev              Core Development Module
gen4iod              4D Systems gen4 IoD Range
oak                  DigiStump Oak
```

Save the file and execute it. After compilation is complete, exit the container.
```sh
./rebuild.sh
exit
```

Copy the file from the container to your local disk:

```sh
docker cp rfbridge:/tmp/mkESP/RFBridge_<your board here>/RFBridge.bin ./rfbridge.bin
```

Done

# Setting up development environment.

Requirements for Windows users

	- [Visual Studio 2017](https://www.visualstudio.com/vs/whatsnew/) Any Edtition will do. Visual Studio 2017 Community is for free for individuals.
	- Make sure you have the c++ and git extension enabled in the Visual Studio Installer (can be launched any time from the start menu).
	- [Visual Micro extension](http://www.visualmicro.com/page/Arduino-Visual-Studio-Downloads.aspx)
	- [git for windows](https://git-scm.com/download/win)
	- python2.7 for windows. I recommend using [chocolaty](https://chocolatey.org/install) to install the python2-package for windows.

Setup the project

	- recursivly clone the project and all submodules to an empty directory
	- change the directory to RFBridge\hardware\esp8266\esp8266\tools directory and run the get.py-file

```sh
c:
cd \
mkdir work
cd work
git clone --recursive https://github.com/Monarch73/RFBridge.git
cd RFBridge\hardware\esp8266\esp8266\tools
python get.py
```
	- launch Visual Studio 2017
	- goto "tools->visualmicro->Configure arduiono ide locations"
	- select "Visual Micro(noide)" from drop down
	- in "sketchbook-locations" enter the directory where you cloned the repository (c:\work\RFBridge);
	- click "OK"
	- select "tools->visualstudio->rescan toolchains and libraries".
	
Visual Studio should now be prepared to open the solution file (.sln) and compile the project.
	
