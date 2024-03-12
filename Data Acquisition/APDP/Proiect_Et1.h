/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: OnExit */
#define  PANEL_GRAPH_2                    2       /* control type: graph, callback function: (none) */
#define  PANEL_DECORATION_5               3       /* control type: deco, callback function: (none) */
#define  PANEL_SWITCH_PANEL_1             4       /* control type: binary, callback function: SwitchPanel */
#define  PANEL_GRAPH                      5       /* control type: graph, callback function: (none) */
#define  PANEL_ZERO                       6       /* control type: numeric, callback function: (none) */
#define  PANEL_INDEX_MAX                  7       /* control type: numeric, callback function: (none) */
#define  PANEL_INDEX_MIN                  8       /* control type: numeric, callback function: (none) */
#define  PANEL_KURTOSIS                   9       /* control type: numeric, callback function: (none) */
#define  PANEL_SKEWNESS                   10      /* control type: numeric, callback function: (none) */
#define  PANEL_Mediana                    11      /* control type: numeric, callback function: (none) */
#define  PANEL_MEDIE                      12      /* control type: numeric, callback function: (none) */
#define  PANEL_DISPERSIE                  13      /* control type: numeric, callback function: (none) */
#define  PANEL_MAX                        14      /* control type: numeric, callback function: (none) */
#define  PANEL_MIN                        15      /* control type: numeric, callback function: (none) */
#define  PANEL_DECORATION                 16      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_2               17      /* control type: deco, callback function: (none) */
#define  PANEL_NEXT_BUTTON                18      /* control type: command, callback function: Prev_Next_Second */
#define  PANEL_PREV_BUTTON                19      /* control type: command, callback function: Prev_Next_Second */
#define  PANEL_START                      20      /* control type: numeric, callback function: (none) */
#define  PANEL_STOP                       21      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG                    22      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION_4               23      /* control type: deco, callback function: (none) */
#define  PANEL_FILTRU                     24      /* control type: ring, callback function: (none) */
#define  PANEL_DIM_FEREASTRA              25      /* control type: ring, callback function: (none) */
#define  PANEL_ANEVELOPA                  26      /* control type: command, callback function: Anvelopa */
#define  PANEL_DERVATA                    27      /* control type: command, callback function: Derivare */
#define  PANEL_SAVE_BUTTON                28      /* control type: command, callback function: SaveImg */
#define  PANEL_LOAD_BUTTON                29      /* control type: command, callback function: OnLoadButtonCB */
#define  PANEL_APPLY_BUTTON               30      /* control type: command, callback function: Selectie_Filtrare */
#define  PANEL_TEXTMSG_2                  31      /* control type: textMsg, callback function: (none) */
#define  PANEL_ANVELOP                    32      /* control type: graph, callback function: (none) */
#define  PANEL_HISTO                      33      /* control type: graph, callback function: (none) */
#define  PANEL_ALPHA                      34      /* control type: numeric, callback function: (none) */

#define  PANEL_2                          2       /* callback function: OnExit */
#define  PANEL_2_SPECTRUM                 2       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH                    3       /* control type: graph, callback function: (none) */
#define  PANEL_2_DECORATION_5             4       /* control type: deco, callback function: (none) */
#define  PANEL_2_DECORATION_2             5       /* control type: deco, callback function: (none) */
#define  PANEL_2_SWITCH_PANEL_2           6       /* control type: binary, callback function: SwitchPanel */
#define  PANEL_2_IDC_SPECTRUM             7       /* control type: command, callback function: spectrum */
#define  PANEL_2_MOD                      8       /* control type: command, callback function: modify */
#define  PANEL_2_SAVE                     9       /* control type: command, callback function: Save */
#define  PANEL_2_APPLY                    10      /* control type: command, callback function: Aply */
#define  PANEL_2_LOAD_BUTTON              11      /* control type: command, callback function: Load */
#define  PANEL_2_IDC_POWER_PEAK           12      /* control type: numeric, callback function: (none) */
#define  PANEL_2_IDC_FREQ_PEAK            13      /* control type: numeric, callback function: (none) */
#define  PANEL_2_IDC_FILTRU               14      /* control type: ring, callback function: (none) */
#define  PANEL_2_IDC_FEREASTRA            15      /* control type: ring, callback function: (none) */
#define  PANEL_2_IDC_SECUNDA              16      /* control type: ring, callback function: (none) */
#define  PANEL_2_IDC_NR_PUNCTE            17      /* control type: ring, callback function: (none) */
#define  PANEL_2_SPECTRUM_FILTRED_2       18      /* control type: graph, callback function: (none) */
#define  PANEL_2_SPECTRUM_FILTRED         19      /* control type: graph, callback function: (none) */
#define  PANEL_2_SPECTRUM_WINDOW          20      /* control type: graph, callback function: (none) */
#define  PANEL_2_SPECTRUM_SECOND          21      /* control type: graph, callback function: (none) */
#define  PANEL_2_WINDOW_2                 22      /* control type: graph, callback function: (none) */
#define  PANEL_2_WINDOW                   23      /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_FILTRED_2          24      /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_FILTRED            25      /* control type: graph, callback function: (none) */
#define  PANEL_2_DATA_SECOND              26      /* control type: graph, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK Anvelopa(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Aply(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Derivare(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Load(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK modify(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnExit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoadButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Prev_Next_Second(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Save(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveImg(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Selectie_Filtrare(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK spectrum(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SwitchPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif