Thesis project
----------------------------------------------------------------------------
It consists of reading a <a href="https://irrometer.com/sensors.html#wm">Watermark sensor</a> with a pic16, then use an embedded system <a href="https://www.st.com/en/microcontrollers-microprocessors/stm32f103c8.html">STM32f</a> with <a href="http://www.nuttx.org/">NuttxOS</a> installed to send the data obtained from the sensor through an <a href="https://www.digi.com/products/tag/zigbee">xbee device</a> based on Zigbee to a server, then the server sends the data to the <a href="https://www.altairsmartcore.com/">Altair Smartcore</a> platform through <a href="https://www.eclipse.org/paho/">Paho MQTT</a>.

gcc -o server server.c -lxbee -ljson-c -lpaho-mqtt3c

gcc -o serialport serialport.c -lxbee -ljson-c

Requirements
----------------------------------------------------------------------------
<a href="https://github.com/json-c/json-c">json-c</a><br>
sudo apt-get install libjson-c-dev

<a href="https://github.com/attie/libxbee3">libxbee</a><br>
git clone https://github.com/attie/libxbee3<br>
cd libxbee3<br>
make configure<br>
make all<br>
sudo make install<br>

<a href="https://github.com/eclipse/paho.mqtt.c">paho mqtt</a><br>
sudo apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui<br>
sudo apt-get install fakeroot fakeroot devscripts dh-make lsb-release<br>
sudo apt-get install libssl-dev<br>
sudo apt-get install doxygen graphviz<br>
git clone https://github.com/eclipse/paho.mqtt.c<br>
cd paho.mqtt.c<br>
make<br>
sudo make install<br>
*******************************************************
Xbee modules configuration using <a href="https://www.digi.com/products/embedded-systems/digi-xbee-tools/xctu">XCTU</a><br><br>
Firmware Zibgee th v4060<br><br>
Coordinator (server side) <br>
PANid 1234<br>
CE 1<br>
Api mode 1<br><br>
Router(serialport side)<br>
PANid 1234<br>
JV 1<br>
Transparent mode 0<br>
*******************************************************
Nuttx
-------------------------------------------------------
Packages needed<br><br>
sudo apt-get install automake bison build-essential flex gcc-arm-none-eabi gperf git libncurses5-dev libtool libusb-dev libusb-1.0.0-dev pkg-config minicom<br><br>
mkdir nuttxspace/<br>
cd nuttxspace/<br><br>
OpenOCD needed<br><br>
git clone http://repo.or.cz/r/openocd.git<br>
cd nuttxspace/openocd/<br>
./bootstrap<br>
./configure --enable-internal-jimtcl --enable-maintainer-mode --disable-werror --disable-shared --enable-stlink --enable-jlink --enable-rlink --enable-vslink --enable-ti-icdi --enable-remote-bitbang<br>
make<br>
sudo make install<br>
*******************************************************
cd nuttxspace/<br>
git clone https://bitbucket.org/nuttx/nuttx<br>
git clone https://bitbucket.org/nuttx/apps<br>
git clone https://bitbucket.org/nuttx/tools<br>
*******************************************************
cd nuttxspace/tools/kconfig-frontends/<br>
./configure<br>
make<br>
sudo make install<br>
sudo ldconfig<br>
*******************************************************
cd nuttxspace/nuttx/tools/<br>
./configure.sh stm32f103-minimum/nsh<br>
*******************************************************
cd nuttxspace/nuttx/<br>
make menuconfig<br><br>
system type >>smt32 peripheral support >>usb device activado<br>
board selection >>enable boardctl() interface activado, enable usb device controls activado<br>
device drivers >>usb device driver support >>usb modem cdc/acd >>cdc/acd console device activado<br>
device driver >>serial driver support >>no serial console<br>
system type>>smt32 peripheral support >>usart1 activado<br>
device drivers>>serial driver support>>usart1 configuration>>9600 baud rate<br>
application support >>examples >>serial rx example activado, moisture app example activado<br><br>
(moistureapp_main.c, Makefile, Kconfig and Make.defs need to be in a folder inside Nuttx examples)
*******************************************************
cd nuttxspace/nuttx/<br>
make<br>
*******************************************************
cd nuttxspace/openocd/contrib/<br>
sudo cp 60-openocd.rules /etc/udev/rules.d/<br>
sudo udevadm trigger<br>
*******************************************************
cd nuttxspace/nuttx/<br>
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg<br>
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c init -c "reset halt" -c "flash write_image erase nuttx.bin 0x08000000"<br>
*******************************************************
sudo minicom -s<br><br>
serial port setup>><br>
A-serial device /dev/ttyACM0<br>
E-Bps/Par/Bits: 9600 8N1<br>
F-hardware flow control no<br><br>
save setup as dfl<br>
<br>
nsh> ?<br>
nsh> ls dev/<br>
nsh> cat dev/ttyS0<br>
