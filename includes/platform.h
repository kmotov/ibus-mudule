/*
 * platform.h
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 

#ifndef PLATFORM_H_
#define PLATFORM_H_

/* Devices*/
#define IBUS_DEV_ALL0               (0x00)
#define IBUS_DEV_CDC                (0x18)
#define IBUS_DEV_CCM                (0x30)
#define IBUS_DEV_GT                 (0x3B)
#define IBUS_DEV_DIA                (0x3F)
#define IBUS_DEV_FBZV               (0x40)
#define IBUS_DEV_GTF                (0x43)
#define IBUS_DEV_EWS                (0x44)
#define IBUS_DEV_CID                (0x46)
#define IBUS_DEV_FMBT               (0x47)
#define IBUS_DEV_MFL                (0x50)
#define IBUS_DEV_MML                (0x51)
#define IBUS_DEV_IHK                (0x5B)
#define IBUS_DEV_PDC                (0x60)
#define IBUS_DEV_CDCD               (0x66)
#define IBUS_DEV_RADIO              (0x68)
#define IBUS_DEV_DSP                (0x6A)
#define IBUS_DEV_RDC                (0x70)
#define IBUS_DEV_SM                 (0x72)
#define IBUS_DEV_SDRS               (0x73)
#define IBUS_DEV_CDCD2              (0x76)
#define IBUS_DEV_OBC1               (0x7E)
#define IBUS_DEV_NAVE               (0x7f)
#define IBUS_DEV_IKE                (0x80)
#define IBUS_DEV_MMR                (0x9B)
#define IBUS_DEV_CVM                (0x9C)
#define IBUS_DEV_FMID               (0xA0)
#define IBUS_DEV_ACM                (0xA4)
#define IBUS_DEV_FHK                (0xA7)
#define IBUS_DEV_NAVC               (0xA8)
#define IBUS_DEV_EHC                (0xAC)
#define IBUS_DEV_SES                (0xB0)
#define IBUS_DEV_TV                 (0xBB)
#define IBUS_DEV_LCM                (0xBF)
#define IBUS_DEV_MID                (0xC0)
#define IBUS_DEV_TEL                (0xC8)
#define IBUS_DEV_LKM                (0xD0)
#define IBUS_DEV_SMAD               (0xDA)
#define IBUS_DEV_IRIS               (0xE0)
#define IBUS_DEV_OBC2               (0xE7)
#define IBUS_DEV_SIP                (0xE8)
#define IBUS_DEV_LWSMT              (0xED)
#define IBUS_DEV_BMB                (0xF0)
#define IBUS_DEV_CSU                (0xF5)
#define IBUS_DEV_ALLF               (0xFF)

/* Commands */
#define IBUS_CMD_POLL               (0x01) //
#define IBUS_CMD_RESPOND            (0x02) //
#define IBUS_CMD_DIA_CMD            (0x0C) // Ctrl command, common for all diagnostic command
#define IBUS_CMD_IGN_STAT           (0x11) // Ignition status
#define IBUS_CMD_IKE_SEN_STAT       (0x13) // IKE Sensor Status. Reverse gear, but not prove
#define IBUS_CMD_CNTR_CODE_STAT     (0x15) // Country coding status
#define IBUS_CMD_SPEED_RPM          (0x18) // speed and rpm
#define IBUS_CMD_MID_SBUTTON_PRESS  (0x20) // Mid Special button press
#define IBUS_CMD_SEND_TO_MID_KEYS   (0x21) // Special button press
#define IBUS_CMD_SEND_TO_DISP_ACK   (0x22) // Show when audio is on
#define IBUS_CMD_SEND_TO_DISP       (0x23) // Send text to display
#define IBUS_CMD_MID_BUTTON_PRESS   (0x31) // MID regular MID button Press
#define IBUS_CMD_NAVI_LEFT_NOB      (0x32)
#define IBUS_CMD_REQUEST_CD         (0x38)
#define IBUS_CMD_NAVI_INFO_PRESS    (0x47)
#define IBUS_CMD_NAVI_BUTTON_PRESS  (0x48) // NAVI regular NAVI button Press
#define IBUS_CMD_NAVI_RIGHT_NOB     (0x49)
#define IBUS_CMD_CAR_DATA_REQUEST   (0x53) // Vehicle data request (VIN, km and service data)
#define IBUS_CMD_CAR_DATA_STATUS    (0x54) // return VIN, km and service 
#define IBUS_CMD_STAT_CHENGED       (0x5A) // Status changed
#define IBUS_CMD_LIGHT_STATUS       (0x5B) // Light status
#define IBUS_CMD_REM_KEY_PRESS      (0x72) // Remote key press
#define IBUS_CMD_EWS_STATUS         (0x74) // key plug in/plug out
#define IBUS_CMD_INTER_STAT         (0x7A) // Interior status
#define IBUS_CMD_RET_DATA           (0xA0) // Returned status or data on request

