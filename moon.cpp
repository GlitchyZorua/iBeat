/*
**  qpom.c
**  Christopher Osburn 911102
**
**  Routines returning age of the moon (qpom), quantity of lunar face
**  under illumination (illum), and the current lunation number
**  (lunation) using the extremely cheesy epoch/iteration method.
**
*/
#include <math.h>
#include "moon.h"

double MoonAgeInDays(time_t now)
{
	double diff, howfar;

	diff = (double) now - EPOCH;
	howfar = diff / LPERIOD;
	return LPERIOD * (howfar - (int) howfar) / 86400.0;
}

double MoonAgeInSeconds(time_t now)
{
	double diff, howfar;

	diff = (double) now - EPOCH;
	howfar = diff / LPERIOD;
	return LPERIOD * (howfar - (int) howfar);
}

double MoonIllumination(time_t now)
{
	double phase;

	phase = MoonAgeInDays(now);
	return (1.0 - cos((2.0 * M_PI * phase) / (LPERIOD / 86400.0))) / 2.0;
}

int MoonLunation(time_t now)
{
	long diff;

	diff = (long) now - EPOCH;
	return LUNEPOCH + diff / LPERIOD;
}
