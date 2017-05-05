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

// LCDMenuLib_add(id, group, prev_layer_element, new_element_num, lang_char_array, callback_function)
#define _LCDML_DISP_cnt    32
LCDML_DISP_init(_LCDML_DISP_cnt);
LCDML_DISP_add(0, _LCDML_G1, LCDML_root, 1, "Sensor", LCDML_FUNC);
LCDML_DISP_add(1, _LCDML_G1, LCDML_root_1, 1, "\232berblick", LCDML_FUNC_sensor_overview);
LCDML_DISP_add(2, _LCDML_G1, LCDML_root_1, 2, "Accel/Gyro max.", LCDML_FUNC_sensor_min_max);
LCDML_DISP_add(3, _LCDML_G1, LCDML_root_1, 3, "Temp/Druck/H\224he", LCDML_FUNC);
LCDML_DISP_add(4, _LCDML_G1, LCDML_root_1, 4, "Kompass", LCDML_FUNC);
LCDML_DISP_add(5, _LCDML_G1, LCDML_root_1, 5, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(6, _LCDML_G1, LCDML_root, 2, "Status", LCDML_FUNC);
LCDML_DISP_add(7, _LCDML_G1, LCDML_root_2, 1, "WLAN/Batterie/Zeit", LCDML_FUNC_status_wlan);
LCDML_DISP_add(8, _LCDML_G1, LCDML_root_2, 2, "MQTT", LCDML_FUNC);
LCDML_DISP_add(9, _LCDML_G1, LCDML_root_2, 3, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(10, _LCDML_G1, LCDML_root, 3, "Konfiguration", LCDML_FUNC);
LCDML_DISP_add(11, _LCDML_G1, LCDML_root_3, 1, "WLAN", LCDML_FUNC);
LCDML_DISP_add(12, _LCDML_G1, LCDML_root_3, 2, "Sensor", LCDML_FUNC);
LCDML_DISP_add(13, _LCDML_G1, LCDML_root_3_2, 1, "Accel", LCDML_FUNC);
LCDML_DISP_add(14, _LCDML_G1, LCDML_root_3_2, 2, "Gyro", LCDML_FUNC);
LCDML_DISP_add(15, _LCDML_G1, LCDML_root_3_2, 3, "Kompass", LCDML_FUNC);
LCDML_DISP_add(16, _LCDML_G1, LCDML_root_3_2, 4, "H\224he", LCDML_FUNC_config_altitude);
LCDML_DISP_add(17, _LCDML_G1, LCDML_root_3_2, 5, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(18, _LCDML_G1, LCDML_root_3, 3, "MQTT", LCDML_FUNC);
LCDML_DISP_add(19, _LCDML_G1, LCDML_root_3, 4, "EEPROM", LCDML_FUNC);
LCDML_DISP_add(20, _LCDML_G1, LCDML_root_3_4, 1, "In EEPROM speichern", LCDML_FUNC);
LCDML_DISP_add(21, _LCDML_G1, LCDML_root_3_4, 2, "Aus EEPROM lesen", LCDML_FUNC_config_altitude);
LCDML_DISP_add(22, _LCDML_G1, LCDML_root_3_4, 3, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(23, _LCDML_G1, LCDML_root_3, 5, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(24, _LCDML_G1, LCDML_root, 4, "Information", LCDML_FUNC);
LCDML_DISP_add(25, _LCDML_G1, LCDML_root_4, 1, "Besitzer", LCDML_FUNC_ownerinformation);
LCDML_DISP_add(26, _LCDML_G1, LCDML_root_4, 2, "QR-Code", LCDML_FUNC_test2);
LCDML_DISP_add(27, _LCDML_G1, LCDML_root_4, 3, "Sensor-Ausrichtung", LCDML_FUNC);
LCDML_DISP_add(28, _LCDML_G1, LCDML_root_4_3, 1, "Accel", LCDML_FUNC);
LCDML_DISP_add(29, _LCDML_G1, LCDML_root_4_3, 2, "Gyro", LCDML_FUNC);
LCDML_DISP_add(30, _LCDML_G1, LCDML_root_4_3, 3, "Kompass", LCDML_FUNC);
LCDML_DISP_add(31, _LCDML_G1, LCDML_root_4_3, 4, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_add(32, _LCDML_G1, LCDML_root_4, 4, "Zur\201ck", LCDML_FUNC_back);
LCDML_DISP_createMenu(_LCDML_DISP_cnt);

#define _LCDML_BACK_cnt    1  // last backend function id
LCDML_BACK_init(_LCDML_BACK_cnt);
LCDML_BACK_new_timebased_dynamic(0, (20UL), _LCDML_start, LCDML_BACKEND_control);
LCDML_BACK_new_timebased_dynamic(1, (10000000UL), _LCDML_stop, LCDML_BACKEND_menu);
LCDML_BACK_create();