#define MID_L_SIZE                  (11)
#define MID_R_SIZE                  (20)
#define MID_BUTTONS_SIZE            (8)

#define MID_L_STR_SIZE              (MID_L_SIZE       + 1)
#define MID_R_STR_SIZE              (MID_R_SIZE       + 1)
#define MID_BUTTONS_STR_SIZE        (MID_BUTTONS_SIZE + 1)

#define IBUS_EEPROM_ADDR_VIN        (0x00)
#define IBUS_EEPROM_ADDR_CFG        (0x08)

#define PDC_OUT_SEN_RANGE_MIN       (25) // external sensors
#define PDC_OUT_SEN_RANGE_MAX       (57)
#define PDC_INT_SEN_RANGE_MIN       (25) // middle sensors
#define PDC_INT_SEN_RANGE_MAX       (149)

#define PDC_OUT_SEN_STEP ((PDC_OUT_SEN_RANGE_MAX - PDC_OUT_SEN_RANGE_MIN) / 4)
#define PDC_INT_SEN_STEP ((PDC_INT_SEN_RANGE_MAX - PDC_INT_SEN_RANGE_MIN) / 4)

/* Mid Buttons [3] = 0x31 / key:[6] */
#define MID_KEY_1                   (0x00)
#define MID_KEY_2                   (0x01)
#define MID_KEY_3                   (0x02)
#define MID_KEY_4                   (0x03)
#define MID_KEY_5                   (0x04)
#define MID_KEY_6                   (0x05)
#define MID_KEY_7                   (0x06)
#define MID_KEY_8                   (0x07)
#define MID_KEY_9                   (0x08)
#define MID_KEY_10                  (0x09)
#define MID_KEY_11                  (0x0A)
#define MID_KEY_12                  (0x0B)

/* Mid SButtons [3] = 0x20 / key:[4] */
#define MID_KEY_AUDIO               (0x01)
#define MID_KEY_BC                  (0x10)
#define MID_KEY_BC_LONG             (0x50)
#define MID_KEY_TIME                (0x08)
#define MID_KEY_POWER               (0x20)

/* Menu keys */
#define MENU_KEY_UP                 (MID_KEY_2)
#define MENU_KEY_DOWN               (MID_KEY_1)
#define MENU_KEY_SET_PRV            (MID_KEY_5)
#define MENU_KEY_SET_NEXT           (MID_KEY_6)
#define MENU_KEY_VER1               (MID_KEY_9)
#define MENU_KEY_VER2               (MID_KEY_10)

