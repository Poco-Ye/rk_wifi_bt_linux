
```
在sdk根目录，先 source envsetup.sh 选择你正在用的config，再执行下面的命令

1、external/rkwifibt替换成附件中的，在sdk根目录，make rkwifibt-dirclean && make rkwifibt-rebuild

2、用3308 SDK自带的脚本验证hfp 正常：
1）、运行bt_realtek_start，启动蓝牙
2）、运行bt_realtek_hfp_start，启动hfp，如果2mic_loopback 报声道设置异常，请将2mic_loopback改为6mic_loopback测试
3）、手机A 连接realtek_bt设备，手机B 拨打手机A 电话
4）、adb或串口运行 bt_realtek_hfp_accept 接通电话

```
