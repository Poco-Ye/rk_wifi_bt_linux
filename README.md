BT FTP

ftp.realsil.com.cn

账号btlinux 密码 ujv376xc

版本为rk3308_linux_release_v1.3.1_20190913.xml

rk 3308 linux wifi bt 部分

和andorid不一样 linux一般就是只有一个版本 所有改动都是集中在一个分支，还有一些定制产品

改动比较频繁，更新有时不及时，版本上很快就被抛后，有很多commit也经常没来得及放到repo上，详细操作参考doc

0、broadcom_bsa是bsa_server的前端,deviceio是一个封装其它库的软件
```
deviceio支持正基海华瑞昱的芯片

正基选择配置(直接在buildroot make menuconfig 可参考device doc目录指导)buildroot/package/rockchip/broadcom_bsa/Config.in
海华选择配置(直接在buildroot make menuconfig 可参考device doc目录指导)buildroot/package/rockchip/cypress_bsa/Config.in
瑞昱选择配置(直接在buildroot make menuconfig 可参考device doc目录指导)buildroot/package/rockchip/bluez-alsa/Config.in  buildroot/package/rockchip/rkwifibt/Config.in

deviceio软件的编译主要根据以上文件的配置，根据不同设置来设置不同的宏使用到软件函数选择不同路径的库或者功能，编译在buildroot/package/rockchip/deviceio/
deviceio.mk
```

```
broadcom_bsa：正基更新的bsa相关代码，直接替换external/broadcom_bsa，替换后需make broadcom_bsa-dirclean && make broadcom_bsa-rebuild
改为可以直接push libbsa.so，删掉这个deviceio是起不来的

首先这个是前端的库
./broadcom_bsa/3rdparty/embedded/bsa_examples/linux/libbsa/build/arm/sharedlib/libbsa.so
./broadcom_bsa/3rdparty/embedded/bsa_examples/linux/libbsa/build/arm64/sharedlib/libbsa.so
./broadcom_bsa/3rdparty/embedded/bsa_examples/linux/libbsa/build/x86_64/sharedlib/libbsa.so
./broadcom_bsa/3rdparty/embedded/bsa_examples/linux/libbsa/build/arm/libbsa.a
./broadcom_bsa/3rdparty/embedded/bsa_examples/linux/libbsa/build/arm64/libbsa.a
./broadcom_bsa/3rdparty/embedded/bsa_examples/linux/libbsa/build/x86_64/libbsa.a

bluez的库
./usr/lib/pulse-11.1/modules/libbluez5-util.so
./usr/lib/pulse-11.1/modules/module-bluetooth-policy.so
./usr/lib/pulse-11.1/modules/module-bluetooth-discover.so
./usr/lib/pulse-11.1/modules/module-bluez5-discover.so
./usr/lib/pulse-11.1/modules/module-bluez5-device.so



然后是deviceio的库，调用bsa的库 bluez的库
./DeviceIO/lib64/libDeviceIo_broadcom.so
./DeviceIO/lib64/libDeviceIo_bluez.so
./DeviceIO/lib64/libDeviceIo_cypress.so
./DeviceIO/lib32/libDeviceIo_broadcom.so
./DeviceIO/lib32/libDeviceIo_bluez.so
./DeviceIO/lib32/libDeviceIo_cypress.so

deviceio是一个简单的封装软件，没有bsa的源码，调用bsa的库 bluez应该也是使用加载库的方式


```


1、有两种模组，一种是博通的，另外一种是rtk的

博通跑bsa或者bluez协议栈，rtk只跑bluez协议

broadcom_bsa + deviceio(examples) = 博通
rk_wifi_bt_linux/external/rkwifibt/brcm_tools/brcm_patchram_plus1.c + bluetoothd + bluealsa + bluealsa-aplay = 博通
rtk_hciattch + bluetoothd + bluealsa + bluealsa-aplay  =rtk


2、跑rtk的
```
这个是rtk的启动脚本，rtk_hciattach加载起来fw的后面，brcm_patchram_plus1做完前面的后面也是一样的

busybox killall bluetoothd

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
```
echo 0 > /sys/class/rfkill/rfkill0/state

sleep 2

echo 1 > /sys/class/rfkill/rfkill0/state

sleep 2

mkdir -p /data/bsa/config

cd /data/bsa/config

bsa_server -r 12 -p /system/etc/firmware/BCM4343B0.hcd -d /dev/ttyS4 -b /data/btsnoop.log > /data/bsa_log &

