/*
 * tools.c
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */
 #include "ibus.h"
 #include "display.h"
 #include "util/delay.h"
 #include <string.h>


extern IBusCtxT ctx;

static AVR_S16 old_R_RR = 0;
static AVR_S16 old_R_LL = 0;

//static AVR_U16 old_raw_R_LL = 255;
//static AVR_U16 old_raw_R_ML = 255;
//static AVR_U16 old_raw_R_MR = 255;
//static AVR_U16 old_raw_R_RR = 255;

static AVR_S16 pdcEnabled = 0;

#ifdef IBUS_DEBUG
    extern AVR_U8 cmdMidLedOff[];
    extern AVR_U8 cmdMidLedOn[];
    extern AVR_U8 cmdMidLedBlinc[];
#endif

extern AVR_U8 cmdMirrorLFold[];
extern AVR_U8 cmdMirrorRFold[];
extern AVR_U8 cmdMirrorLUnfold[];
extern AVR_U8 cmdMirrorRUnfold[];
extern AVR_U8 cmdTogleLockUnlock[];
extern AVR_U8 cmdMirrorLFoldCancel[];
extern AVR_U8 cmdMirrorRFoldCancel[];
extern AVR_U8 cmdMirrorLUnfoldCancel[];
extern AVR_U8 cmdMirrorRUnfoldCancel[];

extern AVR_U8 cmdCdStatus[];

extern AVR_U8 CmdFogs[];
extern AVR_U8 CmdLBeam[];
extern AVR_U8 CmdSideLight[];

extern AVR_U8 Msg11[];
extern AVR_U8 Msg20[];
extern AVR_U8 MsgMidKey[];

void CdEmulatorDisplay(void);

void PrintRawData(AVR_S16 L, AVR_S16 ML, AVR_S16 MR, AVR_S16 R)
{
    AVR_S8 shift = 7;

    memset(Msg20, 0x20, 20);

    if (L > 60)
    {
        Msg20[0 + shift] = 0x2A;
        Msg20[1 + shift] = 0x2A;
    }
    else
    {
        Msg20[0 + shift] = (AVR_U8)(L / 10 + 48);
        Msg20[1 + shift] = (AVR_U8)(L % 10 + 48);
    }

    Msg20[2 + shift] = 0x20;

    if (ML > 199)
    {
        Msg20[3 + shift] = 0x2A;
        Msg20[4 + shift] = 0x2A;
        Msg20[5 + shift] = 0x2A;
    }
    else
    {
        Msg20[3 + shift] = (AVR_U8)(ML / 100 + 48);
        Msg20[4 + shift] = (AVR_U8)(((ML / 10) % 10) + 48);
        Msg20[5 + shift] = (AVR_U8)(ML % 10 + 48);
    }

    Msg20[6 + shift] = 0x20;

    if (MR > 199)
    {
        Msg20[7 + shift] = 0x2A;
        Msg20[8 + shift] = 0x2A;
        Msg20[9 + shift] = 0x2A;
    }
    else
    {
        Msg20[7 + shift] = (AVR_U8)(MR / 100 + 48);
        Msg20[8 + shift] = (AVR_U8)(((MR / 10) % 10) + 48);
        Msg20[9 + shift] = (AVR_U8)(MR % 10 + 48);
    }

    Msg20[10 + shift] = 0x20;

    if (R > 60)
    {
        Msg20[11 + shift] = 0x2A;
        Msg20[12 + shift] = 0x2A;
    }
    else
    {
        Msg20[11 + shift] = (AVR_U8)(R / 10 + 48);
        Msg20[12 + shift] = (AVR_U8)(R % 10 + 48);
    }

    DispSendToMidR(Msg20);
}

static AVR_U8 InterpPdcL[3] = {255, 255, 255};
static AVR_U8 InterpPdcR[3] = {255, 255, 255};
static AVR_S8 InterpInter = 0;
static AVR_S8 InterpValidData = 0;

/* InterpolatePdc
 * 
 * cmd - array containing last 3 PDC parsed output values
 *
 * return: middle value of array
 */
