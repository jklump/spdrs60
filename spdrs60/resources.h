/*
 * File with gobal resources
 */


#ifndef RESOURCES_H
#define RESOURCES_H

/* include AC-Variables  */
#include "config.h"

#define   MIN_COLS         4   // min number of columns in a layout
#define   MAX_COLS         250 // max number of columns in a layout
                               // change value if you need bigger ones
#define   MIN_ROWS         4   // min number of rows in layout
#define   MAX_ROWS         250 // max number of rows in layout

#define   cDelayTime       5000 // serd: Timer for WHT and Routing.
                                // 2000 ms was too Short for
                                // big layouts when working with
                                // touchpad (or my fingers are too slow:-)
#define   cNumRepeatCommands 3  //serd: Viessman Signale often need
                                // several attempts for reaching
                                // their correct position

#define   SMAJ             0   // SRCP version, major value
#define   SMIN             7   // SRCP version, minor value
#define   MAX_HISTORY      100 // max lines in debugging history

/**********************************************************************
  DO  NOT  EDIT  ANYTHING  BEYOND  THIS  LINE!
 **********************************************************************/

//   Resource dependant, do NOT edit
#define   EL_WIDTH         56  // width of an element in pixels
#define   EL_HEIGHT        35  // height of an element in pixels
#define   MAX_FB           1984// number of total s88 feedback ports

//#define   GBS_FILE_SUFFIX  ".dat.gbs"     // file appendix for layout files
#define   RTS_FILE_SUFFIX  ".dat.rts"     // file appendix for routing files
#define   S88_FILE_SUFFIX  ".dat.s88"     // file appendix for feedback files
#define   XPM_SUFFIX       ".xpm"         // file appendix for bitmap files
#define   SPDRS60_INIT     ".spdrs60rc"   // program init filename
#define   APP_NAME         "SpDrS60"

//   General, do NOT edit
#define   SRCH_TX          0   // search string should be in text field
#define   SRCH_A1          1   // search string should be in address 1 field
#define   SRCH_A2          2   // search string should be in address 2 field

#define   SRCP             0   // ID if prog works with a SRCP server
#define   EDITS            1   // ID if prog works with EDiTS Pro
#define   MAERKIF          2   // ID if prog works with Märklin IF
#define   INTELLI          3   // ID if prog works with a Intellibox

#define   VALID            1   // for validity check in various windows
#define   INVALID          0

#define   RED              1   // init state for signals
#define   LAYOUT           0

#define   TEXT             1   // show text or address in elements
#define   ADDRESS          0

#define   LOAD_TIMER_TIME  1000// delay for autoloader
#define   LOCATE_TIMER     5000// delay for edit mode after element locating

#define   LOCKED           1   // state of a routing
#define   UNLOCKED         0

#define   SET              0   // activities for a routing
#define   RESET            1

#define   LED_OFF          0   // LED states of an element = off
#define   LED_YEL          1   // route selected
#define   LED_RED          2   // occupied

#define   PROT_MS          1   // protocol is maerklin/motorola
#define   PROT_NA          0   // protocol is dcc/nmra

#define   FB_16            0   // feedback types
#define   FB_8             1

#define   USAGE            1   // modes for digital turntable
#define   PROG             0

#define   LEFT             1   // turn modes for digital turntable
#define   RIGHT            0

#define   HIST             0
#define   CMD              0   // different types for debugging window
#define   INFO             1
#define   FEED             2

#define   R_SHOW_STA       30  // R_EDIT_CLICKED + 0
#define   R_SHOW_STO       31  // R_EDIT_CLICKED + 1
#define   R_SHOW_ELM       32  // R_EDIT_CLICKED + 2

#define   R_SHOW           4   // = REC_SHOW
#define   R_EDIT_CLICKED   3   // edit mode for routings
#define   R_EDIT           2   // = REC_START  edit mode for routings
#define   L_EDIT           1   // edit mode for a layout
#define   NOEDIT           0   // also = REC_STOPP

#define   REC_STASTO       0   // clicked element while recording is a signal
#define   REC_NORMAL       1   // clicked element while recording is a
                               // normal element
#define   REC_FINISH       2

#define   REC_SHOW         4   // = R_SHOW
#define   REC_START        2   // = R_EDIT
#define   REC_STOPP        0   // = NOEDIT

#define   SINGLE           0   // search only one element
#define   MULTI            1   // search all elements

//   IDs for menu items in MainWindow.cpp, do NOT edit
#define   FILE_ID_NEW      101
#define   FILE_ID_OPEN     102
#define   FILE_ID_SAVE     103
#define   FILE_ID_SAVE_AS  104
#define   FILE_ID_NEWWIN   105
#define   FILE_ID_CLOSE    106
#define   FILE_ID_QUIT     107

#define   EDIT_ID_CUT      201
#define   EDIT_ID_COPY     202
#define   EDIT_ID_PASTE    203
#define   EDIT_ID_FIND     204
#define   EDIT_ID_OPT      205

#define   EDITFILE_ID_GBS  210
#define   EDITFILE_ID_RTS  211
#define   EDITFILE_ID_CON  212

#define   VIEW_ID_ROUTES   401
#define   VIEW_ID_FBMOD    402
#define   VIEW_ID_CLOCK    403
#define   VIEW_ID_KEYB     404
#define   VIEW_ID_DEBG     405
#define   VIEW_ID_EDITMODE 406

#define   DAEMON_ID_CONNECT      301
#define   DAEMON_ID_DISCONNECT   302
#define   DAEMON_ID_RESET  303
#define   DAEMON_ID_KILL   304
#define   DAEMON_ID_INFO   305

#define   LAYOUT_ID_START   501
#define   LAYOUT_ID_FHT     502
#define   LAYOUT_ID_WGT     503
#define   LAYOUT_ID_UFGT    504
#define   LAYOUT_ID_NOTROT  505
#define   LAYOUT_ID_SEND    506
#define   LAYOUT_ID_TOGGLE  507
#define   LAYOUT_ID_UNLOCKR 508
#define   LAYOUT_ID_CHSIZE  509

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
#define   SYM_PRE          "prellbock"
#define   SYM_BUE          "uebergang"
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
#define   SYM_DRE          "drehscheibe"
#define   SYM_SBN          "schiebebuehne"
#define   SYM_HS1          "haus_1"
#define   SYM_HS2          "haus_2"
#define   SYM_SHO          "schuppen_o"
#define   SYM_SHM          "schuppen_m"
#define   SYM_SHU          "schuppen_u"
#define   SYM_MDC          "motor_dc"
#define   SYM_UHR          "uhr"

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

//   QStrList IDs, do NOT edit
#define   LIST_ID_INDEX        0
#define   LIST_ID_ICON         1
#define   LIST_ID_ROTATE       2
#define   LIST_ID_INVERT       3
#define   LIST_ID_DECODER      4
#define   LIST_ID_PROTOCOL     5
#define   LIST_ID_ADDRESS_1    6
#define   LIST_ID_ADDRESS_2    7
#define   LIST_ID_CHACONN_1    8
#define   LIST_ID_CHACONN_2    9
#define   LIST_ID_DIRECTION    10
#define   LIST_ID_SUBTYPE      11
#define   LIST_ID_TEXT         12
#define   LIST_ID_ACTTIME      13
#define   LIST_ID_FBPORT       14
#define   LIST_ID_LEDOFF       15
#define   LIST_ID_DATA_2       16
#define   LIST_ID_DATA_3       17

#endif
