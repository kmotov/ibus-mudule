/*
 * menu.c
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */

#include "ibus.h"
#include "display.h"
#include "util/delay.h"
#include "tools.h"
#include <string.h>
#include <avr/pgmspace.h>

extern IBusCtxT ctx;

AVR_U8 dbg_mark_end;

#define MENU_NUM (sizeof(menu) / sizeof(menu_t))


typedef struct {
    AVR_U8   name[9];
    AVR_BOOL bSuported;
    const AVR_U8 ** const * ValueName;
    AVR_U8   num;
    AVR_U8   *pbValue;
    AVR_U8   *pMax;
} menu_t;

/* MID */
extern AVR_U8 cmdMidPressTime1[];
extern AVR_U8 cmdMidPressTime2[];

extern AVR_U8 Msg11[];
extern AVR_U8 Msg20[];
extern AVR_U8 MsgMidKey[];

/* Put all menu strings in program memory (flash memory) */
const AVR_U8 stringDisable [MID_R_STR_SIZE] PROGMEM = {"Disable             "};
const AVR_U8 stringEnable  [MID_R_STR_SIZE] PROGMEM = {"Enable              "};

const AVR_U8 stringPdcMid  [MID_R_STR_SIZE] PROGMEM = {"MID                 "};
const AVR_U8 stringPdcIke  [MID_R_STR_SIZE] PROGMEM = {"IKE                 "};
                           
const AVR_U8 stringHello1  [MID_R_STR_SIZE] PROGMEM = {"  BMW Club Bulgaria "};
const AVR_U8 stringHello2  [MID_R_STR_SIZE] PROGMEM = {"    Turn Off DSC    "};
const AVR_U8 stringHello3  [MID_R_STR_SIZE] PROGMEM = {"   Special Edition  "};
const AVR_U8 stringHello4  [MID_R_STR_SIZE] PROGMEM = {"   Limited Edition  "};
const AVR_U8 stringHello5  [MID_R_STR_SIZE] PROGMEM = {"   Turn Lights On   "};
const AVR_U8 stringHello6  [MID_R_STR_SIZE] PROGMEM = {"      BMW 530d      "};
const AVR_U8 stringHello7  [MID_R_STR_SIZE] PROGMEM = {"  DSC OFF / FUN ON  "};

const AVR_U8 stringLock1   [MID_R_STR_SIZE] PROGMEM = {"10 km/h             "};
const AVR_U8 stringLock2   [MID_R_STR_SIZE] PROGMEM = {"20 km/h             "};
const AVR_U8 stringLock3   [MID_R_STR_SIZE] PROGMEM = {"30 km/h             "};
const AVR_U8 stringLock4   [MID_R_STR_SIZE] PROGMEM = {"Auto Unlock         "};

const AVR_U8 stringWelcome1[MID_R_STR_SIZE] PROGMEM = {"Sidelight / Angles  "};
const AVR_U8 stringWelcome2[MID_R_STR_SIZE] PROGMEM = {"Low Beam            "};
const AVR_U8 stringWelcome3[MID_R_STR_SIZE] PROGMEM = {"Fog Light           "};

const AVR_U8 stringDebug1  [MID_R_STR_SIZE] PROGMEM = {"MID                 "};
const AVR_U8 stringDebug2  [MID_R_STR_SIZE] PROGMEM = {"IKE                 "};
const AVR_U8 stringDebug3  [MID_R_STR_SIZE] PROGMEM = {"Full                "};

const AVR_U8* const OnOffValue[] PROGMEM = {
    stringDisable,
    stringEnable
};

const AVR_U8* const PdcValue[] PROGMEM = {
    stringDisable,
    stringPdcMid,
    stringPdcIke
};

const AVR_U8* const HelloValue[] PROGMEM = {
    stringDisable,
    stringHello1,
    stringHello2,
    stringHello3,
    stringHello4,
    stringHello5,
    stringHello6,
    stringHello7
};

const AVR_U8* const LockValue[] PROGMEM = {
    stringDisable,
    stringLock1,
    stringLock2,
    stringLock3,
    stringLock4
};

const AVR_U8* const WelcomeValue[] PROGMEM = {
    stringDisable,
    stringWelcome1,
    stringWelcome2,
    stringWelcome3
};

const AVR_U8* const DebugValue[] PROGMEM = {
    stringDisable,
    stringDebug1,
    stringDebug2,
    stringDebug3
};

