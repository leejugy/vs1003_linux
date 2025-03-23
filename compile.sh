source /opt/gcc-beaglebone/environment-setup-armv7at2hf-neon-poky-linux-gnueabi
export CFLAGS="-Wunused"
export LDFLAGS="-lrt -lpthread"
export CPPFLAGS=""
export SRC="main.c gpio.c spi_ctl.c vs1003.c audio.c" 
export OBJECT="vs1003"

make -j
cp vs1003 ~/share
sync