// *********************************************************************
// CONTROL TASK, DO NOT CHANGE
// *********************************************************************
void LCDML_BACK_setup(LCDML_BACKEND_control)
// *********************************************************************
{
	// call setup
	LCDML_CONTROL_setup();
}
// backend loop
boolean LCDML_BACK_loop(LCDML_BACKEND_control)
{
	// call loop
	LCDML_CONTROL_loop();

	// go to next backend function and do not block it
	return true;
}
// backend stop stable
void LCDML_BACK_stable(LCDML_BACKEND_control)
{
}

// *********************************************************************
// setup
void LCDML_CONTROL_setup()
{
}

// *********************************************************************
// loop
void LCDML_CONTROL_loop()
{
}