menu_t menu[] = {
{"PDC     ", TRUE , (const AVR_U8 ** const *)&PdcValue,     sizeof(PdcValue    ) / sizeof(char *), &(ctx.Config.nPdc)       , &(ctx.Config.nPdcMax)       },
{"IKE Msg ", TRUE , (const AVR_U8 ** const *)&HelloValue,   sizeof(HelloValue  ) / sizeof(char *), &(ctx.Config.nHello)     , &(ctx.Config.nHelloMax)     },
{"Lock    ", TRUE , (const AVR_U8 ** const *)&LockValue,    sizeof(LockValue   ) / sizeof(char *), &(ctx.Config.nAutoLock)  , &(ctx.Config.nAutoLockMax)  },
{"Mirrors ", TRUE , (const AVR_U8 ** const *)&OnOffValue,   sizeof(OnOffValue  ) / sizeof(char *), &(ctx.Config.nMirrorFold), &(ctx.Config.nMirrorFoldMax)},
{"Welcome ", TRUE , (const AVR_U8 ** const *)&WelcomeValue, sizeof(WelcomeValue) / sizeof(char *), &(ctx.Config.nWelcome)   , &(ctx.Config.nWelcomeMax)   },
#ifdef IBUS_DEBUG                        
//{"AudioAUX", TRUE , (const AVR_U8 ** const *)&OnOffValue,   sizeof(OnOffValue  ) / sizeof(char *), &(ctx.Config.nAudioAux)  , &(ctx.Config.nAudioAuxMax)  },
//{"FollowMe", TRUE , (const AVR_U8 ** const *)&OnOffValue,   sizeof(OnOffValue )  / sizeof(char *), &(ctx.Config.nFollowMe)  , &(ctx.Config.nFollowMeMax)  },
{"PDC Raw ", TRUE , (const AVR_U8 ** const *)&OnOffValue,   sizeof(OnOffValue )  / sizeof(char *), &(ctx.Config.nPdcRaw)    , &(ctx.Config.nPdcRawMax)    },
//{"Debug   ", TRUE , (const AVR_U8 ** const *)&DebugValue,   sizeof(DebugValue )  / sizeof(char *), &(ctx.Config.nDebug)     , &(ctx.Config.nDebugMax)     },
#endif // IBUS_DEBUG
};

void MenuDisableUnsuportedFeatures(void)
{
    AVR_U8 i;

    /* If some feature is mark as unsupported set it to disable */
    for (i = 0; i < MENU_NUM; i++) {
        if (menu[i].bSuported == FALSE) {
            *(menu[i].pbValue) = 0;
        }

        /* hack for good */
        *menu[i].pMax = menu[i].num;
    }
}

void MenuSet(AVR_U8 nMenuId, AVR_U8 move)
{
    AVR_S8 val = *(menu[nMenuId].pbValue);

    if (menu[nMenuId].bSuported == TRUE) {
        val += move;
        if (val >= menu[nMenuId].num)
            val = 0;
        if (val < 0)
            val = menu[nMenuId].num - 1;

        *(menu[nMenuId].pbValue) = val;

        /* Pointer of PROGMEM array is special value. Because of that
         * we need to store it in menu holder. After that we just add offset. */
        strcpy_P((char *)Msg20, (PGM_P)pgm_read_word(menu[nMenuId].ValueName + val));

        DispSendToMidR((AVR_U8 *)Msg20);
    } else {
        memcpy(Msg20, "Not Support         ", 20);
        DispSendToMidR(Msg20);
    }
}

void MenuLoad(AVR_U8 nMenuId)
{
    AVR_U8 i;
    Msg11[0] = nMenuId + 1 + 48;
    Msg11[1] = '.';
    Msg11[2] = ' ';

    for(i = 0; i < 8; i++) {
        Msg11[i + 3] = menu[nMenuId].name[i];
    }

    DispSendToMidL(Msg11);
    MenuSet(nMenuId, 0);
}

void SayHello(AVR_U8 num)
{
    if (num >= (sizeof(HelloValue) / sizeof(char *)))
        return;
    
    strcpy_P((char *)Msg20, (PGM_P)pgm_read_word(&(HelloValue[num])));

    DispSendToIke(Msg20);
}

