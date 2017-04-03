/*
 * main.c
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 

#include <string.h>
#include "led.h"
#include "ibus.h"
#include "tools.h"
#include "menu.h"
#include "display.h"
#include "eeprom.h"


AVR_U8 dbg_mark6;

led_t ledPower;

IBusCtxT ctx;

AVR_U16 PwrMngrCnt = 0;

extern AVR_U8 cmdGetPdc[];
extern AVR_U8 cmdCarDataRequest[];
extern AVR_U8 Msg20[MID_R_SIZE];

AVR_S16 nRxMaxSize = 0;
AVR_S16 nTxMaxSize = 0;
AVR_U16 nRxSkiped = 0;
AVR_U16 nTxSkiped = 0;

AVR_U8 dbg_mark5;

extern AVR_U8 dbg_mark_end;
extern AVR_U8 dbg_mark4;
extern AVR_U8 dbg_mark3;
extern AVR_U8 dbg_mark2;
extern AVR_U8 dbg_mark1;
extern AVR_U8 dbg_mark0;

AVR_S16 isr_count = 0;
inline void RunSysTimer(void)
{
    isr_count = 0;

    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); // Timer Clock = system clock / 1024
    TCNT2  = 0;                         // reset Timer Count Register
    TIFR2  = (1 << TOV2);               // Clear TOV0 / Clear pending interrupts
    TIMSK2 = (1 << TOIE2);              // Enable Timer 0 Overflow Interrupt
}

void StopSysTimer(void)
{
    TIMSK2 &= ~(1 << TOIE2);            // Disable Timer 0 Overflow Interrupt
}

inline void SleepModePowerDown(void)
{
    uint8_t oldSREG;

    led_off(&ledPower);

    SMCR = (2 << 1);            // set sleep mode - power down

    oldSREG = SREG;             // save status register
    cli();                      // interrupt disable
    PCICR |= (1 << PCIE2);      // enable pin interrupt
    PCMSK2 |= (1 << PCINT16);   // enable pin interrupt on rx pin
    SMCR |= 1;                  // enable sleep mode
    // sleep_bod_disable();
    sei();                      // interrupt enable
    SREG = oldSREG;             // restore status register
    __asm__ __volatile__ ( "sleep" "\n\t" :: );
}

inline void SleepModeDisable(void)
{
    // disable sleep mode
    SMCR = 0;

    // disable pin interupt
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT16); // rx pin

    led_on(&ledPower);
}

void SaveCfg(IBusCfgT *cfg)
{
    AVR_U8 * tmp = (AVR_U8 *)cfg;
    AVR_U8 i;

    for (i = 0; i < sizeof(IBusCfgT); i++) {
        eeprom_write_byte((AVR_U8 *)(IBUS_EEPROM_ADDR_CFG + i), tmp[i]);
    }
}

void LoadCfg(IBusCfgT *cfg)
{
    AVR_U8 * tmp = (AVR_U8 *)cfg;
    AVR_U8 i;

    for (i = 0; i < sizeof(IBusCfgT); i++) {
        tmp[i] = eeprom_read_byte((AVR_U8 *)(IBUS_EEPROM_ADDR_CFG + i));
    }
}

//void SaveVin(AVR_U8 *vin)
//{
//    int i;
//
//    for (i = 0; i < VIN_SIZE; i++) {
//        eeprom_write_byte((AVR_U8 *)(IBUS_EEPROM_ADDR_VIN + i), vin[i]);
//    }
//}
//
//void LoadVin(AVR_U8 *vin)
//{
//    int i;
//
//    for (i = 0; i < VIN_SIZE; i++) {
//#ifdef NEW_CAR
//        vin[i] = 0;
//#else
//        vin[i] = eeprom_read_byte((AVR_U8 *)(IBUS_EEPROM_ADDR_VIN + i));
//#endif
//    }
//}

//AVR_U8 MidMsg[MID_R_SIZE] = " Can not read VIN  ";
//AVR_U8 CarVin[VIN_SIZE];
AVR_U8 temp_2bytes[2];
//
//void checkVin(void)
//{
//    AVR_U8 *cmd;
//    AVR_U8 i = 0;
//    
//    LoadVin(ctx.nVin);
//
//    ibus_send_msg(carDataRequest);
//
//    while(1) {
//        cmd = ibus_get_msg();
//        if (cmd) {
//            if (cmd[3] == IBUS_CMD_CAR_DATA_STATUS) {
//                for (i = 0; i < VIN_SIZE; i++) {
//                    CarVin[i] = cmd[i + 4];
//                }
//                break;
//            }
//            i++;
//            if (!(i % 10)) {
//                ibus_send_msg(carDataRequest);
//            }
//            if (i > 105) {
//                //MidMsg = " Can not read VIN  ";
//                DispSendToMidR(MidMsg);
//                while(1);
//            }
//        }
//    }
//
//    // Check VIN
//    if (!ctx.nVin[0]) {
//        memcpy(ctx.nVin, CarVin, VIN_SIZE);
//        
//        MidMsg[0]  = ' ';
//        MidMsg[1]  = 'S';
//        MidMsg[2]  = 'a';
//        MidMsg[3]  = 'v';
//        MidMsg[4]  = 'e';
//        MidMsg[5]  = ' ';
//        MidMsg[6]  = 'V';
//        MidMsg[7]  = 'I';
//        MidMsg[8]  = 'N';
//        MidMsg[9]  = ':';
//        MidMsg[10] = ' ';
//        MidMsg[11] = ' ';
//        MidMsg[12] = ' ';
//        MidMsg[13] = ctx.nVin[0];
//        MidMsg[14] = ctx.nVin[1];
//        getHexAncci(ctx.nVin[2], temp_2bytes);
//        MidMsg[15] = temp_2bytes[0];
//        MidMsg[16] = temp_2bytes[1];
//        getHexAncci(ctx.nVin[2], temp_2bytes);
//        MidMsg[17] = temp_2bytes[0];
//        MidMsg[18] = temp_2bytes[1];
//        getHexAncci(ctx.nVin[2], temp_2bytes);
//        MidMsg[19] = temp_2bytes[0];
//
//        DispSendToMidR(MidMsg);
//        SaveVin(ctx.nVin);
//    } else if (memcmp(CarVin, ctx.nVin, VIN_SIZE)) {
//        
//        MidMsg[0]  = ' ';
//        MidMsg[1]  = 'L';
//        MidMsg[2]  = 'o';
//        MidMsg[3]  = 'c';
//        MidMsg[4]  = 'k';
//        MidMsg[5]  = 'e';
//        MidMsg[6]  = 'd';
//        MidMsg[7]  = ' ';
//        MidMsg[8]  = 't';
//        MidMsg[9]  = 'o';
//        MidMsg[10] = ':';
//        MidMsg[11] = ' ';
//        MidMsg[12] = ' ';
//        MidMsg[13] = ctx.nVin[0];
//        MidMsg[14] = ctx.nVin[1];
//        getHexAncci(ctx.nVin[2], temp_2bytes);
//        MidMsg[15] = temp_2bytes[0];
//        MidMsg[16] = temp_2bytes[1];
//        getHexAncci(ctx.nVin[2], temp_2bytes);
//        MidMsg[17] = temp_2bytes[0];
//        MidMsg[18] = temp_2bytes[1];
//        getHexAncci(ctx.nVin[2], temp_2bytes);
//        MidMsg[19] = temp_2bytes[0];
//
//        DispSendToMidR(MidMsg);
//        while(1); // lock
//    }
//}

inline void init () 
{
    sei();
    RunSysTimer();
    
    ledPower.port = POWER_LED_PORT;
    ledPower.pin = POWER_LED_PIN;
    led_init(&ledPower);

    // led is turn off during power saver mode only
    led_on(&ledPower);

    ibus_init();

    ctx.Status.eMulti         = MULTIMEDIA_NONE;          // dynamic
    ctx.Status.nSpeed         = 0;                        // dynamic
    ctx.Status.bIgnitionRad   = FALSE;                    // dynamic
    ctx.Status.bIgnitionOn    = FALSE;                    // dynamic
    ctx.Status.bEngineOn      = FALSE;                    // dynamic
    ctx.Status.bCarRunning    = FALSE;                    // dynamic
    ctx.Status.bPdcOn         = FALSE;                    // dynamic
    ctx.Status.nLocked        = INTERIOR_STATUS_UNLOCKED; // dynamic
    ctx.Status.bLightsOn      = FALSE;                    // dynamic
    ctx.Status.bRadioOn       = FALSE;                    // dynamic
    ctx.Status.nLockDClickCnt = 0;                        // local
    ctx.Status.nUnLockDClickCnt = 0;                      // local
    ctx.Status.nNtwk          = NETWORK_NONE;             // dynamic
    ctx.Status.cGear          = '?';                      // dynamic
    ctx.Status.eMirrorFold    = MIRROR_UNFOLD;            // local
    ctx.Status.nMirrorCnt     = 0;                        // local
    ctx.Status.bWelcomeOn     = FALSE;                    // local
    ctx.Status.nInterior      = 0;                        // dynamic
    ctx.Status.nCarKey        = 0xFF;                     // dynamic
    ctx.Status.bCdEmulatorOn  = FALSE;                    // local
    ctx.Status.nCdEmulatorDisk  = 1;                      // local
    ctx.Status.nCdEmulatorTrack = 1;                      // local

    dbg_mark0 = 66;
    dbg_mark1 = 66;
    dbg_mark2 = 66;
    dbg_mark3 = 66;
    dbg_mark4 = 66;
    dbg_mark5 = 66;
    dbg_mark6 = 66;
    dbg_mark_end = 66;
    
    memset(&ctx.Config, 0, sizeof(ctx.Config));

    LoadCfg(&ctx.Config);

    // this for initial initialization of eeprom
    //SaveCfg(&ctx.Config);
    
    // checkVin();

    // todo check for supported functions

    MenuDisableUnsuportedFeatures();
}

AVR_S16 main(void)
{
    AVR_U8 *cmd;
    //remote_key_t eRemButtClicked = REMOTE_KEY_NO;
    AVR_U8 locked;
    
    init();

    while(1) {
        if (PwrMngrCnt > PWR_MNGR_TIMEOUT) {
            // counter should be clear,
            // otherwise it will sleep immediately after resume
            PwrMngrCnt = 0;
            SleepModePowerDown();
        }

        cmd = ibus_get_msg();
        if (cmd) {
            PwrMngrCnt = 0; // reset power manager counter
            switch (cmd[3]) {
                case IBUS_CMD_RET_DATA:
                    if (cmd[0] == IBUS_DEV_PDC && cmd[2] == IBUS_DEV_DIA && ctx.Config.nPdc)
                        ctx.Status.bPdcOn = PdcParseAndPrint(cmd);
                    break;

                case IBUS_CMD_IKE_SEN_STAT:
                    if ((cmd[5] >> 4) == GEAR_R) {
                        ctx.Status.cGear = 'R';
                        if (ctx.Config.nPdc)
                            ctx.Status.bPdcOn = TRUE;
                    }
                    else if ((cmd[5] >> 4) == GEAR_D)
                        ctx.Status.cGear = 'D';
                    else if ((cmd[5] >> 4) == GEAR_N)
                        ctx.Status.cGear = 'N';
                    else if ((cmd[5] >> 4) == GEAR_P)
                        ctx.Status.cGear = 'P';
                    else if ((cmd[5] >> 4) == GEAR_1)
                        ctx.Status.cGear = '1';
                    else if ((cmd[5] >> 4) == GEAR_2)
                        ctx.Status.cGear = '2';
                    else if ((cmd[5] >> 4) == GEAR_3)
                        ctx.Status.cGear = '3';
                    else if ((cmd[5] >> 4) == GEAR_4)
                        ctx.Status.cGear = '4';
                    else if ((cmd[5] >> 4) == GEAR_5)
                        ctx.Status.cGear = '5';
                    else
                        ctx.Status.cGear = '_';
                    break;

                case IBUS_CMD_STAT_CHENGED:
                    if (cmd[0] == IBUS_DEV_PDC && ctx.Config.nPdc)
                        ctx.Status.bPdcOn = TRUE;
                    break;

                case IBUS_CMD_IGN_STAT:
                    if ((ctx.Status.bIgnitionRad == 0) && (cmd[4] & 1)) {
                        if (ctx.Config.nHello)
                            SayHello(ctx.Config.nHello);
                    }
                    ctx.Status.bIgnitionRad = cmd[4] & 1;
                    ctx.Status.bIgnitionOn = (cmd[4] & 2) >> 1;

                    if (!ctx.Status.bIgnitionRad) {
                        ctx.Status.bRadioOn = FALSE;
                    }
                    break;

                case IBUS_CMD_SPEED_RPM:
                    if ((ctx.Status.bEngineOn == 1) && (cmd[5] == 0)) {
                        if ((ctx.Config.nAutoLock) && 
                            (ctx.Status.nLocked == INTERIOR_STATUS_LOCKED)) {
                            ToggleLockUnlock(); // Unlock
                            // Status will be update later automatically, but if status is not set it might toggle lock/unlock again
                            ctx.Status.nLocked = INTERIOR_STATUS_UNLOCKED; 
                        }
                        // Clear display when engine is turn off
                        DispSetToDefault();
                    }

                    ctx.Status.nSpeed = cmd[4] * 2; // speed [km/h]
                    ctx.Status.bEngineOn = cmd[5] ? TRUE : FALSE; // oboroti
                    ctx.Status.bCarRunning = (ctx.Status.nSpeed > 3) ? TRUE : FALSE;

                    // Lock car if auto-lock is enable, car is running and car is still unlock
                    if ((ctx.Config.nAutoLock > 0) && (ctx.Config.nAutoLock < ctx.Config.nAutoLockMax) && 
                        (ctx.Status.nLocked == INTERIOR_STATUS_UNLOCKED) &&
                        (ctx.Status.nSpeed >= (AVR_U8)(ctx.Config.nAutoLock * 10))) {
                        ToggleLockUnlock(); // Lock
                        // Status will be update later automatically, but if status is not set it might toggle lock/unlock again
                        ctx.Status.nLocked = INTERIOR_STATUS_LOCKED;
                    }
                    break;

                case IBUS_CMD_SEND_TO_DISP:
                    // Display Preempted
                    DispMidReload();
                    
                    if (cmd[0] == IBUS_DEV_RADIO && cmd[2] == IBUS_DEV_MID) {
                        if (cmd[1] == 0x05 && cmd[5] == 0x20) { // 68 05 C0 23 00 20 AE
                            ctx.Status.bRadioOn = FALSE;
                        } else {
                            ctx.Status.bRadioOn = TRUE;
                        }
                    }
                    break;

                case IBUS_CMD_INTER_STAT:
                    locked = INTERIOR_STATUS_LOCKED_MASK & cmd[4];
                    ctx.Status.nInterior = *((AVR_U16 *)&cmd[4]);

                    if (locked == INTERIOR_STATUS_LOCKED_FULL && 
                        ctx.Status.nLocked == INTERIOR_STATUS_UNLOCKED &&
                        ctx.Config.nMirrorFold) {
                        mirrorFold();
                    } else
                    if (locked == INTERIOR_STATUS_UNLOCKED &&
                        ctx.Status.nLocked == INTERIOR_STATUS_LOCKED_FULL &&
                        ctx.Config.nWelcome) {
                        WellcomeLights();
                    }
                    ctx.Status.nLocked = locked;
                    break;

//                case IBUS_CMD_REM_KEY_PRESS: // Remote keys
//                    if (cmd[4] & 0x10) { // lock button
//                        eRemButtClicked = REMOTE_KEY_LOCK;
//                        ctx.Status.nUnLockDClickCnt = 0;
//
//                        if (ctx.Status.nLockDClickCnt > 0) { // second click
//                            ctx.Status.nLockDClickCnt = 0;
//                            midLedOff();
//
//                            if (ctx.Config.nMirrorFold == 2) { // double click fold 2
//                                mirrorFold();
//                            }
//                        } else if (ctx.Status.nLockDClickCnt == 0) { // first click
//                            ctx.Status.nLockDClickCnt = -1;
//
//                            if (ctx.Config.nMirrorFold == 1) { // auto fold 1
//                                mirrorFold();
//                            }
//
//                            if (ctx.Config.nWelcome && ctx.Status.bIgnitionRad == FALSE && ctx.Status.bWelcomeOn == TRUE) {
//                                ctx.Status.bWelcomeOn = FALSE;
//                                //ibus_send_msg(CmdLightsOff); // tabloto sveti
//                            }
//                        }
//
//                    } else if (cmd[4] & 0x20) { // unlock button
//                        eRemButtClicked = REMOTE_KEY_UNLOCK;
//                        ctx.Status.nLockDClickCnt = 0;
//
//                        if (ctx.Status.nUnLockDClickCnt > 0) { // second click
//                            ctx.Status.nUnLockDClickCnt = 0;
//                            midLedOff();
//
//                            if (ctx.Config.nMirrorFold) {
//                                mirrorUnfold(); // fold out on double unlock click
//                            }
//                        } else if (ctx.Status.nUnLockDClickCnt == 0) { // first click
//                            ctx.Status.nUnLockDClickCnt = -1;
//
//                            if (ctx.Config.nWelcome && ctx.Status.bIgnitionRad == FALSE && ctx.Status.bWelcomeOn == FALSE) {
//                                ctx.Status.bWelcomeOn = TRUE;
//                                if (ctx.Config.nWelcome == 1) { // side light
//                                    ibus_send_msg(CmdSideLight);
//                                } else if (ctx.Config.nWelcome == 2) { // low beam
//                                    ibus_send_msg(CmdLBeam);
//                                } else if (ctx.Config.nWelcome == 3) { // fog light
//                                    ibus_send_msg(CmdFogs);
//                                }
//                            }
//                        }
//					} else if (cmd[4] & 0x40) { // boot button
//                        eRemButtClicked = REMOTE_KEY_BOOT;
//                    }
//
//                    if (((cmd[4] & 0xF0) == 0) && (eRemButtClicked != REMOTE_KEY_NO)) { // key release
//                        /* Start double click counter here because of issue. 
//                            It looks like car send two times unlock command in some case. */
//                        //if (eRemButtClicked == REMOTE_KEY_LOCK) {
//                        //    if (!ctx.Status.nLockDClickCnt) {
//                        //        ctx.Status.nLockDClickCnt = 5;
//                        //    }
//                        //} else if (eRemButtClicked == REMOTE_KEY_UNLOCK) {
//                        //    if (!ctx.Status.nUnLockDClickCnt) {
//                        //        ctx.Status.nUnLockDClickCnt = 5;
//                        //    }
//                        //}
//                        eRemButtClicked = REMOTE_KEY_NO;
//                    }
//                    break;

                case IBUS_CMD_EWS_STATUS:
                    if (cmd[5] != 0xFF) { // key plug in
                        if (ctx.Config.nMirrorFold) {
                            mirrorUnfold();
                        }
                        ctx.Status.nCarKey = cmd[5];
                    }
                    break;

                case IBUS_CMD_LIGHT_STATUS:
                    if (cmd[0] != IBUS_DEV_LKM || cmd[2] != IBUS_DEV_LCM || cmd[1] != 0x07) {
                        break; // for sure
                    }
                    if (cmd[4] & 0x01) { // gabarit
                        //
                    }
                    ctx.Status.bLightsOn = FALSE;
                    if (cmd[4] & 0x02) { // farove
                        ctx.Status.bLightsOn = TRUE;
                    }
                    if (cmd[4] & 0x08) { // halogen
                        //
                    }
                    if (cmd[4] & 0x20) { // migach L
                        if (ctx.Status.bLightsOn && ctx.Status.nSpeed < 50) {
                            //ibus_send_msg(CmdFogL);
                        }
                    }
                    if (cmd[4] & 0x40) { // migach R
                        if (ctx.Status.bLightsOn && ctx.Status.nSpeed < 50) {
                            //ibus_send_msg(CmdFogR);
                        }
                    }
                    break;

                case IBUS_CMD_MID_SBUTTON_PRESS:
                    if (cmd[4] == MID_KEY_BC_LONG) {
                        StopSysTimer();
                        if (ctx.Status.eMulti == MULTIMEDIA_MID) {
                            MenuMid();
                        } else if (ctx.Status.eMulti == MULTIMEDIA_NAVI) {
                            //MenuNavi(); // todo move to other button
                        }
                        SaveCfg(&ctx.Config);
                        RunSysTimer();
                    }
                    break;

                case IBUS_CMD_CNTR_CODE_STAT:
                    ctx.Status.nNtwk = cmd[4] >> 4; // model
                    break;

                case IBUS_CMD_POLL:
                case IBUS_CMD_RESPOND:
//                    if (cmd[0] == IBUS_DEV_IRIS) // this is generate without iris
//                        ctx.Status.eMulti = MULTIMEDIA_IRIS;

                    if (cmd[0] == IBUS_DEV_MID || cmd[2] == IBUS_DEV_MID) {
                        ctx.Status.eMulti = MULTIMEDIA_MID;
                    }

                    if (cmd[0] == IBUS_DEV_BMB || cmd[2] == IBUS_DEV_BMB || 
                        cmd[0] == IBUS_DEV_GT  || cmd[2] == IBUS_DEV_GT) {
                        ctx.Status.eMulti = MULTIMEDIA_NAVI;
                    }
                    break;

                case IBUS_CMD_REQUEST_CD: 
                // todo send status regularly
                    if (ctx.Config.nAudioAux) {
                        switch (cmd[4]) {
                            case 0x00: // Status request
                                CdEmulatorReportStatus();
                                break;

                            case 0x01: // Stop
                                CdEmulatorStopCd();
                                break;

                            case 0x03: // Start
                                CdEmulatorStartCd();
                                break;

                            case 0x06: // LoadDisk
                                CdEmulatorLoadDisk(cmd[5]);
                                break;

                            case 0x0A: // Switch track
                                if (!cmd[5]) {
                                    CdEmulatorTrackNext();
                                } else {
                                    CdEmulatorTrackPrv();
                                }
                                break;

                            default:
                                CdEmulatorReportStatus();
                                break;
                        }
                    }
                    break;

                // todo: search for device

                default:
                    break;
            }
        }
    }
}

