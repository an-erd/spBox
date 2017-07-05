#pragma once

#define _LCDML_TEXT_COLOR		WHITE
#define _LCDML_FONT_SIZE		1
#define _LCDML_FONT_W			(6*_LCDML_FONT_SIZE)
#define _LCDML_FONT_H			(8*_LCDML_FONT_SIZE)
#define _LCDML_lcd_w			128
#define _LCDML_lcd_h			32
#define _LCDML_cols_max			(_LCDML_lcd_w/_LCDML_FONT_W)
#define _LCDML_rows_max			(_LCDML_lcd_h/_LCDML_FONT_H)
#define _LCDML_cols				20
#define _LCDML_rows				_LCDML_rows_max
#define _LCDML_scrollbar_w		6
#define _LCDML_DISP_cols		_LCDML_cols
#define _LCDML_DISP_rows		_LCDML_rows
#define _LCDML_DISP_cfg_initscreen_time			10000
//#define _LCDML_DISP_cfg_enable_use_ram_mode		1

#define _LCDML_DISP_cnt    66
LCDML_DISP_init(_LCDML_DISP_cnt);
LCDML_DISP_add(0, _LCDML_G1, LCDML_root, 1, "Haussteuerung", LCDML_FUNC);
LCDML_DISP_add(1, _LCDML_G7, LCDML_root_1, 1, "EG", LCDML_FUNC);
LCDML_DISP_add(2, _LCDML_G7, LCDML_root_1_1, 1, "Lampe            %s", LCDML_FUNC_mqtt_toggle);
LCDML_DISP_add(3, _LCDML_G7, LCDML_root_1_1, 2, "Lichtleiste      %s", LCDML_FUNC_mqtt_dimmer);
LCDML_DISP_add(4, _LCDML_G7, LCDML_root_1_1, 3, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(5, _LCDML_G7, LCDML_root_1, 2, "1. OG", LCDML_FUNC);
LCDML_DISP_add(6, _LCDML_G7, LCDML_root_1_2, 1, "Lampe            %s", LCDML_FUNC_mqtt_toggle);
LCDML_DISP_add(7, _LCDML_G7, LCDML_root_1_2, 2, "Lichtleiste      %s", LCDML_FUNC_mqtt_dimmer);
LCDML_DISP_add(8, _LCDML_G7, LCDML_root_1_2, 3, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(9, _LCDML_G1, LCDML_root_1, 3, "2. OG", LCDML_FUNC);
#define LCDML_MENU_MQTT_2OG_L1		10
LCDML_DISP_add(10, _LCDML_G1, LCDML_root_1_3, 1, "Lampe            %s", LCDML_FUNC_mqtt_toggle);
LCDML_DISP_add(11, _LCDML_G7, LCDML_root_1_3, 2, "Lichtleiste      %s", LCDML_FUNC_mqtt_dimmer);
LCDML_DISP_add(12, _LCDML_G7, LCDML_root_1_3, 3, "Lampe            %s", LCDML_FUNC);
LCDML_DISP_add(13, _LCDML_G7, LCDML_root_1_3, 4, "Lampe            %s", LCDML_FUNC);
LCDML_DISP_add(14, _LCDML_G7, LCDML_root_1_3, 5, "Lampe            %s", LCDML_FUNC);
LCDML_DISP_add(15, _LCDML_G7, LCDML_root_1_3, 6, "Lampe            %s", LCDML_FUNC);
LCDML_DISP_add(16, _LCDML_G1, LCDML_root_1_3, 7, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(17, _LCDML_G1, LCDML_root_1, 4, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(18, _LCDML_G1, LCDML_root, 2, "Sensor", LCDML_FUNC);
LCDML_DISP_add(19, _LCDML_G1, LCDML_root_2, 1, "\232berblick", LCDML_FUNC_sensor_overview);
LCDML_DISP_add(20, _LCDML_G1, LCDML_root_2, 2, "Accel/Gyro min/max", LCDML_FUNC_sensor_min_max);
LCDML_DISP_add(21, _LCDML_G1, LCDML_root_2, 3, "Accel max.", LCDML_FUNC_max_accel);
LCDML_DISP_add(22, _LCDML_G1, LCDML_root_2, 4, "Temp/Druck/H\224he", LCDML_FUNC_sensor_temp_press_alt);
LCDML_DISP_add(23, _LCDML_G1, LCDML_root_2, 5, "Kompass", LCDML_FUNC_kompass);
LCDML_DISP_add(24, _LCDML_G1, LCDML_root_2, 6, "H\224he", LCDML_FUNC_altitude);
LCDML_DISP_add(25, _LCDML_G1, LCDML_root_2, 7, "Wasserwaage", LCDML_FUNC_waterbubble);
LCDML_DISP_add(26, _LCDML_G1, LCDML_root_2, 8, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(27, _LCDML_G1, LCDML_root, 3, "Status", LCDML_FUNC);
LCDML_DISP_add(28, _LCDML_G1, LCDML_root_3, 1, "WLAN/Batterie", LCDML_FUNC_status_wlan);
LCDML_DISP_add(29, _LCDML_G1, LCDML_root_3, 2, "MQTT", LCDML_FUNC_status_mqtt);
LCDML_DISP_add(30, _LCDML_G1, LCDML_root_3, 3, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(31, _LCDML_G1, LCDML_root, 4, "Konfiguration", LCDML_FUNC);
LCDML_DISP_add(32, _LCDML_G1, LCDML_root_4, 1, "WLAN", LCDML_FUNC);
#define LCDML_MENU_CONF_WLAN_TOGGLE		33
LCDML_DISP_add(33, _LCDML_G1, LCDML_root_4_1, 1, "WLAN aktiv:      %s", LCDML_FUNC_toggle_conf_wlan);
LCDML_DISP_add(34, _LCDML_G1, LCDML_root_4_1, 2, "Profil ausw\204hlen", LCDML_FUNC_select_wlan_profile);
LCDML_DISP_add(35, _LCDML_G1, LCDML_root_4_1, 3, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(36, _LCDML_G7, LCDML_root_4, 2, "Sensor", LCDML_FUNC);
LCDML_DISP_add(37, _LCDML_G7, LCDML_root_4_2, 1, "Accel", LCDML_FUNC);
LCDML_DISP_add(38, _LCDML_G7, LCDML_root_4_2, 2, "Gyro", LCDML_FUNC);
LCDML_DISP_add(39, _LCDML_G7, LCDML_root_4_2, 3, "Kompass", LCDML_FUNC);
#define LCDML_MENU_ALTITUDE			40
LCDML_DISP_add(40, _LCDML_G1, LCDML_root_4_2, 4, "H\224he", LCDML_FUNC_config_altitude);
LCDML_DISP_add(41, _LCDML_G1, LCDML_root_4_2, 5, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(42, _LCDML_G1, LCDML_root_4, 3, "MQTT", LCDML_FUNC);
LCDML_DISP_add(43, _LCDML_G1, LCDML_root_4_3, 1, "Einstellungen", LCDML_FUNC);
#define LCDML_MENU_MQTT_TOGGLE		44
LCDML_DISP_add(44, _LCDML_G1, LCDML_root_4_3_1, 1, "MQTT aktiv:      %s", LCDML_FUNC_toggle_mqtt);
#define LCDML_MENU_MQTT_HEALTHDATA	45
LCDML_DISP_add(45, _LCDML_G1, LCDML_root_4_3_1, 2, "MQTT Healthdata: %s", LCDML_FUNC_toggle_mqtthealth);
LCDML_DISP_add(46, _LCDML_G1, LCDML_root_4_3_1, 3, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(47, _LCDML_G1, LCDML_root_4_3, 2, "Broker ausw\204hlen", LCDML_FUNC_select_mqtt);
LCDML_DISP_add(48, _LCDML_G1, LCDML_root_4_3, 3, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(49, _LCDML_G1, LCDML_root_4, 4, "Werkseinstellung", LCDML_FUNC);
LCDML_DISP_add(50, _LCDML_G1, LCDML_root_4_4, 1, "Ger\204t zur\201cksetzen", LCDML_FUNC);
LCDML_DISP_add(51, _LCDML_G1, LCDML_root_4_4_1, 1, "Jetzt zur\201cksetzen", LCDML_FUNC_reset);
LCDML_DISP_add(52, _LCDML_G1, LCDML_root_4_4_1, 2, "Abbruch", LCDML_FUNC_back);
LCDML_DISP_add(53, _LCDML_G1, LCDML_root_4_4, 2, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(54, _LCDML_G1, LCDML_root_4, 5, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(55, _LCDML_G1, LCDML_root, 5, "Information", LCDML_FUNC);
LCDML_DISP_add(56, _LCDML_G1, LCDML_root_5, 1, "Zeit", LCDML_FUNC_clock);
LCDML_DISP_add(57, _LCDML_G1, LCDML_root_5, 2, "Besitzer", LCDML_FUNC_ownerinformation);
LCDML_DISP_add(58, _LCDML_G7, LCDML_root_5, 3, "Sensor-Ausrichtung", LCDML_FUNC);
LCDML_DISP_add(59, _LCDML_G1, LCDML_root_5_3, 1, "Accel", LCDML_FUNC_scrolltest);
LCDML_DISP_add(60, _LCDML_G1, LCDML_root_5_3, 2, "Gyro", LCDML_FUNC);
LCDML_DISP_add(61, _LCDML_G1, LCDML_root_5_3, 3, "Kompass", LCDML_FUNC);
LCDML_DISP_add(62, _LCDML_G1, LCDML_root_5_3, 4, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(63, _LCDML_G1, LCDML_root_5, 4, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(64, _LCDML_G2, LCDML_root, 6, "Entsperren", LCDML_FUNC_unlock);
LCDML_DISP_add(65, _LCDML_G1, LCDML_root, 7, "Sperren", LCDML_FUNC_lock);
LCDML_DISP_add(66, _LCDML_G7, LCDML_root, 8, "Initscreen", LCDML_FUNC_initscreen); // in g7 => hidden
LCDML_DISP_createMenu(_LCDML_DISP_cnt);

#define _LCDML_BACK_cnt    1  // last backend function id
LCDML_BACK_init(_LCDML_BACK_cnt);
LCDML_BACK_new_timebased_dynamic(0, (20UL), _LCDML_start, LCDML_BACKEND_control);
LCDML_BACK_new_timebased_dynamic(1, (10000000UL), _LCDML_stop, LCDML_BACKEND_menu);
LCDML_BACK_create();
