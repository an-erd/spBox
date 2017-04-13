void LCDML_lcd_menu_display()
{
	display.clearDisplay();
	display.setTextColor(_LCDML_TEXT_COLOR);
	display.setTextSize(_LCDML_FONT_SIZE);

	if (LCDML_DISP_update()) {
		// init vars
		uint8_t n_max = (LCDML.getChilds() >= _LCDML_rows) ? ((_LCDML_rows > _LCDML_rows_max) ? _LCDML_rows : _LCDML_rows_max) : (LCDML.getChilds());

		// display rows and cursor
		for (uint8_t n = 0; n < n_max; n++)
		{
			// set cursor
			if (n == LCDML.getCursorPos()) {
				display.setCursor(0, _LCDML_FONT_H * (n));
				//display.println(">");
				display.write(0x10);
			}

			// set content
			// with content id you can add special content to your static menu or replace the content
			// the content_id contains the id wich is set on main tab for a menuitem
			switch (LCDML.content_id[n])
			{
				//case 0: // dynamic content
				//	display.setCursor(0, _LCDML_FONT_H * (n));
				//	display.println("var_datetime");
				//	break;

			default: // static content
				display.setCursor(_LCDML_FONT_W, _LCDML_FONT_H * (n));
				display.println(LCDML.content[n]);
				break;
			}
		}

		// display scrollbar when more content as rows available
		if (LCDML.getChilds() > n_max) {
			// set frame for scrollbar
			display.drawRect(_LCDML_lcd_w - _LCDML_scrollbar_w, 0, _LCDML_scrollbar_w, _LCDML_lcd_h, _LCDML_TEXT_COLOR);

			// calculate scrollbar length
			uint8_t scrollbar_block_length = LCDML.getChilds() - n_max;
			scrollbar_block_length = _LCDML_lcd_h / (scrollbar_block_length + _LCDML_DISP_rows);

			//set scrollbar
			// hier muss du mal schauen wie du blöcke einfügen kannst, bei der u8glib geht das wie unten angezeigt, bei deiner lib weis ich das nicht
			if (LCDML.getCursorPosAbs() == 0) {                                   // top position     (min)
																				  //u8g.drawBox(_LCDML_lcd_w - (_LCDML_scrollbar_w-1), 1                                                    , (_LCDML_scrollbar_w-2)  , scrollbar_block_length);
			}
			else if (LCDML.getCursorPosAbs() == (LCDML.getChilds())) {            // bottom position  (max)
																				  //u8g.drawBox(_LCDML_lcd_w - (_LCDML_scrollbar_w-1), _LCDML_lcd_h - scrollbar_block_length            , (_LCDML_scrollbar_w-2)  , scrollbar_block_length);
			}
			else {                                                                // between top and bottom
																				  //u8g.drawBox(_LCDML_lcd_w - (_LCDML_scrollbar_w-1), (scrollbar_block_length * LCDML.getCursorPosAbs() + 1),(_LCDML_scrollbar_w-2)  , scrollbar_block_length);
			}
		}
	}

	display.display();

	// reinit some vars
	LCDML_DISP_update_end();
}

// lcd clear
void LCDML_lcd_menu_clear()
{
}