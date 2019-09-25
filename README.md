版本为rk3308_linux_release_v1.3.1_20190913.xml

rk 3308 linux wifi bt 部分

和andorid不一样 linux一般就是只有一个版本 所有改动都是集中在一个分支，还有一些定制产品

改动比较频繁，更新有时不及时，版本上很快就被抛后，有很多commit也经常没来得及放到repo上，详细操作参考doc





1、有两种模组，一种是博通的，另外一种是rtk的

博通跑bsa或者bluez协议栈，rtk只跑bluez协议

broadcom_bsa + deviceio = 博通
rk_wifi_bt_linux/external/rkwifibt/brcm_tools/brcm_patchram_plus1.c + bluetoothd + bluealsa + bluealsa-aplay = 博通
rtk_hciattch + bluetoothd + bluealsa + bluealsa-aplay  =rtk


2、跑rtk的
```
这个是rtk的启动脚本，rtk_hciattach加载起来fw的后面，brcm_patchram_plus1做完前面的后面也是一样的
echo 0 > /sys/class/rfkill/rfkill0/state

sleep 2

echo 1 > /sys/class/rfkill/rfkill0/state

sleep 2

insmod /usr/lib/modules/hci_uart.ko

rtk_hciattach -n -s 115200 /dev/ttyS4 rtk_h5 &

hciconfig hci0 up

/usr/libexec/bluetooth/bluetoothd --compat -n  &
sleep 1
sdptool add A2SNK
sleep 1
hciconfig hci0 up
sleep 1
hciconfig hci0 piscan
sleep 1
hciconfig hci0 name 'realtek_bt'
sleep 1
hciconfig hci0 down
sleep 1
hciconfig hci0 up
sleep 2
bluealsa --profile=a2dp-sink & 
sleep 1
bluealsa-aplay --profile-a2dp 00:00:00:00:00:00 & 
sleep 1
hciconfig hci0 class 0x240404
```
```
这个是hfp的测试，这个是必须先跑bluetoothd，就是衔接上面
busybox killall bluealsa-aplay 

busybox killall bluealsa

bluealsa -p hfp-hf &

arecord -Dhw:1,0 -r 8000 -c 2 -f S16_LE |aplay -r 8000 -c 2 -f S16_LE &

arecord -D 2mic_loopback -r 8000 -f S16_LE -c 2 --period-size 1024 --buffer-size 65536|aplay -D hw:1,0 -r 8000 -c 2 -f S16_LE &
```


3、跑博通的

博通的bsa协议中，因为是个黑盒子，不提供代码，所以只能调用接口，另一方面如果自己需要一些特殊功能，也需要博通修改重新编译提供后台应用，博通不提供技术支持，只能让代理商去改。

是什么后台应用？ 就是broadcom_bsa/server这个目录下的bsa_server

bsa_server这个大小大概是4M，这个后台是一定要跑的，不管前端写的是应用还是脚本还是自己敲得命令，这个应用必须跑起来

看一下手动的命令

echo 0 > /sys/class/rfkill/rfkill0/state

sleep 2

echo 1 > /sys/class/rfkill/rfkill0/state

sleep 2

mkdir -p /data/bsa/config

cd /data/bsa/config

bsa_server -r 12 -p /system/etc/firmware/BCM4343B0.hcd -d /dev/ttyS4 -b /data/btsnoop.log > /data/bsa_log &

app_manager &

有跑bsa_server的后台，sniff log也可以输出到data/btsnoop.log，前端只跑了app_manager

还有哪些前端呢，在broadcom_bsa/3rdparty里面，这个目录下就是编译测试程序，

库在 ./bsa_examples/linux/libbsa/build/arm64/sharedlib/libbsa.so

./bsa_examples/linux/libbsa/build/arm/sharedlib/libbsa.so

./bsa_examples/linux/libbsa/build/x86_64/sharedlib/libbsa.so

一些测试程序

博通在linux上面，除了可以跑自己的协议栈，当然也可以跑开源的bluez，看一下手动的命令，主要就是有个hciattach加载hci0的

echo 0 > /sys/class/rfkill/rfkill0/state

sleep 2

echo 1 > /sys/class/rfkill/rfkill0/state

sleep 2

brcm_patchram_plus1 -d --enable_hci --no2bytes --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM4343B0.hcd /dev/ttyS4 & //初始化

/usr/libexec/bluetooth/bluetoothd --compat -n &

hciconfig hci0 up

hciconfig hci0 piscan

这个就是跑BLUEZ的，IPC机制是D-BUS，一样是后台前端的模式，跑起bluetoothd可以运行一些工具或者bluez-alsa

然后再看一下，运行博通协议栈的时候，一些配网的例子，这个脚本是我们sdk自带的，我们自己写的









