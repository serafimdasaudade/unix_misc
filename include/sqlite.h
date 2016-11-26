#include <sqlite3.h>

void *open_sqlite(FILE *fplog,char *name);
void close_sqlite(FILE *fplog,void *mobiedb);
void exec_sqlite(FILE *fplog,void *mobiedb, char*sqlcommand);

//int mobieCardAuthorization(char *stationId, char *cardNum, struct sessionData *stSessionData,char **sErrorResponse);
//int mobieChargeStart(char *satId, struct sessionData *stSessionData, char **sErrorResponse);
//int mobieChargeStop(char *satId, int sEndChargeReason, float sEnergySupplied, struct sessionData *stSessionData,char **sErrorResponse);