AVR_U8 InterpolatePdc(AVR_U8 *pData)
{
    AVR_U8 found;

    found = 1; // result
    if( pData[0] > pData[1] ) {
        found = 0; // result
    }
    if( pData[found] > pData[2] ) {
        // in this case 'found' is the biggest,
        // so we need to compare others
        if (found == 1) { // inverse
            found = 0;
        } else {
            found = 1;
        }
        if( pData[2] > pData[found]) {
            found = 2; // result
        }
    }

    return pData[found];
}

void PdcOutputMulti(AVR_S16 L, AVR_S16 R)
{
    // Prepare new scale for display
    Msg11[0]  = 0x5F;
    Msg11[1]  = 0x5F;
    Msg11[2]  = 0x5F;
    Msg11[3]  = 0x5F;
    Msg11[4]  = 0x5F;
    Msg11[5]  = 0xCB;
    Msg11[6]  = 0x5F;
    Msg11[7]  = 0x5F;
    Msg11[8]  = 0x5F;
    Msg11[9]  = 0x5F;
    Msg11[10] = 0x5F;

    switch (L) {
        case 0: Msg11[0] = 0xB2;
        case 1: Msg11[1] = 0xB3;
        case 2: Msg11[2] = 0xB4;
        case 3: Msg11[3] = 0xB5;
        case 4: Msg11[4] = 0xB6;
    }

    switch (R) {
        case 0: Msg11[10] = 0xB2;
        case 1: Msg11[9]  = 0xB3;
        case 2: Msg11[8]  = 0xB4;
        case 3: Msg11[7]  = 0xB5;
        case 4: Msg11[6]  = 0xB6;
    }

    if (ctx.Config.nPdc == 1) { // mid/navi
        if (ctx.Status.eMulti == MULTIMEDIA_MID) {
            DispSendToMidL(Msg11);
        } else if (ctx.Status.eMulti == MULTIMEDIA_NAVI) {
            DispSendToNavi(Msg11);
        }
    } else if (ctx.Config.nPdc == 2) { // ike
        memset(Msg20, 0x20, MID_R_SIZE);
        memcpy(&Msg20[5], Msg11, MID_L_SIZE);
        DispSendToIke(Msg20);
    }
}

/* PdcParseAndPrint
 * 
 * cmd - PDC modul output data
 *
 * return: PDC state (enable/disable)
 */
