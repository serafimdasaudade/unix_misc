#include "test_reader.h"

/*
const unsigned char CALYPSO_APPLICATION[] = "1TIC.ICA";
const unsigned char ENVIRONMENT_FILE[]    = { 0x20, 0x01 ,0x00};
const unsigned char CONTRACTS_FILE[]      = { 0x20, 0x20 ,0x00};
const unsigned char COUNTERS_FILE[]       = { 0x20, 0x69 ,0x00};
const unsigned char EVENTS_FILE[]         = { 0x20, 0x10 ,0x00};
const unsigned char SPECIAL_EVENT_FILE[]  = { 0x20, 0x40 ,0x00};
const unsigned char CONTRACT_LIST_FILE[]   = { 0x20, 0x50 ,0x00};
*/

FILE *fplog;

main()
{

	int alen;
	unsigned char atrib[1000];
	char stratrib[1000];

	scard_read_atrib(NULL, atrib ,&alen);
	bin_to_hex_str(atrib,alen,stratrib);

	log_printf(NULL,LOG_INFO,"ATRIB:%s",stratrib);

}
