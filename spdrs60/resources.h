/*
 * File with gobal resources
 */


#ifndef RESOURCES_H
#define RESOURCES_H

/* include AC-Variables  */
#include "config.h"

#define   MIN_COLS         4   // min number of columns in a layout
#define   MAX_COLS         120 // max number of columns in a layout
                               // change value if you need bigger ones
#define   MIN_ROWS         4   // min number of rows in layout
#define   MAX_ROWS         30  // max number of rows in layout

#define   cDelayTime       5000 // Timer for external buttons and routing.

#define   cNumRepeatCommands 3  //serd: Viessman Signale often need
                                // several attempts for reaching
                                // their correct position

#define   MAX_HISTORY      100 // max lines in debugging history

/**********************************************************************
  DO  NOT  EDIT  ANYTHING  BEYOND  THIS  LINE!
 **********************************************************************/

//   Resource dependant, do NOT edit
#define MAX_FB         1984    // number of total s88 feedback ports
#define MAX_GAMM       324     // max number of motorola addresses
#define MAX_GADCC      4096    // max number of DCC addresses
#define MAX_GASX       114     // max value of Selectrix addresses
#define MAX_RB         6000    // max value of route button addresses

/*maximal length of an address edit line in dialogs*/
#define LEMAXWIDTH 55

#define APP_NAME       "SpDrS60"
#define SPDRS60_INIT   ".spdrs60rc"   // program init filename

//   General, do NOT edit
#define   SRCH_TX          0   // search string should be in text field
#define   SRCH_A1          1   // search string should be in address 1 field
#define   SRCH_A2          2   // search string should be in address 2 field

#define   LOCATE_TIMER     5000// delay for edit mode after element locating

#define   LED_OFF          0   // LED states of an element = off
#define   LED_YEL          1   // route selected
#define   LED_RED          2   // occupied

//TODO: different feedback types
#define   FB_S88           0
#define   FB_I8255         1
#define   FB_M6051         2

#define   USAGE            1   // modes for digital turntable
#define   PROG             0

#define   LEFT             1   // turn modes for digital turntable
#define   RIGHT            0

// TODO: change to enum type kmtCmd, kmtInfo
#define   MT_CMD           0   // command message type
#define   MT_INFO          1   // info message type

// TODO: change to enum type khlHist, khlInfo, khlFeed
#define   HL_HINT          0   // hint history line
#define   HL_CMND          1   // command history line
#define   HL_INFO          2   // info history line
#define   HL_FEED          3   // feedback history line

#define   SINGLE           0   // search only one element
#define   MULTI            1   // search all elements

#define   CTX_ID_REP       901
#define   CTX_ID_TOGGLE    902
#define   CTX_ID_CLEAR     903
#define   CTX_ID_ROTATE    904

//   Element directions, do NOT edit
#define   DIR_HP0          0
#define   DIR_HP1          1
#define   DIR_HP2          2
#define   DIR_SH1          3
#define   DIR_Sh0          0  // bei Sperrsignalen, die Hauptsignalvarianten
#define   DIR_Sh1          1  // können nicht benutzt werden, da mit anderen
                              // Richtungswerten gearbeitet wird
#define   DIR_ENK_DW       0  // Entkuppler aus
#define   DIR_ENK_UP       1  // Entkuppler an
#define   DIR_REL0         0  // Relais aus
#define   DIR_REL1         1  // Relais an
#define   DIR_0            0
#define   DIR_1            1

//   Symbol names, do NOT edit
#define   SYM_HS           "signal_hs"
#define   SYM_HSS          "signal_hss"
#define   SYM_SS           "signal_ss"
#define   SYM_SSH          "signal_ssh" 
#define   SYM_SSS          "signal_sss" 
#define   SYM_WS           "signal_ws"
#define   SYM_VS           "signal_vs"
#define   SYM_ZP           "signal_zp"
#define   SYM_NRB          "signal_nrb" // not realy signals but rails
#define   SYM_SRB          "signal_srb" // with a routing button

#define   SYM_WEL          "weiche_links"
#define   SYM_WER          "weiche_rechts"
#define   SYM_DWL          "weiche_diag_links"
#define   SYM_DWR          "weiche_diag_rechts"
#define   SYM_WEY          "weiche_y"
#define   SYM_GER          "gerade"
#define   SYM_GET          "gerade_tl"
#define   SYM_BLD          "blind"           // Blindelement schaltbar
#define   SYM_ADR          "adresse"
#define   SYM_LEE          "leer"
#define   SYM_EKR          "ekw_rechts"
#define   SYM_EKL          "ekw_links"
#define   SYM_DKR          "dkw_rechts"
#define   SYM_DKL          "dkw_links"
#define   SYM_DIR          "diagonale_rechts"
#define   SYM_DIL          "diagonale_links"
#define   SYM_DRW          "dreier_weiche"
#define   SYM_PRE          "prellbock"  // buffer stop
#define   SYM_BUE          "uebergang"  // level crossing
#define   SYM_KUR          "kurve_rechts"
#define   SYM_KUL          "kurve_links"
#define   SYM_KRH          "kreuzung_hose"
#define   SYM_KRR          "kreuzung_rechts"
#define   SYM_KRL          "kreuzung_links"
#define   SYM_ENK          "entkoppler"
#define   SYM_RI1          "richtung_1"
#define   SYM_RI2          "richtung_2"
#define   SYM_DLT          "diagonale_links_tl"
#define   SYM_DRT          "diagonale_rechts_tl"
#define   SYM_REL          "relais"
#define   SYM_DRE          "drehscheibe" // turntable
#define   SYM_SBN          "schiebebuehne" // transfer table
#define   SYM_HS1          "haus_1"
#define   SYM_HS2          "haus_2"
#define   SYM_SHO          "schuppen_o" // loco shed
#define   SYM_SHM          "schuppen_m"
#define   SYM_SHU          "schuppen_u"
#define   SYM_MDC          "motor_dc"

#define   SYM_TAF          "taste_fht"
#define   SYM_TAU          "taste_ufgt" // combination with MGT
#define   SYM_TAW          "taste_wgt"
#define   SYM_TAS          "taste_sgt"  // combination with HaGT

#define   SYM_FEG          "panel_green"
#define   SYM_FEB          "panel_blue"
#define   SYM_FER          "panel_red"
#define   SYM_FEY          "panel_yellow"
#define   SYM_FEE          "panel_grey"
#define   SYM_FEN          "panel_brown"

//   special symbol name, do NOT edit
#define   SYM_KURR         "kurr"
#define   SYM_KULR         "kulr"


#endif