void MenuMid(void)
{
    AVR_U8 *cmd;
    AVR_U8 cnt = 0;
    AVR_S8 MenuId = 0;
    AVR_BOOL bExit; 

    _delay_ms(50); // fix missing mid keys

    DispClearMidKeys();
    MsgMidKey[0] = 0x5E;
    MsgMidKey[1] = 0x20;
    MsgMidKey[2] = 0x4D;
    MsgMidKey[3] = 0x45;
    MsgMidKey[4] = 0x4E;
    MsgMidKey[5] = 0x55;
    MsgMidKey[6] = 0x20;
    MsgMidKey[7] = 0xC1;
    DispSendToMidKeys(MsgMidKey, 1);
    MsgMidKey[0] = 0x5E;
    MsgMidKey[1] = 0x20;
    MsgMidKey[2] = 0x53;
    MsgMidKey[3] = 0x45;
    MsgMidKey[4] = 0x54;
    MsgMidKey[5] = 0x20;
    MsgMidKey[6] = 0x20;
    MsgMidKey[7] = 0xC1;
    DispSendToMidKeys(MsgMidKey, 3);
    memcpy(MsgMidKey, "    Exit", MID_BUTTONS_SIZE);
    DispSendToMidKeys(MsgMidKey, 6);

    MenuLoad(MenuId);

    bExit = FALSE;
    do {
        cmd = ibus_get_msg();
        switch (cmd[3]) {
        case IBUS_CMD_MID_BUTTON_PRESS:
            switch (cmd[6]) {
            case MENU_KEY_UP:
                MenuId++;
                if(MenuId == MENU_NUM) {
                    MenuId = 0;
                }
                MenuLoad(MenuId);
                break;

            // test todo remove
            case MID_KEY_3:
                //reset();
                break;

            case MENU_KEY_DOWN:
                MenuId--;
                if(MenuId < 0) {
                    MenuId = MENU_NUM - 1;
                }
                MenuLoad(MenuId);
                break;

            case MENU_KEY_SET_PRV:
                MenuSet(MenuId, -1);
                break;

            case MENU_KEY_SET_NEXT:
                MenuSet(MenuId, +1);
                break;

            case MENU_KEY_VER1:
            case MENU_KEY_VER2:
                if (++cnt % 2) {
                    memcpy(MsgMidKey, IBUS_MODULE_VERSION, MID_BUTTONS_SIZE);
                } else {
                    memcpy(MsgMidKey, "        ", MID_BUTTONS_SIZE);
                }
                DispSendToMidKeys(MsgMidKey, 5);
                break;

            case MID_KEY_12:
                bExit = TRUE;
                if (ctx.Status.bRadioOn == 0) {
                    MidClearL();
                } else {
                    MidRefresh();
                }
                break;

            default:
                break;
            }
            break;

        case IBUS_CMD_SEND_TO_DISP:  // mid preemted
            MenuLoad(MenuId);
            break;

        case IBUS_CMD_SEND_TO_MID_KEYS: // mid keys preemted
            DispClearMidKeys();
            MsgMidKey[0] = 0x5E;
            MsgMidKey[1] = 0x20;
            MsgMidKey[2] = 0x4D;
            MsgMidKey[3] = 0x45;
            MsgMidKey[4] = 0x4E;
            MsgMidKey[5] = 0x55;
            MsgMidKey[6] = 0x20;
            MsgMidKey[7] = 0xC1;
            DispSendToMidKeys(MsgMidKey, 1);
            MsgMidKey[0] = 0x5E;
            MsgMidKey[1] = 0x20;
            MsgMidKey[2] = 0x53;
            MsgMidKey[3] = 0x45;
            MsgMidKey[4] = 0x54;
            MsgMidKey[5] = 0x20;
            MsgMidKey[6] = 0x20;
            MsgMidKey[7] = 0xC1;
            DispSendToMidKeys(MsgMidKey, 3);
            memcpy(MsgMidKey, "    Exit", MID_BUTTONS_SIZE);
            DispSendToMidKeys(MsgMidKey, 6);
            break;

        case IBUS_CMD_MID_SBUTTON_PRESS:
            if (cmd[4] == MID_KEY_POWER)
                break;
            
            bExit = TRUE;
            if (ctx.Status.bRadioOn == 0) {
                MidClearL();
            } else {
                MidRefresh();
            }
            if (cmd[4] == MID_KEY_TIME) {
                ibus_send_msg(cmdMidPressTime1);
            }
            if (cmd[4] == MID_KEY_AUDIO) {
                ibus_send_msg(cmdMidPressTime2);
            }
            if (cmd[4] == MID_KEY_BC) {
            }
            break;

        case IBUS_CMD_IGN_STAT:
            if ((ctx.Status.bIgnitionRad == 0) && (cmd[4] & 1)) {
                if (ctx.Config.nHello)
                    SayHello(ctx.Config.nHello);
                if (ctx.Config.nMirrorFold)
                    mirrorUnfold();
            }
            ctx.Status.bIgnitionRad = cmd[4] & 1;
            ctx.Status.bIgnitionOn = (cmd[4] & 2) >> 1;

            if (!ctx.Status.bIgnitionRad) {
                ctx.Status.bRadioOn = FALSE;
                MidClearL();
                bExit = TRUE;
            }
            break;
        }
    } while(!bExit);
}

/************************************************ Navi *************************************************/

void MenuNavi(void)
{
    AVR_U8 i, j;

    for (i = 0; i < MENU_NUM; i++) {
        Msg20[0] = i + 1 + 48;
        Msg20[1] = '.';
        Msg20[2] = ' ';

        for(j = 0; j < 8; j++) {
            Msg20[j + 3] = menu[i].name[j];
        }
        DispSendToNaviIndex(Msg20, i);
    }
}
