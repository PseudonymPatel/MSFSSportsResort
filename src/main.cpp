// Entry point for MSFS Sports Resort Globe Flyover
// @author Sheen Patel

#include <iostream>
#include <thread>

#include "wiiuse.h"
#include "wiimoteHandlers.h"
#include "scHandler.h"

#define MAX_WIIMOTES 1

int main(int argc, char** argv) {

	SCHandler scHandler = SCHandler();

    std::cout << "Testing wiimotes:" << std::endl;
    wiimote** wiimotes;
    int found, connected;
    char shouldRetry;

    scHandler.connectToSC();

    //todo reapply threading
	//scHandler.dispatchLoop();
    std::thread dispatchThread([](SCHandler scHandler) {
    	scHandler.dispatchLoop();
    }, scHandler);
    /*
     *	Initialize an array of wiimote objects.
     *
     *	The parameter is the number of wiimotes I want to create.
     */
    wiimotes = wiiuse_init(MAX_WIIMOTES);

    //Repeat trying to find wiimotes at user's discretion.
    while (true) {
        std::cout << ("Searching for Wiimotes...\n") << std::endl;

        /*
         *	Find wiimote devices
         *
         *	Now we need to find some wiimotes.
         *	Give the function the wiimote array we created, and tell it there
         *	are MAX_WIIMOTES wiimotes we are interested in.
         *
         *	Set the timeout to be 5 seconds.
         *
         *	This will return the number of actual wiimotes that are in discovery mode.
         */
        found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);

        if (!found) {
            std::cout << "!! No wiimotes found. Retry? [Y/n]" << std::endl;
            shouldRetry = getchar();

            // flushes the standard input
            while ((getchar()) != '\n');

            if (shouldRetry == 'n' || shouldRetry == 'N') {
                std::cout << ("Stopped searching for Wiimotes, ending program.\n") << std::endl;
                return 0;
            }
        } else {
            break;
        }
    }


    /*
     *	Connect to the wiimotes
     *
     *	Now that we found some wiimotes, connect to them.
     *	Give the function the wiimote array and the number
     *	of wiimote devices we found.
     *
     *	This will return the number of established connections to the found wiimotes.
     */
    connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
    if (connected) {
        std::cout << ("Connected to %i wiimotes (of %i found).\n", connected, found) << std::endl;
    }
    else {
        std::cout << ("Failed to connect to any wiimote. Exiting program. \n") << std::endl;
        return 1;
    }

    /*
     *	Now set the LEDs and rumble for a second so it's easy
     *	to tell which wiimotes are connected (just like the wii does).
     */
    wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);
    /*wiiuse_set_leds(wiimotes[1], WIIMOTE_LED_2);
    wiiuse_set_leds(wiimotes[2], WIIMOTE_LED_3);
    wiiuse_set_leds(wiimotes[3], WIIMOTE_LED_4);*/
    wiiuse_rumble(wiimotes[0], 1);
    //wiiuse_rumble(wiimotes[1], 1);

#ifndef WIIUSE_WIN32
    usleep(200000);
#else
    Sleep(200);
