#ifndef __WIFI_TEST_H__
#define __WIFI_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

void rk_wifi_airkiss_start(void *data);
void rk_wifi_airkiss_stop(void *data);
void rk_wifi_softap_start(void *data);
void rk_wifi_softap_stop(void *data);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_TEST_H__ */
