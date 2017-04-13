#pragma once

#define _LCDML_DISP_cfg_button_press_time          200    // button press time in ms
#define _ADAFRUITE_I2C_ADR    0x3C
#define _LCDML_ADAFRUITE_TEXT_COLOR WHITE
#define _LCDML_ADAFRUITE_FONT_SIZE   1
#define _LCDML_ADAFRUITE_FONT_W      (6*_LCDML_ADAFRUITE_FONT_SIZE)             // font width
#define _LCDML_ADAFRUITE_FONT_H      (8*_LCDML_ADAFRUITE_FONT_SIZE)             // font heigt
#define _LCDML_ADAFRUITE_lcd_w       128            // lcd width
#define _LCDML_ADAFRUITE_lcd_h       32             // lcd height
#define OLED_RESET 4 // not used / nicht genutzt bei diesem Display
#define _LCDML_ADAFRUITE_cols_max    (_LCDML_ADAFRUITE_lcd_w/_LCDML_ADAFRUITE_FONT_W)
#define _LCDML_ADAFRUITE_rows_max    (_LCDML_ADAFRUITE_lcd_h/_LCDML_ADAFRUITE_FONT_H)
#define _LCDML_ADAFRUITE_cols        20                   // max cols
#define _LCDML_ADAFRUITE_rows        _LCDML_ADAFRUITE_rows_max  // max rows
#define _LCDML_ADAFRUITE_scrollbar_w 6  // scrollbar width
#define _LCDML_DISP_cols      _LCDML_ADAFRUITE_cols
#define _LCDML_DISP_rows      _LCDML_ADAFRUITE_rows

#define _LCDML_DISP_cnt    2

// LCDMenuLib_add(id, group, prev_layer_element, new_element_num, lang_char_array, callback_function)
LCDML_DISP_init(_LCDML_DISP_cnt);
LCDML_DISP_add(0, _LCDML_G1, LCDML_root, 1, "Information", LCDML_FUNC_information);
LCDML_DISP_add(1, _LCDML_G1, LCDML_root, 2, "Time info", LCDML_FUNC_timer_info);
LCDML_DISP_add(2, _LCDML_G1, LCDML_root, 3, "Settings", LCDML_FUNC);
LCDML_DISP_createMenu(_LCDML_DISP_cnt);

#define _LCDML_BACK_cnt    1  // last backend function id
LCDML_BACK_init(_LCDML_BACK_cnt);
LCDML_BACK_new_timebased_dynamic(0, (20UL), _LCDML_start, LCDML_BACKEND_control);
LCDML_BACK_new_timebased_dynamic(1, (10000000UL), _LCDML_stop, LCDML_BACKEND_menu);
LCDML_BACK_create();
