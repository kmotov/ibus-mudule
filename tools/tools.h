/*
 * tools.h
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 


#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_


AVR_S16 PdcParseAndPrint(AVR_U8 *cmd);
void mirrorFold(void);
void mirrorUnfold(void);
void mirrorFoldEnd(void);
void mirrorUnfoldEnd(void);
void ToggleLockUnlock(void);
void getHexAncci(AVR_U8 in, AVR_U8 *out);
void printCmdIke(AVR_U8 *cmd);
void midLedOff(void);
void midLedOn(void);
void midLedBlinc(void);
void WellcomeLights(void);
void CdEmulatorStartCd(void);
void CdEmulatorStopCd(void);
void CdEmulatorReportStatus(void);
void CdEmulatorTrackNext(void);
void CdEmulatorTrackPrv(void);
void CdEmulatorLoadDisk(AVR_U8 disk);
void reset(void);


#endif /* FRAMEWORK_H_ */