/*!
* \file main.cpp
* \brief main file.
* \author Ndongmo Silatsa Fabrice
* \date 08-03-2019
* \version 2.0
*/

#include "Drone.h"

int main(int argc, char *argv[])
{
	int err;
	Drone drone;

	err = drone.begin();
	if (err < 0) return err;

	drone.start();

	err = drone.end();

	return err;
}