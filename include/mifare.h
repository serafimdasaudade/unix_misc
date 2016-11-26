

int MifareGetCardId(FILE *fplog, SCARDHANDLE hCard, BYTE cardid[], int *size);
int MifareClassicRead(FILE *fplog, SCARDHANDLE hCard, WORD address, BYTE data[], int size, const BYTE key_value[6]);
LONG MifareClassicWrite(FILE *fplog, SCARDHANDLE hCard, BYTE block, const BYTE data[], int size, const BYTE key_value[6]);
int scard_mifare_read_sector(FILE *fplog, SCARDCONTEXT hContext, const char *szReader,int sector, unsigned char *key, unsigned char *sector_data, int *sector_data_len);
int scard_mifare_write_block(FILE *fplog, SCARDCONTEXT hContext, const char *szReader,int block, unsigned char *key, unsigned char *_data, int data_len);
int mobie_mifare_read_sector(FILE *fplog, SCARDCONTEXT hContext, const char *szReader,int sector, unsigned char *sector_data, int *sector_data_len);
void mobie_mifare_parse_pin_from_sector_15_data(FILE *fplog, unsigned char *sector_data, int *pin);
void mobie_mifare_parse_cardid_from_sector_0_data(FILE *fplog, unsigned char *sector_data, unsigned char *cardid);
int mobie_mifare_write_pin(FILE *fplog, SCARDCONTEXT hContext, const char *szReader,unsigned int new_pin);
int mobie_mifare_write_data(FILE *fplog, SCARDCONTEXT hContext, const char *szReader,char sdata[4]);
int mobie_mifare_write_new_id(FILE *fplog, SCARDCONTEXT hContext, const char *szReader, char *sdata);