#endif

    wiiuse_rumble(wiimotes[0], 0);
    //wiiuse_rumble(wiimotes[1], 0);

    std::cout << ("\nReady to GO!\n") << std::endl;

    /*
     *	Maybe I'm interested in the battery power of the 0th
     *	wiimote.  This should be WIIMOTE_ID_1 but to be sure
     *	you can get the wiimote associated with WIIMOTE_ID_1
     *	using the wiiuse_get_by_id() function.
     *
     *	A status request will return other things too, like
     *	if any expansions are plugged into the wiimote or
     *	what LEDs are lit.
     */
    /* wiiuse_status(wiimotes[0]); */

    /*
     *	This is the main loop
     *
     *	wiiuse_poll() needs to be called with the wiimote array
     *	and the number of wiimote structures in that array
     *	(it doesn't matter if some of those wiimotes are not used
     *	or are not connected).
     *
     *	This function will set the event flag for each wiimote
     *	when the wiimote has things to report.
     */
    while (any_wiimote_connected(wiimotes, MAX_WIIMOTES)) {

	    //check for any simconnect messages, like position updates
//	    while(scHandler.getNextDispatch() == 0) {
//		    //repeats getNextDispatch until null is received or some other things too.
//	    }

	    //now do wiimote stuff
    	float wiimotePitch = 0;
    	float wiimoteRoll = 0;
        if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
            /*
             *	This happens if something happened on any wiimote.
             *	So go through each one and check if anything happened.
             */
            int i = 0;
            for (; i < MAX_WIIMOTES; ++i) {
                switch (wiimotes[i]->event) {
                    case WIIUSE_EVENT:
                        /* a generic event occurred */
                        handle_event(wiimotes[i]);

                        if (WIIUSE_USING_ACC(wiimotes[i])) {
	                        struct wiimote_t* wm = wiimotes[i];
//	                        printf("wiimote roll  = %f [%f]\n", wm->orient.roll, wm->orient.a_roll);
//					        printf("wiimote pitch = %f [%f]\n", wm->orient.pitch, wm->orient.a_pitch);
					        wiimotePitch = wm->orient.pitch;
					        wiimoteRoll = wm->orient.roll;
					        scHandler.sendPositions(wiimotePitch, wiimoteRoll);
                        }

                        break;

                    case WIIUSE_STATUS:
                        /* a status event occurred */
                        handle_ctrl_status(wiimotes[i]);
                        break;

                    case WIIUSE_DISCONNECT:
                    case WIIUSE_UNEXPECTED_DISCONNECT:
                        /* the wiimote disconnected */
                        handle_disconnect(wiimotes[i]);
                        break;

                    case WIIUSE_READ_DATA:
                        /*
                         *	Data we requested to read was returned.
                         *	Take a look at wiimotes[i]->read_req
                         *	for the data.
                         */
                        break;

                    case WIIUSE_NUNCHUK_INSERTED:
                        /*
                         *	a nunchuk was inserted
                         *	This is a good place to set any nunchuk specific
                         *	threshold values.  By default they are the same
                         *	as the wiimote.
                         */
                        /* wiiuse_set_nunchuk_orient_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 90.0f); */
                        /* wiiuse_set_nunchuk_accel_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 100); */
                        std::cout << ("Nunchuk inserted.\n") << std::endl;
                        break;

                    case WIIUSE_CLASSIC_CTRL_INSERTED:
                        std::cout << ("Classic controller inserted.\n") << std::endl;
                        break;

                    case WIIUSE_WII_BOARD_CTRL_INSERTED:
                        std::cout << ("Balance board controller inserted.\n") << std::endl;
                        break;

                    case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
                        /* some expansion was inserted */
                        handle_ctrl_status(wiimotes[i]);
                        std::cout << ("Guitar Hero 3 controller inserted.\n") << std::endl;
                        break;

                    case WIIUSE_MOTION_PLUS_ACTIVATED:
                        std::cout << ("Motion+ was activated\n") << std::endl;
                        break;

                    case WIIUSE_NUNCHUK_REMOVED:
                    case WIIUSE_CLASSIC_CTRL_REMOVED:
                    case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
                    case WIIUSE_WII_BOARD_CTRL_REMOVED:
                    case WIIUSE_MOTION_PLUS_REMOVED:
                        /* some expansion was removed */
                        handle_ctrl_status(wiimotes[i]);
                        std::cout << ("An expansion was removed.\n") << std::endl;
                        break;

                    default:
                        break;
                }
            }
        }
    }

    /*
     *	Disconnect the wiimotes
     */
    wiiuse_cleanup(wiimotes, MAX_WIIMOTES);
    scHandler.disconnectFromSC();

    return 0;
}