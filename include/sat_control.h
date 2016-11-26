
/*Deprecating waiting for kiosk*/
#define MC_SAT_CONTROL_TCP_PORT 7070
#define MC_SAT_PIPE_USER_MSG "CTRLFILES/satfifo" 

#define SAT_MSG_START 0
#define SAT_MSG_END 2
#define SAT_MSG_IDLE 1

#define MAX_CHARGERS_ON_ONE_FRIENDLY_ARM	4

#define CMD_START	0x10 /* This is base, 0x11=unit 1, 0x12=unit 2, etc */
#define CMD_STOP	0x20 /* This is base, 0x21=unit 1, 0x22=unit 2, etc */
#define RESP_START	0x30 /* This is base, 0x31=unit 1, 0x32=unit 2, etc */
#define RESP_STOP	0x40 /* This is base, 0x41=unit 1, 0x42=unit 2, etc */

/*25 Mar 2012  - Luis acho que nao estamos a usar estes se descobrires que alguma coisa nao compila avisa-me
#define SAT_SELECT	0x50
#define SAT_UNSELECT	0x60
*/

#define CMD_STATUS	0x70 /* This is base, 0x71=unit 1, 0x72=unit 2, etc */
#define RESP_STATUS	0x80 /* This is base, 0x81=unit 1, 0x82=unit 2, etc */
#define CMD_START_FC_LOCK_ONLY	0x90 /* This is base, 0x91=unit 1, 0x92=unit 2, etc */
#define RESP_START_FC_LOCK_ONLY	0xA0 /* This is base, 0x91=unit 1, 0x92=unit 2, etc */
#define CMD_START_FC_UNLOCK_ONLY	0xB0 /* This is base, 0xA1=unit 1, 0xA2=unit 2, etc */
#define RESP_START_FC_UNLOCK_ONLY	0xC0 /* This is base, 0xA1=unit 1, 0xA2=unit 2, etc */

#define CMD_BEGIN  0xAF
#define BYTE1_CMD(b) ((b)&0xF0)
#define BYTE1_SAT(b) ((b)&0x0F)

#define START_CHARGE_OK 0xFF
#define START_CHARGE_DOOROPEN_ERR 0xEE
#define START_CHARGE_BAD_SATNUM_ERR 0xEF
#define START_CHARGE_FC_LOCK_ONLY_OK 0xD0;
#define START_CHARGE_FC_UNLOCK_ONLY_OK 0xD1;
#define START_CHARGE_FC_LOCK_ONLY_NOT_OK 0xD2;
#define START_CHARGE_FC_UNLOCK_ONLY_NOT_OK 0xD3;
#define START_CHARGE_INTERNAL_ERR 0x00

#define CMD_SAT_SELECT			{0xAF, SAT_SELECT}
#define CMD_SAT_UNSELECT		{0xAF, SAT_UNSELECT}

/*
	These are used when power modules are shared among 2 kiosks to lock vehicle while power modules are not available
*/
#define RSP_START_CHRG_FC_LOCK_ONLY_OK		{0xAF, RESP_START_FC_LOCK_ONLY, 0x01, 0xFF}
#define RSP_START_CHRG_FC_LOCK_ONLY_NOT_OK	{0xAF, RESP_START_FC_LOCK_ONLY, 0x01, 0x00}

#define RSP_START_CHRG_FC_UNLOCK_ONLY_OK	{0xAF, RESP_START_FC_UNLOCK_ONLY, 0x01, 0xFF}
#define RSP_START_CHRG_FC_UNLOCK_ONLY_NOT_OK	{0xAF, RESP_START_FC_UNLOCK_ONLY, 0x01, 0x00}


#define CMD_START_CHRG			{0xAF, CMD_START}
#define RSP_START_CHRG_OK		{0xAF, RESP_START, 0x01, 0xFF}
#define RSP_START_CHRG_DOOROPEN_ERR	{0xAF, RESP_START, 0x01, 0xEE}
#define RSP_START_CHRG_BAD_SATNUM_ERR	{0xAF, RESP_START, 0x01, 0xEF}
#define RSP_START_CHRG_INTERNAL_ERR	{0xAF, RESP_START, 0x01, 0x00}
	/* Resp in Fast Charger
		MSB first
		4bytes=Energy
		2bytes=Voltage
		1byte=Current Vehicle Requesting
		1byte=Current Real
		1byte=%battery
		1byte=charge status
		1byte=vehicle_status
		1byte=vehicle_fault_flag
	 */
#define RSP_STATUS_CHRG  {0xAF, RESP_STATUS, 17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} 

#define CMD_STOP_CHRG			{0xAF, CMD_STOP}
#define RSP_STOP_CHRG_OK		{0xAF, RESP_STOP, 0x01, 0xDD}
#define RSP_STOP_CHRG_BAD_SATNUM_ERR	{0xAF, RESP_STOP, 0x01, 0xEF}
#define RSP_STOP_CHRG_INTERNAL_ERR	{0xAF, RESP_STOP, 0x01, 0x00}

#define CMD_STATUS_CHRG			{0xAF, CMD_STATUS}

#define ADD_SAT_NUM(var,satnum)		var[1]+=satnum 


/* FAST CHARGE */

/* Indexes for fc_returns */
#define FC_INITIATING			1
#define FC_IN_IEBC			2
#define FC_WAIT_FIRST_VEHICLE_DATA	3
#define FC_WAIT_FOR_J_CHARGE_ENABLE	4
#define FC_IN_CLITV			5
#define FC_CHARGING			6

#define FC_NORMAL_END			20
#define FC_VEHICLE_CAN_ERROR		21
#define FC_CHARGER_CAN_ERROR		22
#define FC_CM_REQUESTED_STOP		23
#define FC_VM_REQUESTED_STOP		24
#define FC_TIAC_REQUESTED_STOP		26
#define FC_TIAC_TIMEOUT			27
#define FC_TVICB_BATTERY_FAULT		28
#define FC_NO_FIRST_CURRENT_REQ		29
#define FC_TVICB_TIMEOUT_CURRENT	30
#define FC_TVICB_VEHICLE_RELAY_CLOSE_TOO_EARLY_OR_CHARGER_10V	31
#define FC_TVICB_CONNECTOR_LOCK_FAULT_1	32
#define FC_TVICB_CONNECTOR_LOCK_FAULT_2	33
#define FC_TIMEOUT_CLRP_STOP		34
#define FC_STOP_KIOSK_RED_BUTTON	35
#define FC_STOP_BY_KIOSK		36
#define FC_STOP_BY_MT_CHARGE_VOLT_TOO_HIGH	37
#define FC_STOP_BY_MT_CHARGE_TIME	38
#define FC_STOP_BY_VEHICLE_K_J		39
#define FC_VEHICLE_ASKED_TOO_MUCH_I		40
#define FC_STOP_BY_UNKNOWN			41
#define FC_INTERNAL_ERR				42
#define FC_INSULATION_FAILURE		44

///////////////////////////
#define FC_STOP_BY_BLUE_BUTTON
////////////////////

#define FC_NOT_DISCHARGING		45
#define FC_CHARGER_MALFUNCTION		46
#define FC_VEHICLE_FAULT		47
#define FC_VEHICLE_CHARGE_ENABLE_OFF		48
#define FC_VEHICLE_SHIFT_POSITION		49
#define FC_VEHICLE_OTHER_FAULTS		50
#define FC_STOP_EARTH_FAULT		51
#define FC_STOP_EARTH_FAULT_INSUL		52
#define FC_VOLTAGE_HIGH_ON_EXIT_C		60
#define FC_VOLTAGE_HIGH_ON_EXIT_V		61

#define FC_CHRG_END_OK		{0xAF, RESP_STOP, 0x01, 0xF0}
#define FC_CHRG_INTERNAL_ERR	{0xAF, RESP_STATUS, 0x01, 0xF1}
#define FC_CHRG_CONNECTOR_NOT_IN_PLACE	{0xAF, RESP_STATUS, 0x01, 0xF2}
#define FC_CHRG_NO_RESP_ERR	{0xAF, RESP_STOP, 0x01, 0xF2}
