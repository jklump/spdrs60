/*
 * File with gobal resources
 */


#ifndef RESOURCES_H
#define RESOURCES_H

/* include AC-Variables  */
#ifdef WIN32
#include "config_w32.h"
#else
#include "config.h"
#endif

#define   MIN_COLS         4   // min number of columns in a layout
#define   MAX_COLS         200 // max number of columns in a layout
                               // change value if you need bigger ones
#define   MIN_ROWS         4   // min number of rows in layout
#define   MAX_ROWS         200 // max number of rows in layout

#define   cDelayTime       5000 // Timer for external buttons and routing.

/**********************************************************************
  DO  NOT  EDIT  ANYTHING  BEYOND  THIS  LINE!
 **********************************************************************/

#define MAX_FB         1984    // number of total s88 feedback ports
#define MAX_GAMM       324     // max number of motorola addresses
#define MAX_GADCC      4096    // max number of DCC addresses
#define MAX_GASX       114     // max value of Selectrix addresses

#define MIN_RB         5000    // min value of route button addresses
#define MAX_RB         6000    // max value of route button addresses
#define MIN_DISP       6000    // min value of train number displays
#define MAX_DISP       6999    // max value of train number displays

/*maximal length of an address edit line in dialogs*/
#define LEMAXWIDTH 55

#endif