/* Interior Status cdm 15 */
#define INTERIOR_STATUS_DOOR_FL     (1 << 0)
#define INTERIOR_STATUS_DOOR_FR     (1 << 1)
#define INTERIOR_STATUS_DOOR_RL     (1 << 2)
#define INTERIOR_STATUS_DOOR_RR     (1 << 3)
#define INTERIOR_STATUS_UNLOCKED    (1 << 4)
#define INTERIOR_STATUS_LOCKED      (1 << 5)
#define INTERIOR_STATUS_LOCKED_FULL (1 << 4 | 1 << 5)
#define INTERIOR_STATUS_LOCKED_MASK (1 << 4 | 1 << 5)
#define INTERIOR_STATUS_INT_LIGHT   (1 << 6)
#define INTERIOR_STATUS_X1          (1 << 7)
#define INTERIOR_STATUS_WINDOW_FL   (1 << 8)
#define INTERIOR_STATUS_WINDOW_FR   (1 << 9)
#define INTERIOR_STATUS_WINDOW_RL   (1 << 10)
#define INTERIOR_STATUS_WINDOW_RR   (1 << 11)
#define INTERIOR_STATUS_SUNROOF     (1 << 12)
#define INTERIOR_STATUS_TRUNK       (1 << 13)
#define INTERIOR_STATUS_X2          (1 << 14)
#define INTERIOR_STATUS_X3          (1 << 15)

/* Networks cmd IBUS_CMD_CNTR_CODE_STAT byte 4 bit 4-7 */
#define NETWORK_E38_E39H_E53H       (0x0)
#define NETWORK_E53                 (0x1)
#define NETWORK_E52                 (0x2)
#define NETWORK_E39B_E53B_E52       (0x3)
#define NETWORK_E46_BEFORE_P98      (0x4)
#define NETWORK_E46_AFTER_P98       (0x6)
#define NETWORK_NONE                (0x9)
#define NETWORK_E83_E85             (0xA)
#define NETWORK_E65                 (0xE)
#define NETWORK_E46                 (0xF)

/* Gears cmd IBUS_CMD_IKE_SEN_STAT byte 5 bit 4-7 */
#define GEAR_R                      (0x1)
#define GEAR_D                      (0x8)
#define GEAR_N                      (0x6)
#define GEAR_P                      (0xB)
#define GEAR_1                      (0x2)
#define GEAR_2                      (0x4)
#define GEAR_3                      (0xD)
#define GEAR_4                      (0xC)
#define GEAR_5                      (0xE)

/* Navi keys cmd IBUS_CMD_NAVI_BUTTON_PRESS */
#define NAVI_KEY_1                  (0x11)
#define NAVI_KEY_2                  (0x01)
#define NAVI_KEY_3                  (0x12)
#define NAVI_KEY_4                  (0x02)
#define NAVI_KEY_5                  (0x13)
#define NAVI_KEY_6                  (0x03)
#define NAVI_KEY_MODE               (0x23)
#define NAVI_KEY_AM                 (0x21)
#define NAVI_KEY_FM                 (0x31)
#define NAVI_KEY_CASS               (0x30)
#define NAVI_KEY_LEFT_NOB           (0x06)
#define NAVI_KEY_RIGHT_NOB          (0x05)
#define NAVI_KEY_EJECT              (0x24)
#define NAVI_KEY_PHONE              (0x08)
#define NAVI_KEY_PRV                (0x10)
#define NAVI_KEY_NEXT               (0x00)
#define NAVI_KEY_REV                (0x14)
#define NAVI_KEY_CLOCK              (0x07)
#define NAVI_KEY_TONE               (0x04)
#define NAVI_KEY_MENU               (0x34)

/* Cd status bytes */
#define CD_STATUS_STATUS            (4)
#define CD_STATUS_REQUEST           (5)
#define CD_STATUS_LOAD_DISK         (7)
#define CD_STATUS_DISK              (9)
#define CD_STATUS_TRACK             (10)

#define CD_STATUS_STATUS_STOP       (0x00)
#define CD_STATUS_STATUS_PLAY       (0x02)
#define CD_STATUS_STATUS_END        (0x07)
#define CD_STATUS_STATUS_LOADING    (0x08)

#define CD_STATUS_REQUEST_PAUSE     (0x02)
#define CD_STATUS_REQUEST_PLAY      (0x09)


#endif /* PLATFORM_H_ */