app_manager &（运行到此步骤，才能在手机蓝牙列表发现并连接设备）
```
有跑bsa_server的后台，sniff log也可以输出到data/btsnoop.log，前端只跑了app_manager

还有哪些前端呢，在broadcom_bsa/3rdparty里面，这个目录下就是编译测试程序，

库在 ./bsa_examples/linux/libbsa/build/arm64/sharedlib/libbsa.so

./bsa_examples/linux/libbsa/build/arm/sharedlib/libbsa.so

./bsa_examples/linux/libbsa/build/x86_64/sharedlib/libbsa.so

一些测试程序

博通在linux上面，除了可以跑自己的协议栈，当然也可以跑开源的bluez，看一下手动的命令，主要就是有个hciattach加载hci0的
```
echo 0 > /sys/class/rfkill/rfkill0/state

sleep 2

echo 1 > /sys/class/rfkill/rfkill0/state

sleep 2

brcm_patchram_plus1 -d --enable_hci --no2bytes --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM4343B0.hcd /dev/ttyS4 & //初始化

/usr/libexec/bluetooth/bluetoothd --compat -n &

hciconfig hci0 up

hciconfig hci0 piscan
```
这个就是跑BLUEZ的，IPC机制是D-BUS，一样是后台前端的模式，跑起bluetoothd可以运行一些工具或者bluez-alsa

然后再看一下，运行博通协议栈的时候，一些配网的例子，这个脚本是我们sdk自带的，我们自己写的


4、跑deviceio

deviceio是被封装了的，提供的只是接口，什么都看不了
```
deviceio_test
version:V1.2.4
ERROR:invalid argument.
deviceio_test [Usage]:
        "deviceio_test bluetooth":show bluetooth test cmd menu.
        "deviceio_test wificonfig":show wifi config test cmd menu.
```
这个软件给客户用的是个图表脚本
```
# deviceio_test bluetooth
version:V1.2.4
#### Please Input Your Test Command Index ####
01.  bt_server_open
02.  bt_test_set_class
03.  bt_test_enable_reconnect
04.  bt_test_disable_reconnect
05.  bt_test_source_auto_start
06.  bt_test_source_connect_status
....
```
然后选择需要运行的选项
```
出问题时

修改/usr/bin/bsa_server.sh（deviceio软件内部是调用这个脚本启动bsa_server）

chmod 777 /usr/bin/bsa_server.sh

