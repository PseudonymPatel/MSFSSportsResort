# MSFSSportsResort
Bringing the fun of Wii Sports Resort Island Flyover to Microsoft Flight Simulator 2020! Fly a plane with your Wii remote's motion controls.
This is currently in more of a proof-of-concept phase, although the advertised fuctionality is fully there.

# HOW TO USE
1. Get release from the releases section on the right sidebar.
2. Make sure wii remote is connected via bluetooth (the wii's leds on the bottom should be constantly blinking and should not turn off)
3. Have the simulator open and preferably loaded into a flight.
4. Run the program, it will take about 15 seconds max to find your Wiimote, restart the program if it has a hard time.

## Quirks
* **Don't point the Wiimote near straight down or straight up, there's some quirky accelerometer behavior at those points**

# Building
It's a cmake project, all the dependencies are included (should I have done that?)  
I used Visual Studio compiler.  
Only works on windows of course.
