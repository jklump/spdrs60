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

enum {
    MIN_COLS = 4,     // min number of columns in a layout
    MAX_COLS = 200,   // max number of columns in a layout
    MIN_ROWS = 4,     // min number of rows in layout
    MAX_ROWS = 200,   // max number of rows in layout
    cDelayTime = 5000,// Timer for external buttons and routing.

    MAX_FB = 1984,    // number of total s88 feedback ports
    MAX_GAMM = 324,   // max number of motorola addresses
    MAX_GADCC = 2044, // max number of DCC addresses
    MAX_GASX = 114,   // max value of Selectrix addresses

    MIN_RB = 5000,    // min value of route button addresses
    MAX_RB = 5999,    // max value of route button addresses
    MIN_DISP = 6000,  // min value of train number displays
    MAX_DISP = 6999,  // max value of train number displays
    MIN_CROSS = 7000, // min value of crossings virtual address
    MAX_CROSS = 7999, // max value of crossings virtual address

    /*maximal length of an address edit line in dialogs*/
    LEMAXWIDTH = 55
};

#endif
