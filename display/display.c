/*
 * display.c
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 

#include "ibus.h"
#include "commands.h"

#define DISPLAY_MIDL                (1 << 0)
#define DISPLAY_MIDR                (1 << 1)
#define DISPLAY_IKE                 (1 << 2)
#define DISPLAY_NAVI_TITELFELD      (1 << 3)
#define DISPLAY_NAVI_INDEXFELD      (1 << 4)

// ****************************** MID ******************************
static AVR_U8 cmdMidLDisp[]     = {0x68, 0x10, 0xC0, 0x23, 0x62, 0x30, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0xCB, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F};
static AVR_U8 cmdMidRDisp[]     = {0x80, 0x19, 0xC0, 0x23, 0x40, 0x20, 0x20, 0x20, 0x42, 0x4D, 0x57, 0x20, 0x43, 0x6C, 0x75, 0x62, 0x20, 0x42, 0x75, 0x6C, 0x67, 0x61, 0x72, 0x69, 0x61, 0x20};

extern AVR_U8 cmdSetToDefaultMidR[];
extern AVR_U8 cmdMidRefresh[];
extern AVR_U8 cmdMidLClear[];
extern AVR_U8 cmdMidRClear[];
extern AVR_U8 cmdMidKeysClear[];
extern AVR_U8 cmdMidKeysDisp[];
extern AVR_U8 cmdMidKeyDisp[];

// ****************************** NAVI *****************************
extern AVR_U8 cmdNaviClear[];
extern AVR_U8 cmdDispNaviIndex[];

// ****************************** IKE ******************************
extern AVR_U8 cmdDispIke [];
extern AVR_U8 cmdDispIkeP[];
extern AVR_U8 cmdIkeClearPern[];
extern AVR_U8 cmdIkeClearNorm[];

// *************************** Broadcast ***************************
extern AVR_U8 cmdGoToRadio[];

static AVR_U8 DisplayStatus = 0;

// *************************** Functions ***************************
void GoToRadio(void)
{
    DisplayStatus &= ~DISPLAY_MIDL;
    DisplayStatus &= ~DISPLAY_MIDR;
    ibus_send_msg(cmdGoToRadio);
}

void SetToDefaultMidR(void)
{
    DisplayStatus &= ~DISPLAY_MIDR;
    ibus_send_msg(cmdSetToDefaultMidR);
}

void MidRefresh(void)
{
    DisplayStatus &= ~DISPLAY_MIDL;
    DisplayStatus &= ~DISPLAY_MIDR;
    ibus_send_msg(cmdMidRefresh);
}

void MidClearL(void)
{
    DisplayStatus &= ~DISPLAY_MIDL;
    DisplayStatus &= ~DISPLAY_MIDR;
    ibus_send_msg(cmdMidLClear);
}

void SetToDefaultMid(void)
{
    if (DisplayStatus & DISPLAY_MIDL || DisplayStatus & DISPLAY_MIDR ) {
        DisplayStatus &= ~DISPLAY_MIDL;
        DisplayStatus &= ~DISPLAY_MIDR;
        ibus_send_msg(cmdMidRClear);
        ibus_send_msg(cmdMidLClear);
        ibus_send_msg(cmdMidRefresh);
    }
}

void SetToDefaultIke(void)
{
    if (DisplayStatus & DISPLAY_IKE) {
        DisplayStatus &= ~DISPLAY_IKE;
        ibus_send_msg(cmdIkeClearNorm);
        ibus_send_msg(cmdIkeClearPern);
    }
}

void DispSetToDefault(void)
{
    SetToDefaultIke();
    SetToDefaultMid();
}

// data must be MID_L_SIZEb byte minimum
void DispSendToMidL(AVR_U8 *data)
{
    AVR_S16 i;

    for(i = 0; i < MID_L_SIZE; i++) {
        cmdMidLDisp[i + 6] = data[i];
    }

    ibus_send_msg(cmdMidLDisp);

    DisplayStatus |= DISPLAY_MIDL;
}

// data must be MID_R_SIZE bytes minimum
void DispSendToMidR(AVR_U8 *data)
{
    AVR_S16 i;

    for(i = 0; i < MID_R_SIZE; i++) {
        cmdMidRDisp[i + 6] = data[i];
    }

    ibus_send_msg(cmdMidRDisp);

    DisplayStatus |= DISPLAY_MIDR;
}

void DispMidReload(void)
{
    if (DisplayStatus & DISPLAY_MIDL) {
        ibus_send_msg(cmdMidLDisp);
    }

    if (DisplayStatus & DISPLAY_MIDR) {
        ibus_send_msg(cmdMidRDisp);
    }
}

// data must be MID_R_SIZE bytes minimum
void DispSendToIke(AVR_U8 *data)
{
    AVR_S16 i;

    for(i = 0; i < MID_R_SIZE; i++) {
        cmdDispIke[i + 6] = data[i];
    }

    ibus_send_msg(cmdDispIke);

    // this is temporal massage that disappear automatically
    // but need to clear for PDC
    DisplayStatus |= DISPLAY_IKE;
}

// data must be MID_R_SIZE bytes minimum
void DispSendToIkePersistent(AVR_U8 *data)
{
    AVR_S8 i;

    for(i = 0; i < MID_R_SIZE; i++) {
        cmdDispIkeP[i + 6] = data[i];
    }

    ibus_send_msg(cmdDispIkeP);

    DisplayStatus |= DISPLAY_IKE;
}

void DispClearMidKeys(void)
{
    ibus_send_msg(cmdMidKeysClear);
}

void DispSendToMidKeys(AVR_U8 data[MID_BUTTONS_SIZE], AVR_U8 disp) // disp: 1 - 6
{
    AVR_S8 i;

    if(disp > 0 && disp < 7) {
        cmdMidKeysDisp[6] = (disp - 1) * 2;

        for(i = 0; i < MID_BUTTONS_SIZE; i++) {
            cmdMidKeysDisp[i + 8] = data[i];
        }

        ibus_send_msg(cmdMidKeysDisp);
    }
}

void DispSendToMidKey(AVR_U8 data[MID_BUTTONS_SIZE / 2], AVR_U8 key) // key: 1 - 12
{
    AVR_S8 i;
    // todo: bug: write in next field

    if(key > 0 && key < 13) {
        cmdMidKeyDisp[6] = key - 1;

        for(i = 0; i < (MID_BUTTONS_SIZE / 2); i++) {
            cmdMidKeyDisp[i + 8] = data[i];
        }

        ibus_send_msg(cmdMidKeyDisp);
    }
}

/***************************************************** NAVI ***********************************************************/

void SetToDefaultNavi(void)
{
    if (DisplayStatus & DISPLAY_NAVI_TITELFELD) {
        DisplayStatus &= ~DISPLAY_NAVI_TITELFELD;
        ibus_send_msg(cmdNaviClear);
    }
}

// data must be MID_R_SIZE bytes minimum
void DispSendToNavi(AVR_U8 *data)
{
    AVR_S16 i;

    for (i = 0; i < MID_R_SIZE; i++) {
        cmdDispNaviMk2Area0[i + 6] = data[i];
    }

    ibus_send_msg(cmdDispNaviMk2Area0);

    DisplayStatus |= DISPLAY_NAVI_TITELFELD;
}

void DispSendToNaviIndex(AVR_U8 *data, AVR_U8 index)
{
    AVR_S16 i;

    cmdDispNaviIndex[6] = 0x40 + index;

    for (i = 0; i < MID_R_SIZE; i++) {
        cmdDispNaviIndex[i + 7] = data[i];
    }

    ibus_send_msg(cmdDispNaviIndex);

    DisplayStatus |= DISPLAY_NAVI_INDEXFELD;
}
