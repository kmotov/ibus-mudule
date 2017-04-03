/*
 * defines.h
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_
#define DEFINES_H_

//#include <math.h>
//#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdio.h>

#include "platform.h"

/* HW versions */
#define HW_DEBUG 1
#define HW_V1 2

// todo
#define IBUS_DEBUG
#define MODULE_HW_VER HW_DEBUG

#define IBUS_MODULE_VERSION "Ver: 2.0"

#define AVR_PORTB   (2)
#define AVR_PORTC   (3)
#define AVR_PORTD   (4)


#if (MODULE_HW_VER == HW_DEBUG)
#define POWER_LED_PORT (AVR_PORTB);
#define POWER_LED_PIN  (5);
#elif (MODULE_HW_VER == HW_V1)
#define POWER_LED_PORT (AVR_PORTB);
#define POWER_LED_PIN  (1);
#else
#error
#endif

typedef unsigned char AVR_U8;
typedef signed char AVR_S8;

typedef unsigned int AVR_U16;
typedef signed int AVR_S16;

typedef void* AVR_PTR;

typedef unsigned char AVR_BOOL;

#define TRUE (1)
#define FALSE (0)

#define PWR_MNGR_TIMEOUT    (60)    // time to enter in sleep mode in seconds
#define AUTO_LOCK_SPEED     (10)    // threshold for lock of auto lock function in km/h
#define VIN_SIZE            (5)

typedef enum {
    MULTIMEDIA_NONE,
    MULTIMEDIA_IRIS,
    MULTIMEDIA_MID,
    MULTIMEDIA_NAVI
} multimedia_type_t;

typedef enum {
    REMOTE_KEY_NO,
    REMOTE_KEY_LOCK,
    REMOTE_KEY_UNLOCK,
    REMOTE_KEY_BOOT
} remote_key_t;

typedef enum {
    MIRROR_FOLD,
    MIRROR_FOLDING,
    MIRROR_UNFOLD,
    MIRROR_UNFOLDING
} mirror_fold_state_t;

typedef struct {
    multimedia_type_t   eMulti;
    AVR_U8              nSpeed;         // speed in km/h
    AVR_BOOL            bIgnitionRad;   // ignition radio position
    AVR_BOOL            bIgnitionOn;    // ignition
    AVR_BOOL            bEngineOn;      // engine is up
    AVR_BOOL            bCarRunning;    // car is moving
    AVR_BOOL            bPdcOn;         // park assist is running
    AVR_U8              nLocked;        // doors are locked
    AVR_BOOL            bLightsOn;      // head light are turn on
    AVR_BOOL            bRadioOn;       // audio is turn on
    AVR_S8              nLockDClickCnt; // counter for lock button
    AVR_S8              nUnLockDClickCnt; // counter for lock button
    AVR_U16             nInterior;      // interior status
    AVR_U8              nNtwk;          // network
    AVR_U8              cGear;          // gear
    mirror_fold_state_t eMirrorFold;    //
    AVR_U8              nMirrorCnt;
    AVR_BOOL            bWelcomeOn;     //
    AVR_U8              nCarKey;
    AVR_BOOL            bCdEmulatorOn;  // CD emulator status
    AVR_U8              nCdEmulatorDisk;
    AVR_U8              nCdEmulatorTrack;
} IBusStatT;

/* Need eeprom attention when new cfg is add */
typedef struct {
    AVR_U8 nPdc;
    AVR_U8 nPdcMax;
    AVR_U8 nPdcRaw;
    AVR_U8 nPdcRawMax;
    AVR_U8 nHello;
    AVR_U8 nHelloMax;
    AVR_U8 nMirrorFold;
    AVR_U8 nMirrorFoldMax;
    AVR_U8 nAutoLock;
    AVR_U8 nAutoLockMax;
    AVR_U8 nWelcome;
    AVR_U8 nWelcomeMax;
    AVR_U8 nDebug;
    AVR_U8 nDebugMax;
    AVR_U8 nFollowMe;
    AVR_U8 nFollowMeMax;
    AVR_U8 nAudioAux;
    AVR_U8 nAudioAuxMax;
} IBusCfgT;

typedef struct {
    IBusStatT Status;
    IBusCfgT Config;
    AVR_U8 nVin[VIN_SIZE]; // VIN 01 - letters 234 - hex numbers (skip last 4 bits)
} IBusCtxT;


#endif /* DEFINES_H_ */