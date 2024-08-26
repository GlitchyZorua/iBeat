/*
**  qpom.h
**  Christopher Osburn 911102
**
**  Function declarators for use with qpom (phase of the moon)
**  functions defined in qpom.c
*/
#include <time.h>

#define LPERIOD  (2551442.9)   /* synodic period of the moon, seconds,
                                  from astronomical almanac 1992 */

#define EPOCH    (811961700L)  /* commencement of lunation 900
                                  1995 Sep 24 16:55 UT (assuming
                                  16:55:00 for lack of better info) */
#define LUNEPOCH (900)         /* see above */

#define IYEAR    (1416)        /* Islamic year corresponding to LUNEPOCH */
#define IMONTH   (5)           /* Islamic month corresponding to LUNEPOCH
                                  (1 = first month, etc.) */
#define HYEAR    (5756)        /* Hebrew year corresponding to LUNEPOCH */
#define HMONTH   (1)           /* Hebrew month corresponding to LUNEPOCH 
                                  (1 = first month, etc.) */
#define M_PI (3.141592) 

double MoonAgeInDays(time_t now);       /* Returns age of moon in days at time now */
double MoonAgeInSeconds(time_t now);

double MoonIllumination(time_t now);      /* Returns fraction of lunar face illuminated
	                                  at time now; returns in [0.0, 1.0] */
int MoonLunation(time_t now);      /* Returns lunation number of active lunation
		                              at time now */
