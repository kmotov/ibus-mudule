/*
 * display.h
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "defines.h"

void GoToRadio(void);

void MidRefresh(void);
void MidClearL(void);
void SetToDefaultMid(void);
void SetToDefaultMidR(void);
void SetToDefaultIke(void);
void DispSetToDefault(void);

void DispSendToNavi(AVR_U8 *data);
void SetToDefaultNavi(void);
void DispSendToNaviIndex(AVR_U8 *data, AVR_U8 index);

void DispSendToMidL(AVR_U8 *data);
void DispSendToMidR(AVR_U8 *data);
void DispSendToIke(AVR_U8 *data);
void DispSendToIkePersistent(AVR_U8 *data);

void DispMidReload(void);

void DispClearMidKeys(void);
void DispSendToMidKey(AVR_U8 data[MID_BUTTONS_SIZE / 2], AVR_U8 key);
void DispSendToMidKeys(AVR_U8 data[MID_BUTTONS_SIZE], AVR_U8 disp);

#endif /* DISPLAY_H_ */