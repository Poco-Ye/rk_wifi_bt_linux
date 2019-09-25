/*
 * Copyright (c) 2017 Rockchip, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <sys/select.h>
#include <linux/input.h>
#include <linux/rtc.h>
#include <DeviceIo/Rk_system.h>

#include "bt_test.h"
#include "rk_ble_app.h"
#include "rk_wifi_test.h"

static void deviceio_test_bluetooth();
static void deviceio_test_wifi_config();

typedef struct {
	const char *cmd;
	const char *desc;
	void (*action)(void);
} menu_command_t;

static menu_command_t menu_command_table[] = {
	{"bluetooth", "show bluetooth test cmd menu", deviceio_test_bluetooth},
	{"wificonfig", "show wifi config test cmd menu", deviceio_test_wifi_config},
};

typedef struct {
	const char *cmd;
	void (*action)(void *userdata);
} command_t;

static command_t wifi_config_command_table[] = {
	{"", NULL},
	{"ble_wifi_config_start", rk_ble_wifi_init},
	{"ble_wifi_config_stop", rk_ble_wifi_deinit},
	{"airkiss_wifi_config_start", rk_wifi_airkiss_start},
	{"airkiss_wifi_config_stop", rk_wifi_airkiss_stop},
	{"softap_wifi_config_start", rk_wifi_softap_start},
	{"softap_wifi_config_stop", rk_wifi_softap_stop},
};

static command_t bt_command_table[] = {
	{"", NULL},
	{"bt_server_open", bt_test_bluetooth_init},
	{"bt_test_set_class", bt_test_set_class},
	{"bt_test_enable_reconnect", bt_test_enable_reconnect},
	{"bt_test_disable_reconnect", bt_test_disable_reconnect},
	{"bt_test_source_auto_start", bt_test_source_auto_start},
	{"bt_test_source_connect_status", bt_test_source_connect_status},
	{"bt_test_source_auto_stop", bt_test_source_auto_stop},
	{"bt_test_sink_open", bt_test_sink_open},
	{"bt_test_sink_visibility00", bt_test_sink_visibility00},
	{"bt_test_sink_visibility01", bt_test_sink_visibility01},
	{"bt_test_sink_visibility10", bt_test_sink_visibility10},
	{"bt_test_sink_visibility11", bt_test_sink_visibility11},
	{"bt_test_sink_status", bt_test_sink_status},
	{"bt_test_sink_music_play", bt_test_sink_music_play},
	{"bt_test_sink_music_pause", bt_test_sink_music_pause},
	{"bt_test_sink_music_next", bt_test_sink_music_next},
	{"bt_test_sink_music_previous", bt_test_sink_music_previous},
	{"bt_test_sink_music_stop", bt_test_sink_music_stop},
	{"bt_test_sink_disconnect", bt_test_sink_disconnect},
	{"bt_test_sink_set_volume", bt_test_sink_set_volume},
	{"bt_test_sink_close", bt_test_sink_close},
	{"bt_test_ble_start", bt_test_ble_start},
	{"bt_test_ble_write", bt_test_ble_write},
	{"bt_test_ble_stop", bt_test_ble_stop},
	{"bt_test_ble_get_status", bt_test_ble_get_status},
	{"bt_test_spp_open", bt_test_spp_open},
	{"bt_test_spp_write", bt_test_spp_write},
	{"bt_test_spp_close", bt_test_spp_close},
	{"bt_test_spp_status", bt_test_spp_status},
	{"bt_test_hfp_sink_open", bt_test_hfp_sink_open},
	{"bt_test_hfp_hp_open", bt_test_hfp_hp_open},
	{"bt_test_hfp_hp_accept", bt_test_hfp_hp_accept},
	{"bt_test_hfp_hp_hungup", bt_test_hfp_hp_hungup},
	{"bt_test_hfp_hp_redail", bt_test_hfp_hp_redial},
	{"bt_test_hfp_hp_report_battery", bt_test_hfp_hp_report_battery},
	{"bt_test_hfp_hp_set_volume", bt_test_hfp_hp_set_volume},
	{"bt_test_hfp_hp_close", bt_test_hfp_hp_close},
	{"bt_server_close", bt_test_bluetooth_deinit},
};

static void show_wifi_config_cmd() {
	unsigned int i;
	printf("#### Please Input Your Test Command Index ####\n");
	for (i = 1; i < sizeof(wifi_config_command_table) / sizeof(wifi_config_command_table[0]); i++) {
		printf("%02d.  %s \n", i, wifi_config_command_table[i].cmd);
	}
	printf("Which would you like: ");
}

static void show_bt_cmd() {
	unsigned int i;
	printf("#### Please Input Your Test Command Index ####\n");
	for (i = 1; i < sizeof(bt_command_table) / sizeof(bt_command_table[0]); i++) {
		printf("%02d.  %s \n", i, bt_command_table[i].cmd);
	}
	printf("Which would you like: ");
}

static void show_help(char *bin_name) {
	unsigned int i;
	printf("%s [Usage]:\n", bin_name);
	for (i = 0; i < sizeof(menu_command_table)/sizeof(menu_command_t); i++)
		printf("\t\"%s %s\":%s.\n", bin_name, menu_command_table[i].cmd, menu_command_table[i].desc);
}

static void deviceio_test_wifi_config()
{
	int i, item_cnt;
	char szBuf[64] = {0};

	item_cnt = sizeof(wifi_config_command_table) / sizeof(command_t);
	while(true) {
		memset(szBuf, 0, sizeof(szBuf));
		show_wifi_config_cmd();
		if(!std::cin.getline(szBuf, 64)) {
			std::cout << "error" << std::endl;
			continue;
		}

		i = atoi(szBuf);
		if ((i >= 1) && (i < item_cnt))
			wifi_config_command_table[i].action(NULL);
	}

	return;
}

static void deviceio_test_bluetooth()
{
	int i, item_cnt;
	char szBuf[64] = {0};

	item_cnt = sizeof(bt_command_table) / sizeof(command_t);
	while(true) {
		memset(szBuf, 0, sizeof(szBuf));
		show_bt_cmd();
		if(!std::cin.getline(szBuf, 64)) {
			std::cout << "error" << std::endl;
			continue;
		}

		i = atoi(szBuf);
		if ((i >= 1) && (i < item_cnt))
			bt_command_table[i].action(NULL);
	}

	return;
}

int main(int argc, char *argv[])
{
	int i, item_cnt;
	char version[64] = {0};

	RK_read_version(version, 64);
	std::cout << "version:" << version << std::endl;
	item_cnt = sizeof(menu_command_table) / sizeof(menu_command_t);

	if (argc < 2) {
		printf("ERROR:invalid argument.\n");
		show_help(argv[0]);
		return -1;
	}

	if ((!strncmp(argv[1], "-h", 2)) || (!strncmp(argv[1], "help", 4))) {
		show_help(argv[0]);
		return 0;
	}

	for (i = 0; i < item_cnt; i++) {
		if (!strncmp(argv[1], menu_command_table[i].cmd, strlen(menu_command_table[i].cmd)))
			break;
	}

	if (i >= item_cnt) {
		printf("ERROR:invalid menu cmd.\n");
		show_help(argv[0]);
		return -1;
	}

	menu_command_table[i].action();

	while(true)
		sleep(10);

	return 0;
}