AVR_S16 PdcParseAndPrint(AVR_U8 *cmd)
{
    AVR_S16 R_LL, R_ML, R_MR, R_RR; // rear
    //AVR_S16 F_LL, F_ML, F_MR, F_RR; // front // todo implement
    //AVR_S8 HistDept;

    // check if PDC is still enable
    if( cmd[13] == 0xEF || cmd[13] == 0xC7 ) {
        pdcEnabled = 1;

        if (ctx.Config.nPdcRaw) {
            PrintRawData(cmd[5], cmd[7], cmd[8], cmd[6]);
        }

        // Calculate scale [0-5]
        // Middle and outside sensor have different ranges
        //
        // Range outside: 25 - 55  - > 0 - 32 (8)
        // Range middle:   25 - 145 - > 0 - 120 (30)
        //
        // Intervals:
        // 0 - under minimum
        // 1 - 1/4
        // 2 - 2/4
        // 3 - 3/4
        // 4 - 4/4
        // 5 - over maximum

//todo: HISTERESIS

        // Calculate intervals
        //      raw       clip                    round up
        R_LL = (cmd[5] - PDC_OUT_SEN_RANGE_MIN + PDC_OUT_SEN_STEP); // 20 - 9  + 1;
        R_ML = (cmd[7] - PDC_INT_SEN_RANGE_MIN + PDC_INT_SEN_STEP); // 20 - 31 + 1;
        R_MR = (cmd[8] - PDC_INT_SEN_RANGE_MIN + PDC_INT_SEN_STEP); // 20 - 31 + 1;
        R_RR = (cmd[6] - PDC_OUT_SEN_RANGE_MIN + PDC_OUT_SEN_STEP); // 20 - 9  + 1;

        if (R_LL < 0) R_LL = 0;
        if (R_ML < 0) R_ML = 0;
        if (R_MR < 0) R_MR = 0;
        if (R_RR < 0) R_RR = 0;

        R_LL /= PDC_OUT_SEN_STEP;
        R_ML /= PDC_INT_SEN_STEP;
        R_MR /= PDC_INT_SEN_STEP;
        R_RR /= PDC_OUT_SEN_STEP;

        if (R_ML < R_LL) R_LL = R_ML;
        if (R_MR < R_RR) R_RR = R_MR;

        // interpolate PDC data
        if( InterpValidData > 1) { // 2 or 3
            // Add new data to next position
            InterpPdcR[InterpInter] = R_RR;
            InterpPdcL[InterpInter] = R_LL;

            // get middle value
            R_RR = InterpolatePdc(InterpPdcR);
            R_LL = InterpolatePdc(InterpPdcL);

            InterpInter++;
            if( InterpInter == 3) {
                InterpInter = 0;
            }
        } else { // Collect data
            InterpPdcR[InterpInter] = R_RR;
            InterpPdcL[InterpInter] = R_LL;
            InterpValidData++;
            InterpInter++;
        }

        // if no update return only for mid/navi not for ike
        if ((R_RR == old_R_RR) && (R_LL == old_R_LL) && (ctx.Config.nPdc == 1)) {
            return 1; // return PDC is still enabled
        }

        old_R_LL = R_LL;
        old_R_RR = R_RR;

        PdcOutputMulti(R_LL, R_RR);

        // front sensors todo
        //F_LL = cmd[ 9];
        //F_ML = cmd[11];
        //F_MR = cmd[12];
        //F_RR = cmd[10];

    } else { // if (status == 0xC6 )
        if (pdcEnabled) {
            pdcEnabled      = 0;
            old_R_RR        = -1;
            old_R_LL        = -1;
            InterpValidData = 0;
            InterpInter     = 0;
            if (ctx.Config.nPdc == 1) { // mid/navi
                if (ctx.Status.eMulti == MULTIMEDIA_MID) {
                    if (ctx.Status.bCdEmulatorOn) {
                        CdEmulatorDisplay();
                    } else {
                        SetToDefaultMid();
                    }
                } else if (ctx.Status.eMulti == MULTIMEDIA_NAVI) {
                    if (ctx.Status.bCdEmulatorOn) {
                        //memcpy(Msg11, " Aux       ", MID_L_SIZE);
                        //DispSendToMidL(Msg11);
                    } else {
                        SetToDefaultNavi();
                    }
                }
            } else if (ctx.Config.nPdc == 2) { // ike
                SetToDefaultIke();
            }
        }
    }

    return pdcEnabled;
}

#ifdef IBUS_DEBUG
void midLedOff(void) {
    ibus_send_msg(cmdMidLedOff);
}

void midLedOn(void) {
    ibus_send_msg(cmdMidLedOn);
}

void midLedBlinc(void) {
    ibus_send_msg(cmdMidLedBlinc);
}
#else
void midLedOff(void) {
}

void midLedOn(void) {
}

void midLedBlinc(void) {
}
#endif // IBUS_DEBUG

void ToggleLockUnlock(void)
{
    ibus_send_msg(cmdTogleLockUnlock);
}

