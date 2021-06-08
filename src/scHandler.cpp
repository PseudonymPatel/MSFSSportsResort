//
// Created by Sheen on 6/6/2021.
//

#include <windows.h>
//#include <tchar.h>
#include <strsafe.h>
#include <iostream>

#include "SimConnect.h"
#include "scHandler.h"

#define RADCONV 0.01745329251  // pi/180

//gets the sign from a number, from SO
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

int quit = 0;

///notification groups to add events to, so when one of eventIDs in a group changes, the group gets notified.
enum DEFINITIONS {
    AIRCRAFT_MOVEMENT,
    AIRCRAFT_POSITION,
};

structAircraftPositions aircraftCurrentPosition;

/**
* callback function handling all communication with server.
* @param pData points to the data with what was recieved
* @param cbData size of the data buffer pData
* @param pContext "contains the pointer specified by the client in the CallDispatch function call"
 *
 * @return integer representing if the result was the null thing or not. 1 if null
*/
void CALLBACK MyDispatchProc1(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext) {

	switch(pData->dwID) {
		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: {
			auto *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

			switch(pObjData->dwRequestID) {
				case AIRCRAFT_POSITION: {
					auto *pos = (structAircraftPositions*) &pObjData->dwData;
					aircraftCurrentPosition.pitch = pos -> pitch;
					aircraftCurrentPosition.roll = pos -> roll;
					break;
				}
				default:
					break;
			}
			break;
		}
		case SIMCONNECT_RECV_ID_EXCEPTION: {
			std::cout << "Received exception:" << std::endl;
			auto *err = (SIMCONNECT_RECV_EXCEPTION*) pData;
			std::cout << err->dwException << std::endl;
			break;
		}

		case SIMCONNECT_RECV_ID_QUIT: {
			quit = 1;
			break;
		}

		default:
			break;
	}
}

/**
 * In this class, we want:
 *  connect to sim
 *  disconnect from sim
 *  get plane values from sim
 *      pitch, roll
 *  send inputs to sim based on wiimote position and plane values
 *  know when sim is in menu versus running.
 */
SCHandler::SCHandler() {
	aircraftMovementToSend.pitch = 0;
	aircraftMovementToSend.roll = 0;
	hSimConnect = nullptr;
	pitch = 0;
	roll = 0;
}

void SCHandler::connectToSC() {
    HRESULT hr;

    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Input Event", nullptr, 0, 0, 0))) {
        std::cout << "Connected to SimConnect" << std::endl;

        hr = SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_MOVEMENT, "ELEVATOR POSITION", "radian");
        if (FAILED(hr)) std::cout << "Failed 1" << std::endl;
        hr = SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_MOVEMENT, "AILERON POSITION", "radian");
	    if (FAILED(hr)) std::cout << "Failed 2" << std::endl;

	    hr = SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_POSITION, "PLANE PITCH DEGREES", "radian");
	    if (FAILED(hr)) std::cout << "Failed 3" << std::endl;
	    hr = SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_POSITION, "PLANE BANK DEGREES", "radian");
	    if (FAILED(hr)) std::cout << "Failed 4" << std::endl;

	    hr = SimConnect_RequestDataOnSimObject(hSimConnect, AIRCRAFT_POSITION, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);
	    if (FAILED(hr)) std::cout << "Failed requestdataonsimobject" << std::endl;
    }
}

void SCHandler::disconnectFromSC() {
    if(SUCCEEDED(SimConnect_Close(&hSimConnect))) {
        std::cout << "Disconnected and cleaned up SimConnect server connection." << std::endl;
    }
}

/**
 * Function that will send the positions to set the elevator and aileron to
 * Will take wiimote positions to do calculations with.
 *
 * todo: use wiimote and airplane positions to calculate correct stuff.
 */
void SCHandler::sendPositions(double wiimotePitch, double wiimoteRoll) {
	double deltaRoll = (-wiimoteRoll*RADCONV) - aircraftCurrentPosition.roll;
	double deltaPitch = (wiimotePitch*RADCONV) - aircraftCurrentPosition.pitch; //account for differences in signage
	double elevMag = std::abs(deltaRoll);
	double aileronMag = std::abs(deltaPitch);
	//std::cout << "deltaRoll: " << deltaRoll << std::endl;
	std::cout << "deltaPitch:" << deltaPitch << std::endl;
	// wiimoteRoll - aircraftRoll is the direction to send, assuming the signs are in the same direction.
	// the magnitude can be calculated by the difference between the two, with a larger difference meaning a larger
	// magnitude of movement in opposite direction -- 0.5 (difference)*(difference)
	aircraftMovementToSend.roll = -sgn(deltaRoll) * elevMag;
	aircraftMovementToSend.pitch = -sgn(deltaPitch) * aileronMag;

	HRESULT hr;
	hr = SimConnect_SetDataOnSimObject(hSimConnect, AIRCRAFT_MOVEMENT, SIMCONNECT_OBJECT_ID_USER, 0, 1, sizeof(aircraftMovementToSend), &aircraftMovementToSend);
	if (SUCCEEDED(hr)) {
		//std::cout << "transmitted successfully" << std::endl;
	}
}

void SCHandler::dispatchLoop() {
	std::cout << "Started dispatch loop" << std::endl;
	while (quit == 0) {
		SimConnect_CallDispatch(hSimConnect, MyDispatchProc1, NULL);
		Sleep(10);
	}
}