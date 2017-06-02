extern uint8_t gInitScreenPrevID;

void LCDML_lcd_menu_display()
{
	if (LCDML_DISP_update()) {
		uint8_t n_max = (LCDML.getChilds() >= _LCDML_rows) ? ((_LCDML_rows > _LCDML_rows_max) ? _LCDML_rows : _LCDML_rows_max) : (LCDML.getChilds());
		LCDML_lcd_menu_clear();

		for (uint8_t n = 0; n < n_max; n++) {
			if (n == LCDML.getCursorPos()) {
				display.setCursor(0, _LCDML_FONT_H * (n));
				display.write(0x10);
				//Serial.printf("lcdmenu: LCDML.getCursorPos() = %i, content_id[] = %i, layer = %i\n", LCDML.getCursorPos(), LCDML.content_id[n], LCDML.getLayer());
				gInitScreenPrevID = (LCDML.getLayer()) ? LCDML.content_id[n] : 255;
			}
			display.setCursor(_LCDML_FONT_W, _LCDML_FONT_H * (n));

			switch (LCDML.content_id[n]) {
			case LCDML_MENU_ALTITUDE:
				float altitude;
				char tempbuf[8];

				sensors.getAltitude(&altitude);
				dtostrf(altitude, 1, 0, tempbuf);
				display.printf("H\224he: %s m", tempbuf);
				//snprintf(g_LCDML_DISP_lang_table[LCDML.content_id[n]], 20, "H\224he: %s m", tempbuf);	// relevant for _use_ram
				break;

			default: // static content
				//display.print(LCDML_DISP_getRamContent(n));	// relevant for _use_ram
				display.println(LCDML.content[n]);
				break;
			}
		}

		if (LCDML.getChilds() > n_max) {
			// draw scrollbar
			display.drawRect(_LCDML_lcd_w - _LCDML_scrollbar_w, 0, _LCDML_scrollbar_w, _LCDML_lcd_h, _LCDML_TEXT_COLOR);
			uint8_t scrollbar_block_length = LCDML.getChilds() - n_max;
			scrollbar_block_length = _LCDML_lcd_h / (scrollbar_block_length + _LCDML_DISP_rows);
			if (LCDML.getCursorPosAbs() == 0) {
				display.fillRect(_LCDML_lcd_w - (_LCDML_scrollbar_w - 1), 1, (_LCDML_scrollbar_w - 2), scrollbar_block_length, _LCDML_TEXT_COLOR);
			}
			else if (LCDML.getCursorPosAbs() == (LCDML.getChilds())) {
				display.fillRect(_LCDML_lcd_w - (_LCDML_scrollbar_w - 1), _LCDML_lcd_h - scrollbar_block_length, (_LCDML_scrollbar_w - 2), scrollbar_block_length, _LCDML_TEXT_COLOR);
			}
			else {
				display.fillRect(_LCDML_lcd_w - (_LCDML_scrollbar_w - 1), (scrollbar_block_length * LCDML.getCursorPosAbs() + 1), (_LCDML_scrollbar_w - 2), scrollbar_block_length, _LCDML_TEXT_COLOR);
			}
		}
		display.display();
	}

	LCDML_DISP_update_end();
}

// lcd clear
void LCDML_lcd_menu_clear()
{
	display.clearDisplay();
}