void getHexAncci(AVR_U8 in, AVR_U8 *out)
{
    AVR_U8 hi = in >> 4;
    AVR_U8 lo = in & 0xf;

    switch(hi) {
        case 0:  out[0] = '0'; break;
        case 1:  out[0] = '1'; break;
        case 2:  out[0] = '2'; break;
        case 3:  out[0] = '3'; break;
        case 4:  out[0] = '4'; break;
        case 5:  out[0] = '5'; break;
        case 6:  out[0] = '6'; break;
        case 7:  out[0] = '7'; break;
        case 8:  out[0] = '8'; break;
        case 9:  out[0] = '9'; break;
        case 10: out[0] = 'A'; break;
        case 11: out[0] = 'B'; break;
        case 12: out[0] = 'C'; break;
        case 13: out[0] = 'D'; break;
        case 14: out[0] = 'E'; break;
        case 15: out[0] = 'F'; break;
    }

    switch(lo) {
        case 0:  out[1] = '0'; break;
        case 1:  out[1] = '1'; break;
        case 2:  out[1] = '2'; break;
        case 3:  out[1] = '3'; break;
        case 4:  out[1] = '4'; break;
        case 5:  out[1] = '5'; break;
        case 6:  out[1] = '6'; break;
        case 7:  out[1] = '7'; break;
        case 8:  out[1] = '8'; break;
        case 9:  out[1] = '9'; break;
        case 10: out[1] = 'A'; break;
        case 11: out[1] = 'B'; break;
        case 12: out[1] = 'C'; break;
        case 13: out[1] = 'D'; break;
        case 14: out[1] = 'E'; break;
        case 15: out[1] = 'F'; break;
    }
}

void printCmdIke(AVR_U8 *cmd) 
{
    AVR_U8 net[2];
    AVR_U8 i, size;

    size = cmd[1] + 2;
    if (size > 10)
        size = 10;

    Msg20[ 0] = 0x20;
    Msg20[ 1] = 0x20;
    Msg20[ 2] = 0x20;
    Msg20[ 3] = 0x20;
    Msg20[ 4] = 0x20;
    Msg20[ 5] = 0x20;
    Msg20[ 6] = 0x20;
    Msg20[ 7] = 0x20;
    Msg20[ 8] = 0x20;
    Msg20[ 9] = 0x20;
    Msg20[10] = 0x20;
    Msg20[11] = 0x20;
    Msg20[12] = 0x20;
    Msg20[13] = 0x20;
    Msg20[14] = 0x20;
    Msg20[15] = 0x20;
    Msg20[16] = 0x20;
    Msg20[17] = 0x20;
    Msg20[18] = 0x20;
    Msg20[19] = 0x20;

    for (i = 0; i < size; i++) {
        getHexAncci(cmd[i], net);
        Msg20[2 * i] = net[0];
        Msg20[2 * i + 1] = net[1];
    }

    DispSendToIke(Msg20);
}

void mirrorUnfoldEnd(void)
{
    if (ctx.Status.eMirrorFold == MIRROR_UNFOLDING) {
        ibus_send_msg(cmdMirrorLUnfoldCancel);
        ibus_send_msg(cmdMirrorRUnfoldCancel);
        ctx.Status.eMirrorFold = MIRROR_UNFOLD;
        midLedOff();
    }
}

void mirrorFold(void)
{
    mirrorUnfoldEnd();

    if (ctx.Status.eMirrorFold == MIRROR_UNFOLD) {
        midLedOn();
        ibus_send_msg(cmdMirrorLFold);
        ibus_send_msg(cmdMirrorRFold);
        ctx.Status.eMirrorFold = MIRROR_FOLDING;
        ctx.Status.nMirrorCnt = 6;
    }
}

void mirrorFoldEnd(void)
{
    if (ctx.Status.eMirrorFold == MIRROR_FOLDING) {
        ibus_send_msg(cmdMirrorLFoldCancel);
        ibus_send_msg(cmdMirrorRFoldCancel);
        ctx.Status.eMirrorFold = MIRROR_FOLD;
        midLedOff();
    }
}

void mirrorUnfold(void)
{
    mirrorFoldEnd();

    if (ctx.Status.eMirrorFold == MIRROR_FOLD) {
        midLedOn();
        ibus_send_msg(cmdMirrorLUnfold);
        ibus_send_msg(cmdMirrorRUnfold);
        ctx.Status.eMirrorFold = MIRROR_UNFOLDING;
        ctx.Status.nMirrorCnt = 6;
    }
}

