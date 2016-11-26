#ifndef MC_SCARD_H
#define MC_SCARD_H

#include "log.h"

/*
#include <winscard.h>
*/
#include "PCSC_INC/winscard.h"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib,"winscard.lib")
#endif

#define MC_TRUE 1
#define MC_FALSE 0

#define KEY_INITIAL	1
#define KEY_MOBIE	2

extern const unsigned char CALYPSO_APPLICATION[];
extern const unsigned char ENVIRONMENT_FILE[];
extern const unsigned char CONTRACTS_FILE[];
extern const unsigned char COUNTERS_FILE[];
extern const unsigned char EVENTS_FILE[];
extern const unsigned char SPECIAL_EVENT_FILE[];
extern const unsigned char CONTRACT_LIST_FILE[];

struct calypsoAtributes
{
	unsigned char	raw[1000];
	int		raw_len;
};

struct calypsoFCI
{
	unsigned char	raw[1000];
	int		raw_len;
        BYTE df_name[1000];
        BYTE serial_num[1000];
        BYTE session_max_mods;
        BYTE platform;
        BYTE type;
        BYTE subtype;
        BYTE softissuer;
        BYTE softversion;
        BYTE softrevision;
};

struct cardEnvironment
{
	unsigned char	raw[1000];
	int		raw_len;
        unsigned char	bitmap;
        unsigned char	version; 
        unsigned char	app_issuer;
        unsigned short	network_country; 
        unsigned char	network_ident; 
        unsigned char	end_date;
};

struct cardContract
{
	unsigned char	raw[1000];
	int		raw_len;
};

struct cardContracts
{
	struct cardContract contract[10];
	int num_contracts;
};

int scard_read_file_record(FILE *fplog,const unsigned char *file, int record, char *reader_used );
int scard_startup(FILE *fplog,SCARDCONTEXT *hContext);
int scard_find_first_reader_with_a_card(FILE *fplog, SCARDCONTEXT hContext, char *reader_used);
int scard_get_mifare(FILE *fplog,SCARDCONTEXT hContext,const char *szReader, const unsigned char *file, int record );


#endif
