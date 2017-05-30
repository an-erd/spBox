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

// w/o use ram
#define _LCDML_DISP_cnt    38
LCDML_DISP_init(_LCDML_DISP_cnt);
LCDML_DISP_add(0, _LCDML_G1, LCDML_root, 1, "Sensor", LCDML_FUNC);
LCDML_DISP_add(1, _LCDML_G1, LCDML_root_1, 1, "\232berblick", LCDML_FUNC_sensor_overview);
LCDML_DISP_add(2, _LCDML_G1, LCDML_root_1, 2, "Accel/Gyro min/max", LCDML_FUNC_sensor_min_max);
LCDML_DISP_add(3, _LCDML_G1, LCDML_root_1, 3, "Accel max.", LCDML_FUNC_max_accel);
LCDML_DISP_add(4, _LCDML_G1, LCDML_root_1, 4, "Temp/Druck/H\224he", LCDML_FUNC_sensor_temp_press_alt);
LCDML_DISP_add(5, _LCDML_G1, LCDML_root_1, 5, "Kompass", LCDML_FUNC_kompass);
LCDML_DISP_add(6, _LCDML_G1, LCDML_root_1, 6, "H\224he", LCDML_FUNC_altitude);
LCDML_DISP_add(7, _LCDML_G1, LCDML_root_1, 7, "Wasserwaage", LCDML_FUNC_waterbubble);
LCDML_DISP_add(8, _LCDML_G1, LCDML_root_1, 8, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(9, _LCDML_G1, LCDML_root, 2, "Status", LCDML_FUNC);
LCDML_DISP_add(10, _LCDML_G1, LCDML_root_2, 1, "WLAN/Batterie", LCDML_FUNC_status_wlan);
LCDML_DISP_add(11, _LCDML_G7, LCDML_root_2, 2, "MQTT", LCDML_FUNC);
LCDML_DISP_add(12, _LCDML_G1, LCDML_root_2, 3, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(13, _LCDML_G1, LCDML_root, 3, "Konfiguration", LCDML_FUNC);
LCDML_DISP_add(14, _LCDML_G7, LCDML_root_3, 1, "WLAN", LCDML_FUNC);
LCDML_DISP_add(15, _LCDML_G1, LCDML_root_3, 2, "Sensor", LCDML_FUNC);
LCDML_DISP_add(16, _LCDML_G7, LCDML_root_3_2, 1, "Accel", LCDML_FUNC);
LCDML_DISP_add(17, _LCDML_G7, LCDML_root_3_2, 2, "Gyro", LCDML_FUNC);
LCDML_DISP_add(18, _LCDML_G7, LCDML_root_3_2, 3, "Kompass", LCDML_FUNC);
LCDML_DISP_add(19, _LCDML_G1, LCDML_root_3_2, 4, "H\224he", LCDML_FUNC_config_altitude);
#define LCDML_MENU_ALTITUDE	19
LCDML_DISP_add(20, _LCDML_G1, LCDML_root_3_2, 5, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(21, _LCDML_G7, LCDML_root_3, 3, "MQTT", LCDML_FUNC);
LCDML_DISP_add(22, _LCDML_G1, LCDML_root_3, 4, "Werkseinstellung", LCDML_FUNC);
LCDML_DISP_add(23, _LCDML_G1, LCDML_root_3_4, 1, "Ger\204t zur\201cksetzen", LCDML_FUNC);
LCDML_DISP_add(24, _LCDML_G1, LCDML_root_3_4_1, 1, "Jetzt zur\201cksetzen", LCDML_FUNC_reset);
LCDML_DISP_add(25, _LCDML_G1, LCDML_root_3_4_1, 2, "Abbruch", LCDML_FUNC_back);
LCDML_DISP_add(26, _LCDML_G1, LCDML_root_3_4, 2, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(27, _LCDML_G1, LCDML_root_3, 5, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(28, _LCDML_G1, LCDML_root, 4, "Information", LCDML_FUNC);
LCDML_DISP_add(29, _LCDML_G1, LCDML_root_4, 1, "Uhrzeit", LCDML_FUNC_clock);
LCDML_DISP_add(30, _LCDML_G1, LCDML_root_4, 2, "Uptime", LCDML_FUNC_uptime);
LCDML_DISP_add(31, _LCDML_G1, LCDML_root_4, 3, "Besitzer", LCDML_FUNC_ownerinformation);
LCDML_DISP_add(32, _LCDML_G1, LCDML_root_4, 4, "Sensor-Ausrichtung", LCDML_FUNC);
LCDML_DISP_add(33, _LCDML_G1, LCDML_root_4_4, 1, "Accel", LCDML_FUNC_scrolltest);
LCDML_DISP_add(34, _LCDML_G1, LCDML_root_4_4, 2, "Gyro", LCDML_FUNC);
LCDML_DISP_add(35, _LCDML_G1, LCDML_root_4_4, 3, "Kompass", LCDML_FUNC);
LCDML_DISP_add(36, _LCDML_G1, LCDML_root_4_4, 4, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(37, _LCDML_G1, LCDML_root_4, 5, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(38, _LCDML_G7, LCDML_root, 5, "Initscreen", LCDML_FUNC_initscreen); // in g7 => hidden
LCDML_DISP_createMenu(_LCDML_DISP_cnt);

#define _LCDML_BACK_cnt    1  // last backend function id
LCDML_BACK_init(_LCDML_BACK_cnt);
LCDML_BACK_new_timebased_dynamic(0, (20UL), _LCDML_start, LCDML_BACKEND_control);
LCDML_BACK_new_timebased_dynamic(1, (10000000UL), _LCDML_stop, LCDML_BACKEND_menu);
LCDML_BACK_create();
