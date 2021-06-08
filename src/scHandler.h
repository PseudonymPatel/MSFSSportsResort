#pragma once

struct structAircraftPositions {
	double pitch;
	double roll;
};

class SCHandler {
private:
	HANDLE hSimConnect;
	double pitch;
	double roll;
	structAircraftPositions aircraftMovementToSend{};

public:
	SCHandler();
	void connectToSC();
	void disconnectFromSC();
	void sendPositions(double wiimotePitch, double wiimoteRoll);
	void dispatchLoop();
};