--- a/bsa_server.sh
+++ b/bsa_server.sh
@@ -15,8 +15,8 @@ case "$1" in
     cp /etc/bsa_file/* /data/bsa/config/test_files/av/
     cd /data/bsa/config
     echo "start broadcom bluetooth server bsa_sever" 
-    bsa_server -r 12 -p $hcd_file -d /dev/ttyS4 -all=0 &
-    #bsa_server -r 12 -b /data/bsa/btsnoop.log -p $hcd_file -d /dev/ttyS4 > /data/bsa/bsa_log &
+    #bsa_server -r 12 -p $hcd_file -d /dev/ttyS4 -all=0 &
+    bsa_server -r 12 -b /data/bsa/btsnoop.log -p $hcd_file -d /dev/ttyS4 > /data/bsa/bsa_log &

     echo "|----- bluetooth bsa server is open ------|" 

复现问题提供：
/data/bsa/btsnoop.log
/data/bsa/bsa_log
```


5、ble配网
新的版本配网已经改了，文档有说
```
博通的跑的软件
bsa_ble_wifi_introducer.sh start
deviceio_test

rtk的跑的软件
gatt_server
```
6、github linux位置
```
如：
https://github.com/rockchip-linux/rkwifibt/blob/master/firmware/broadcom/AP6212A1/bt/bcm43438a1.hcd
https://github.com/rockchip-linux/kernel/commit/c201a3aeb65b97b3df906da519d529232e9322c8
```
7、主控端uart电压域是3.0V，模块端的电压是1.8V，电压不匹配导致uart通信异常
```
ts_uart s /data/testfile 115200 1 0 0 /dev/ttyS1
请手动拉高拉低CTS测试下，不用CTS和RTS短接。 这个命令如果很快执行完，说明能正常发送，如果卡住，说明流控起作用了，不能发送。
如果怀疑接收时，RTS不起作用，可以量下RTS的信号是否拉高过。
```
8、linux音频不从蓝牙耳机出来
```
音频输出设备切换是上层处理。
debian SDK使用的是pulseaudio音频服务框架，需要对pulseaudio进行相关配置，具体请自行google相关资料，如：
https://wiki.debian.org/BluetoothUser#Can_connect.2C_but_not_to_audio
https://askubuntu.com/questions/366032/pulseaudio-not-detecting-bluetooth-headset-automatically
```
9、关于linux蓝牙乱七八糟的问题
```
linux蓝牙更新快又多，放的乱
rockchip_rk3308_release/target$ grep brcm_patchram_plus1 -nr

usr/bin/bt_load_broadcom_firmware:8:brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download  --tosleep  200000 --baudrate 1500000 --patchram  /system/etc/firmware/BCM4345C0.hcd /dev/ttyS4 &
usr/bin/bt_init.sh:8:brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download  --tosleep  200000 --baudrate 1500000 --patchram  /system/etc/firmware/BCM4345C0.hcd /dev/ttyS4 &
usr/bin/bt_pcba_test:8:brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download  --tosleep  200000 --baudrate 1500000 --patchram  /system/etc/firmware/BCM4345C0.hcd /dev/ttyS4 &

一个启动的脚本就有好几个，搞那么多，首先判断是跑bluez还是bsa
先找两个软件
一个是brcm_patchram_plus1 
还一个是rtk_hciattach

再找一个软件bsa_server
```
10、wifi sta和ap模式，或者参考wpaserviceutil.h，AP通道切换channel=6为153，不能同时
```
wpa_supplicant.conf

cat data/misc/wifi/wpa_supplicant.conf
ctrl_interface=/data/misc/wifi/sockets
disable_scan_offload=1
driver_param=use_p2p_group_interface=1p2p_device=1
update_config=1
device_name=rk322x_box
manufacturer=rockchip
model_name=rk322x-box
model_number=rk322x-box
serial_number=P1QRMMGNE3
device_type=10-0050F204-5
config_methods=physical_display virtual_push_button
p2p_no_go_freq=5170-5740
pmf=1
external_sim=1
wowlan_triggers=any
p2p_search_delay=0

network={
        ssid="poco"
        bssid=30:74:96:75:24:a7
        psk="3143531435"
        key_mgmt=WPA-PSK
        id_str="%7B%22creatorUid%22%3A%221000%22%2C%22configKey%22%3A%22%5C%22poco%5C%22WPA_PSK%22%7D"
}

hostapd.conf

cat /data/misc/wifi/hostapd.conf
interface=wlan0
driver=nl80211
ctrl_interface=/data/misc/wifi/hostapd
ssid=AndroidAP
channel=1
ieee80211n=1
hw_mode=g
ignore_broadcast_ssid=0
wowlan_triggers=any
wpa=2
rsn_pairwise=CCMP
wpa_psk=2a336741dae590f54e1c7924c940559a848840030ee965aaa178023ab490023b
STA模式
串口命令：
echo 1 > /sys/class/rkwifi/driver 驱动加载
ifconfig wlan0 up 启动网卡
wpa_supplicant -Dnl80211 -c /tmp/wpa_supplicant.config -iwlan0 -B 连接热点。
udhcpc -i wlan0 -b 获取ip地址
模组能够正常连接上wifi热点并获取到ip地址。wpa_supplicant的配置内容可以参考下面。系统在etc目录会有配置文件，只需要修改对应的ssid和密码，就可以直接使用该配置文件。
ctrl_interface=/var/run/wpa_supplicant
ap_scan=1
network={
proto=RSN
key_mgmt=WPA-PSK
pairwise=CCMP TKIP
group=CCMP TKIP
ssid="name"
psk="password"
}

AP模式
串口命令：
echo 1 > sys/class/rkwifi/driver 驱动加载
ifconfig lo 127.0.0.1 netmask 255.255.255.0;
ifconfig wlan0 192.168.100.1 netmask 255.255.255.0 设置ip地址
dnsmasq -C /etc/dnsmasq.conf 启动dns
hostapd /tmp/hostapd.conf -B 启动ap模式
模组能够正常启动ap 模式。hostapd.conf配置内容参考如下
ctrl_interface=/tmp/hostapd
interface=wlan0
driver=nl80211
ssid="name"
channel=6
hw_mode=g
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase="password"
rsn_pairwise=CCMP

```

11、概率性打不开可能和这个有关系
```

--- a/brcm_tools/brcm_patchram_plus1.c
+++ b/brcm_tools/brcm_patchram_plus1.c
@@ -179,7 +179,7 @@ static const fw_auto_detection_entry_t fw_auto_detection_table[] = {
        {"BCM4359C0","BCM4359C0"},  //AP6359
        {"BCM4349B1","BCM4359B1"},  //AP6359
        {"BCM4359C0","BCM4359C0"},      //AP6398s
-       {(const char *) NULL, NULL}
+       {(char *) NULL, NULL}
 };
 int uart_fd = -1;
 int hcdfile_fd = -1;

```
12、linux对蓝牙版本的更新
```
.repo/repo/repo sync -c linux/external/deviceio_release linux/external/bluetooth_bsa 
linux/external/broadcom_bsa linux/external/rkwifibt linux/external/app linux/docs --no-tags
```

