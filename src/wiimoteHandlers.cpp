//functions that handle wii remote events
// like generic_event, disconnect, status events, etc.

#include "wiimoteHandlers.h"
#include "wiiuse.h"

/**
 *	@brief Callback that handles an event.
 *
 *	@param wm		Pointer to a wiimote_t structure.
 *
 *	This function is called automatically by the wiiuse library when an
 *	event occurs on the specified wiimote.
 */
void handle_event(struct wiimote_t* wm) {
    //printf("\n\n--- EVENT [id %i] ---\n", wm->unid);

    /* if a button is pressed, report it */
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_A)) {
        printf("A pressed\n");
    }
    /*if (IS_PRESSED(wm, WIIMOTE_BUTTON_B)) {
        printf("B pressed\n");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP)) {
        printf("UP pressed\n");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN)) {
        printf("DOWN pressed\n");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT)) {
        printf("LEFT pressed\n");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT)) {
        printf("RIGHT pressed\n");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS)) {
        printf("MINUS pressed\n");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS)) {
        printf("PLUS pressed\n");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE)) {
        printf("ONE pressed\n");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO)) {
        printf("TWO pressed\n");
    }
    if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME)) {
        printf("HOME pressed\n");
    }*/

    /*
     *	Pressing minus will tell the wiimote we are no longer interested in movement.
     *	This is useful because it saves battery power.
     */
    if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS)) {
        wiiuse_motion_sensing(wm, 0);
        printf("Stopped motion sensing\n");
    }

    /*
     *	Pressing plus will tell the wiimote we are interested in movement.
     */
    if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS)) {
        wiiuse_motion_sensing(wm, 1);
        printf("enabled motion sensing\n");
    }

    /*
     *	Pressing B will toggle the rumble
     *
     *	if B is pressed but is not held, toggle the rumble
     */
    if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_B)) {
        wiiuse_toggle_rumble(wm);
    }

    /*
     * Motion+ support
     */
    if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_ONE)) {
        if (WIIUSE_USING_EXP(wm)) {
            wiiuse_set_motion_plus(wm, 2);    // nunchuck pass-through
        }
        else {
            wiiuse_set_motion_plus(wm, 1);    // standalone
        }
    }

    if (IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_TWO)) {
        wiiuse_set_motion_plus(wm, 0); // off
    }

//    /* if the accelerometer is turned on then print angles */
//    if (WIIUSE_USING_ACC(wm)) {
//        printf("wiimote roll  = %f [%f]\n", wm->orient.roll, wm->orient.a_roll);
//        printf("wiimote pitch = %f [%f]\n", wm->orient.pitch, wm->orient.a_pitch);
//        printf("wiimote yaw   = %f\n", wm->orient.yaw);
//    }

    /* show events specific to supported expansions */
    if (wm->exp.type == EXP_NUNCHUK || wm->exp.type == EXP_MOTION_PLUS_NUNCHUK) {
        /* nunchuk */
        struct nunchuk_t* nc = (nunchuk_t*)&wm->exp.nunchuk;

        if (IS_PRESSED(nc, NUNCHUK_BUTTON_C)) {
            printf("Nunchuk: C pressed\n");
        }
        if (IS_PRESSED(nc, NUNCHUK_BUTTON_Z)) {
            printf("Nunchuk: Z pressed\n");
        }

        printf("nunchuk roll  = %f\n", nc->orient.roll);
        printf("nunchuk pitch = %f\n", nc->orient.pitch);
        printf("nunchuk yaw   = %f\n", nc->orient.yaw);

        printf("nunchuk joystick angle:     %f\n", nc->js.ang);
        printf("nunchuk joystick magnitude: %f\n", nc->js.mag);

        printf("nunchuk joystick vals:      %f, %f\n", nc->js.x, nc->js.y);
        printf("nunchuk joystick calibration (min, center, max): x: %i, %i, %i  y: %i, %i, %i\n",
               nc->js.min.x,
               nc->js.center.x,
               nc->js.max.x,
               nc->js.min.y,
               nc->js.center.y,
               nc->js.max.y);
    }

    if (wm->exp.type == EXP_MOTION_PLUS ||
        wm->exp.type == EXP_MOTION_PLUS_NUNCHUK) {
        printf("Motion+ angular rates (deg/sec): pitch:%03.2f roll:%03.2f yaw:%03.2f\n",
               wm->exp.mp.angle_rate_gyro.pitch,
               wm->exp.mp.angle_rate_gyro.roll,
               wm->exp.mp.angle_rate_gyro.yaw);
    }
}

/**
 *	@brief Callback that handles a controller status event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *	@param attachment		Is there an attachment? (1 for yes, 0 for no)
 *	@param speaker			Is the speaker enabled? (1 for yes, 0 for no)
 *	@param ir				Is the IR support enabled? (1 for yes, 0 for no)
 *	@param led				What LEDs are lit.
 *	@param battery_level	Battery level, between 0.0 (0%) and 1.0 (100%).
 *
 *	This occurs when either the controller status changed
 *	or the controller status was requested explicitly by
 *	wiiuse_status().
 *
 *	One reason the status can change is if the nunchuk was
 *	inserted or removed from the expansion port.
 */
void handle_ctrl_status(struct wiimote_t* wm) {
    printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

    printf("attachment:      %i\n", wm->exp.type);
    printf("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
    printf("ir:              %i\n", WIIUSE_USING_IR(wm));
    printf("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
    printf("battery:         %f %%\n", wm->battery_level);
}

/**
 *	@brief Callback that handles a disconnection event.
 *
 *	@param wm				Pointer to a wiimote_t structure.
 *
 *	This can happen if the POWER button is pressed, or
 *	if the connection is interrupted.
 */
void handle_disconnect(wiimote* wm) {
    printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);
}


//void test(struct wiimote_t* wm, byte* data, unsigned short len) {
//	printf("test: %i [%x %x %x %x]\n", len, data[0], data[1], data[2], data[3]);
//}

short any_wiimote_connected(wiimote** wm, int wiimotes) {
    int i;
    if (!wm) {
        return 0;
    }

    for (i = 0; i < wiimotes; i++) {
        if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i])) {
            return 1;
        }
    }

    return 0;
}