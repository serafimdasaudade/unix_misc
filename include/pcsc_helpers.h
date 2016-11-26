#ifndef __PCSC_HELPERS_H__
#define __PCSC_HELPERS_H__

#ifdef WIN32
  #include <windows.h>
#endif
#ifdef __linux__
  #include <unistd.h>
#endif

#include "PCSC_INC/winscard.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef PCSC_E_NO_READERS_AVAILABLE
  #define PCSC_E_NO_READERS_AVAILABLE  (LONG) 0x8010002E
#endif

#ifndef MAXIMUM_SMARTCARD_READERS
  #define MAXIMUM_SMARTCARD_READERS 16
#endif

#ifndef FALSE
  #define FALSE 0
#endif
#ifndef TRUE
  #define TRUE 1
#endif

typedef struct
{
  SCARDCONTEXT hContext;
	SCARDHANDLE  hCard;
	DWORD        dwProtocol;
	BYTE         cbAtr[36];
	DWORD        dwAtrLength;

} CARD_CHANNEL_T;

BOOL GetReaderList(char ***aszReaders);
void FreeReaderList(char **aszReaders);

BOOL PrintReaderStatus(const char *szReaderName);

BOOL GetReaderStatus(const char *szReaderName, DWORD *pdwStatus);
BOOL CardAvailable(const char *szReaderName);

BOOL CardConnect(const char *szReaderName, CARD_CHANNEL_T *pChannel);
BOOL CardTransmit(CARD_CHANNEL_T *pChannel, const BYTE *abCommandApdu, DWORD dwCommandApduLength, BYTE *abResponseApdu, DWORD *dwResponseApduLength);
BOOL CardDisconnect(CARD_CHANNEL_T *pChannel, DWORD dwDisposition);

#ifndef UNUSED_PARAMETER
  #define UNUSED_PARAMETER(a) (void) (a)
#endif

#endif
