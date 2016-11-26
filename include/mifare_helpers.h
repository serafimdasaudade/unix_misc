#ifndef __MIFARE_HELPERS_H__
#define __MIFARE_HELPERS_H__

typedef enum
{
  RC_TRUE,
  RC_FALSE,

  RC_PARAMETER_ERROR,
  RC_ACCEPTABLE_ERROR,
  RC_FATAL_ERROR,

} RC_T;

/* Interpreted APDUs defined by the PC/SC standard */
/* ----------------------------------------------- */

RC_T SCardLoadKey(SCARDHANDLE hCard, BYTE key_location, BYTE key_index, const BYTE key_value[], BYTE key_size);
RC_T SCardGeneralAuthenticate(SCARDHANDLE hCard, WORD address, BYTE key_location, BYTE key_index);
RC_T SCardReadBinary(SCARDHANDLE hCard, WORD address, BYTE data[], BYTE size);
RC_T SCardUpdateBinary(SCARDHANDLE hCard, WORD address, const BYTE data[], BYTE size);

/* Interpreted APDUs out of the PC/SC standard */
/* ------------------------------------------- */

RC_T SCardMifareClassicRead_Auto(SCARDHANDLE hCard, WORD address, BYTE data[], BYTE size);
RC_T SCardMifareClassicRead_KeyIndex(SCARDHANDLE hCard, WORD address, BYTE data[], BYTE size, BYTE key_location, BYTE key_index);
RC_T SCardMifareClassicRead_KeyValue(SCARDHANDLE hCard, WORD address, BYTE data[], BYTE size, const BYTE key_value[6]);

RC_T SCardMifareClassicWrite_Auto(SCARDHANDLE hCard, WORD address, const BYTE data[], BYTE size);
RC_T SCardMifareClassicWrite_KeyIndex(SCARDHANDLE hCard, WORD address, const BYTE data[], BYTE size, BYTE key_location, BYTE key_index);
RC_T SCardMifareClassicWrite_KeyValue(SCARDHANDLE hCard, WORD address, const BYTE data[], BYTE size, const BYTE key_value[6]);

/* Mifare helper */
/* ------------- */

void MakeMifareSectorTrailer(BYTE trailer[16], const BYTE key_a[6], const BYTE key_b[6], const BYTE ac[4]);

/* Test helpers */
/* ------------ */

RC_T SCardLoadKeys(SCARDHANDLE hCard);
RC_T SCardMifareIsTransport(SCARDHANDLE hCard, BYTE sectors);
RC_T SCardMifareFormatTransport(SCARDHANDLE hCard, BYTE sectors);
RC_T SCardMifareFormatUsage(SCARDHANDLE hCard, BYTE sectors);

BOOL MifareTest_Standard_Blocks(SCARDHANDLE hCard, BYTE sectors, BOOL transport);
BOOL MifareTest_Standard_Sectors(SCARDHANDLE hCard, BYTE sectors, BOOL transport);

BOOL MifareTest_Specific_Blocks_A(SCARDHANDLE hCard, BYTE sectors, BOOL transport);
BOOL MifareTest_Specific_Sectors_A(SCARDHANDLE hCard, BYTE sectors, BOOL transport);

BOOL MifareTest_Specific_Blocks_K(SCARDHANDLE hCard, BYTE sectors, BOOL transport);
BOOL MifareTest_Specific_Sectors_K(SCARDHANDLE hCard, BYTE sectors, BOOL transport);

BOOL MifareTest_Specific_Blocks_I(SCARDHANDLE hCard, BYTE sectors, BOOL transport);
BOOL MifareTest_Specific_Sectors_I(SCARDHANDLE hCard, BYTE sectors, BOOL transport);

#endif