/****************************************************************/
/*                      Interrupt handlers                      */
/****************************************************************/
ISR(TIMER2_OVF_vect)
{
    AVR_U16 error = 0;

    TCNT2 = 0; // reset Timer Count Register
    isr_count++;

    if (!((isr_count + 0x0) & 0x0F)) {// ~250ms
        if( ctx.Status.bPdcOn ) {
            ibus_send_msg(cmdGetPdc);
        }
    }

    if (!((isr_count + 0x8) & 0x1F)) {// ~500ms

    }

    if (!((isr_count + 0x18) & 0x3F)) {// ~1s
        if ((ctx.Status.bIgnitionRad == FALSE) &&
            (ctx.Status.bIgnitionOn  == FALSE) &&
            (ctx.Status.bEngineOn    == FALSE) &&
            (ctx.Status.bCarRunning  == FALSE)) {
            PwrMngrCnt++;
        }

        //if (ctx.Status.nLockDClickCnt > 0) {
        //    ctx.Status.nLockDClickCnt--;
        //    if (!ctx.Status.nLockDClickCnt) {
        //        midLedOff();
        //    }
        //} else if (ctx.Status.nLockDClickCnt < 0) {
        //    ctx.Status.nLockDClickCnt = 3;
        //    midLedOn();
        //}
        //if (ctx.Status.nUnLockDClickCnt > 0) {
        //    ctx.Status.nUnLockDClickCnt--;
        //    if (!ctx.Status.nUnLockDClickCnt) {
        //        midLedOff();
        //    }
        //} else if (ctx.Status.nUnLockDClickCnt < 0) {
        //    ctx.Status.nUnLockDClickCnt = 3;
        //    midLedOn();
        //}
        
        /* Cancel mirror moving after mirror moving is finish as workaround */
        if (ctx.Status.nMirrorCnt) {
            ctx.Status.nMirrorCnt--;
            if (!ctx.Status.nMirrorCnt) {
                if (ctx.Status.eMirrorFold == MIRROR_FOLDING) {
                    mirrorFoldEnd();
                }
                if (ctx.Status.eMirrorFold == MIRROR_UNFOLDING) {
                    mirrorUnfoldEnd();
                }
            }
        }

        error |= (dbg_mark0 == 66) ? 0 : (1 << 0);
        error |= (dbg_mark1 == 66) ? 0 : (1 << 1);
        error |= (dbg_mark2 == 66) ? 0 : (1 << 2);
        error |= (dbg_mark3 == 66) ? 0 : (1 << 3);
        error |= (dbg_mark4 == 66) ? 0 : (1 << 4);
        error |= (dbg_mark5 == 66) ? 0 : (1 << 5);
        error |= (dbg_mark6 == 66) ? 0 : (1 << 6);

#ifndef IBUS_DEBUG
    if (error) {
        led_off(&ledPower);
        reset();
    }
#endif // IBUS_DEBUG

        // it will be overwrite PDC raw data
        if (dbg_mark_end != 66) {
            Msg20[ 0] = ' ';
            Msg20[ 1] = ' ';
            Msg20[ 2] = ' ';
            Msg20[ 3] = ' ';
            Msg20[ 4] = 'S';
            Msg20[ 5] = 't';
            Msg20[ 6] = 'a';
            Msg20[ 7] = 'c';
            Msg20[ 8] = 'k';
            Msg20[ 9] = ' ';
            Msg20[10] = 'O';
            Msg20[11] = 'v';
            Msg20[12] = 'e';
            Msg20[13] = 'r';
            Msg20[14] = 'f';
            Msg20[15] = 'l';
            Msg20[16] = 'o';
            Msg20[17] = 'w';
            Msg20[18] = ' ';
            Msg20[19] = ' ';            
        } else if (error) {
            Msg20[ 0] = (AVR_U8)(nRxMaxSize / 100 + 48);
            Msg20[ 1] = (AVR_U8)(((nRxMaxSize / 10) % 10) + 48);
            Msg20[ 2] = (AVR_U8)(nRxMaxSize % 10 + 48);
            Msg20[ 3] = 0x20;
            Msg20[ 4] = (AVR_U8)(nTxMaxSize / 100 + 48);
            Msg20[ 5] = (AVR_U8)(((nTxMaxSize / 10) % 10) + 48);
            Msg20[ 6] = (AVR_U8)(nTxMaxSize % 10 + 48);
            Msg20[ 7] = 0x20;
            Msg20[ 8] = 0x20;
            Msg20[ 9] = 0x20;
            Msg20[10] = 'E';
            Msg20[11] = 'r';
            Msg20[12] = 'r';
            Msg20[13] = 'o';
            Msg20[14] = 'r';
            Msg20[15] = ':';
            Msg20[16] = 0x20;
            Msg20[17] = (AVR_U8)(error / 100 + 48);
            Msg20[18] = (AVR_U8)(((error / 10) % 10) + 48);
            Msg20[19] = (AVR_U8)(error % 10 + 48);
            DispSendToMidR(Msg20);
        } else if (ctx.Config.nDebug) {
            AVR_U8 net[2];

            getHexAncci(ctx.Status.nNtwk, net);
            Msg20[ 0] = '0';
            Msg20[ 1] = 'x';
            Msg20[ 2] = net[0];
            Msg20[ 3] = net[1];
            Msg20[ 4] = 0x20;
            Msg20[ 5] = 0x20;
            Msg20[ 6] = ctx.Status.cGear;
            Msg20[ 7] = 0x20;
            Msg20[ 8] = ctx.Status.bIgnitionRad   ? '1' : 'N';
            Msg20[ 9] = ctx.Status.bIgnitionOn    ? '2' : 'N';
            Msg20[10] = ctx.Status.bEngineOn      ? '3' : 'N';
            Msg20[11] = ctx.Status.bCarRunning    ? '4' : 'N';
            Msg20[12] = 0x20;
            Msg20[13] = '0' + ctx.Status.eMulti;
            Msg20[14] = '0' + ctx.Status.eMirrorFold;
            Msg20[15] = 0x20;
            Msg20[16] = ctx.Status.nLocked != INTERIOR_STATUS_UNLOCKED ? 'L' : 'N';
            Msg20[17] = ctx.Status.bPdcOn         ? 'P' : 'N';
            Msg20[18] = ctx.Status.bLightsOn      ? 'Y' : 'N';
            Msg20[19] = ctx.Status.bRadioOn       ? 'R' : 'N';
            if (ctx.Config.nDebug == 1 || ctx.Config.nDebug == 3)
                DispSendToMidR(Msg20);
            else if (ctx.Config.nDebug == 2)
                DispSendToIke(Msg20);
        }
    }

    if (!(isr_count & 0xFF)) {// ~4s
        isr_count = 0;

        if (ctx.Config.nDebug == 3) {
            Msg20[ 0] = ctx.nVin[0];
            Msg20[ 1] = ctx.nVin[1];
            getHexAncci(ctx.nVin[2], temp_2bytes);
            Msg20[ 2] = temp_2bytes[0];
            Msg20[ 3] = temp_2bytes[1];
            getHexAncci(ctx.nVin[2], temp_2bytes);
            Msg20[ 4] = temp_2bytes[0];
            Msg20[ 5] = temp_2bytes[1];
            getHexAncci(ctx.nVin[2], temp_2bytes);
            Msg20[ 6] = temp_2bytes[0];
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
            DispSendToIke(Msg20);
        }
    }
}

ISR(PCINT2_vect)
{
    SleepModeDisable();
}
