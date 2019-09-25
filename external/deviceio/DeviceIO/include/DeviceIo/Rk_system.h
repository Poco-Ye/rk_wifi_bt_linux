#ifndef __RK_SYSTEM_H__
#define __RK_SYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 *Version 1.2.3 Release 2019/06/05
  1.bluez:add a2dp sink volume api
  2.bluez:fix rk_bt_sink_set_visibility api
  3.bluez:fix rk_bt_enable_reconnect api
  4.bluez:cancel spp's dependence on a2dp sink
  5.bluez:fix automatic reconnection coredump
  6.bsa: fix a2dp source can't auto-connect sink devices
  7.bsa：fix rk_bt_hfp_hangup api
 *Version 1.2.2 Release 2019/05/27
  1.add bt source avrcp function
  2.add hfp hf api
  3.add set bt device class api
  4.add auto reconnect for sink & hfp
  5.add softap network config
  6.add airkiss network config
 *Version 1.2.1 Release 2019/04/29
  1.deviceio_test blewifi: register ble data recv callback(BSA)
  2.fix bug with program stuck when starting bluetooth related service(BLUEZ)
  3.fixed a bug that failed to get a bluetooth device role(BLUEZ)
 *Version 1.2.0 Release 2019/04/15
  1. bluethood api: add support for bsa.
  2. add support for deviceio_test.
 *Version 1.1.0 Release 2019/03/27
  1.rewrite the bt api include ble/a2dp/spp
 *Version 1.0.4 Release 2019/01/29
  1.ble_wifi
    1. add config timeout to discon ble
    2. fix gatt_open flag
    3. add reconfigure when last config fail
 *Version 1.0.3 Release 2019/01/24
  1.ble_wifi
    1. fix netstatus print error when NETLINK_NETWORK_CONFIG_WRONG_KEY_FAILED
    2. notify NETLINK_NETWORK_SUCCEEDED when net recovery from config failed
 *Version 1.0.2 Release 2019/01/24
  1.ble_wifi
    1. support wep key
    2. optimize wpa_supplicant.conf
    3. fix many times reports sink connected
    4. add ble_config wifi timeout value
    5. fix chinese code for hisense
  2.Rk_key: add fix register long press callback with different keyevent

 *Version 1.0.1 Release 2019/01/07
  1.ble_wifi:	fix Chinese coding problem
		fix ble report event and add wifi priority
		add wrong key event callback
		add initBTForHis interface
  2.volume:	setVolume support zero
  3.propery:	implement RK_property
  4.player:	separate mediaplayer and deviceio
		add playlist function
 *Version 1.0.0 Release 2018/12/22
 */

#define DEVICEIO_VERSION "V1.2.3"

int RK_read_chip_id(char *buffer, const int size);
int RK_read_version(char *buffer, const int size);
int RK_system_factory_reset(const int reboot = 1);


#ifdef __cplusplus
}
#endif

#endif
