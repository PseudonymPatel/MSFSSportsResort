#pragma once

#include <wiiuse.h>

void handle_event(struct wiimote_t* wm);
void handle_ctrl_status(struct wiimote_t* wm);
void handle_disconnect(wiimote* wm);

short any_wiimote_connected(wiimote** wm, int wiimotes);