void WellcomeLights(void)
{
    if (ctx.Config.nWelcome &&
        ctx.Status.bIgnitionRad == FALSE) {
        if (ctx.Config.nWelcome == 1) { // side light
            ibus_send_msg(CmdSideLight);
        } else if (ctx.Config.nWelcome == 2) { // low beam
            ibus_send_msg(CmdLBeam);
        } else if (ctx.Config.nWelcome == 3) { // fog light
            ibus_send_msg(CmdFogs);
        }
    }

}

void CdEmulatorDisplay(void) 
{
    memcpy(Msg11, " Aux       ", MID_L_SIZE);
    DispSendToMidL(Msg11);
}

void CdEmulatorStartCd(void)
{
    CdEmulatorDisplay();

    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_STOP;
    cmdCdStatus[CD_STATUS_REQUEST] = CD_STATUS_REQUEST_PLAY;
    cmdCdStatus[CD_STATUS_DISK   ] = ctx.Status.nCdEmulatorDisk;
    cmdCdStatus[CD_STATUS_TRACK  ] = ctx.Status.nCdEmulatorTrack;
    ibus_send_msg(cmdCdStatus);
    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_PLAY;
    ibus_send_msg(cmdCdStatus);

    ctx.Status.bCdEmulatorOn  = TRUE;
}

void CdEmulatorStopCd(void)
{
    ctx.Status.bCdEmulatorOn  = FALSE;

    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_END;
    cmdCdStatus[CD_STATUS_REQUEST] = CD_STATUS_REQUEST_PAUSE; 
    cmdCdStatus[CD_STATUS_DISK   ] = ctx.Status.nCdEmulatorDisk;
    cmdCdStatus[CD_STATUS_TRACK  ] = ctx.Status.nCdEmulatorTrack;
    ibus_send_msg(cmdCdStatus);
}

void CdEmulatorTrackNext(void)
{
    //ctx.Status.nCdEmulatorTrack = ++ctx.Status.nCdEmulatorTrack % 2;

    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_STOP;
    cmdCdStatus[CD_STATUS_REQUEST] = CD_STATUS_REQUEST_PLAY;
    cmdCdStatus[CD_STATUS_DISK   ] = ctx.Status.nCdEmulatorDisk;
    cmdCdStatus[CD_STATUS_TRACK  ] = ctx.Status.nCdEmulatorTrack;
    ibus_send_msg(cmdCdStatus);
    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_PLAY;
    ibus_send_msg(cmdCdStatus);
}

void CdEmulatorTrackPrv(void)
{
    //ctx.Status.nCdEmulatorTrack = --ctx.Status.nCdEmulatorTrack % 2;

    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_STOP;
    cmdCdStatus[CD_STATUS_REQUEST] = CD_STATUS_REQUEST_PLAY;
    cmdCdStatus[CD_STATUS_DISK   ] = ctx.Status.nCdEmulatorDisk;
    cmdCdStatus[CD_STATUS_TRACK  ] = ctx.Status.nCdEmulatorTrack;
    ibus_send_msg(cmdCdStatus);
    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_PLAY;
    ibus_send_msg(cmdCdStatus);
}

void CdEmulatorLoadDisk(AVR_U8 disk)
{
    ctx.Status.nCdEmulatorDisk = disk;

    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_LOADING;
    cmdCdStatus[CD_STATUS_REQUEST] = CD_STATUS_REQUEST_PLAY;
    cmdCdStatus[CD_STATUS_DISK   ] = ctx.Status.nCdEmulatorDisk;
    cmdCdStatus[CD_STATUS_TRACK  ] = 0;
    ibus_send_msg(cmdCdStatus);
    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_END;
    ibus_send_msg(cmdCdStatus);
    cmdCdStatus[CD_STATUS_STATUS ] = CD_STATUS_STATUS_PLAY;
    ibus_send_msg(cmdCdStatus);
}

void CdEmulatorReportStatus(void)
{
    ibus_send_msg(cmdCdStatus);
}

// todo test it
/* Reset device */
void reset(void)
{
    // needs fix in bootloader
    //cli();
    //wdt_enable(WDTO_250MS);
    //while(1);
}
