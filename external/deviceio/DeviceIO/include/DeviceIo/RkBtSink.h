#ifndef __BLUETOOTH_SINK_H__
#define __BLUETOOTH_SINK_H__

#include <DeviceIo/RkBtBase.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	RK_BT_SINK_STATE_IDLE = 0,
	RK_BT_SINK_STATE_CONNECT,
	RK_BT_SINK_STATE_PLAY,
	RK_BT_SINK_STATE_PAUSE,
	RK_BT_SINK_STATE_STOP,
	RK_BT_SINK_STATE_DISCONNECT
} RK_BT_SINK_STATE;

typedef int (*RK_BT_SINK_CALLBACK)(RK_BT_SINK_STATE state);
typedef void (*RK_BT_SINK_VOLUME_CALLBACK)(int volume);

int rk_bt_sink_register_callback(RK_BT_SINK_CALLBACK cb);
int rk_bt_sink_register_volume_callback(RK_BT_SINK_VOLUME_CALLBACK cb);
int rk_bt_sink_open();
int rk_bt_sink_set_visibility(const int visiable, const int connectable);
int rk_bt_sink_close(void);
int rk_bt_sink_get_state(RK_BT_SINK_STATE *p_state);
int rk_bt_sink_play(void);
int rk_bt_sink_pause(void);
int rk_bt_sink_prev(void);
int rk_bt_sink_next(void);
int rk_bt_sink_stop(void);
int rk_bt_sink_volume_up(void);
int rk_bt_sink_volume_down(void);
int rk_bt_sink_set_volume(int volume);
int rk_bt_sink_disconnect();

#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_SINK_H__ */