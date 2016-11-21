#include "crc16.h" 
#include <stdio.h>
#include "rs232.h"
#include "log.h"
#include <math.h>
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atoi */

#define ADDRESS		0x01
#define FUNCTION_CODE	0x03
FILE *fplog;

unsigned char buf[20];
unsigned char buffer[200];
unsigned short crc16;
int fd;

float volts_func(void);
float amps_func();
float hz_func();
float power_func();
float energy_func();
float reactive_power_func(void);
float pf_func(void);
int main(void)
{  
  
  fplog=log_start("M",LOG_DEBUG);
 
  while(1){
 
  fd=open_rs232(fplog,"/dev/ttyUSB0",9600);
  
  volts_func();
  amps_func();
  hz_func();
  power_func();
  energy_func();
  reactive_power_func();
  pf_func();
  close_rs232(fplog,fd);
  sleep(1);
 }
  log_end("M");
  //log_close_file(fplog);
  exit(0);
}

float pf_func(void)
{
  buf[0]=ADDRESS;
  buf[1]=FUNCTION_CODE;
  buf[2]=0x00;
  buf[3]=0x06;
  buf[4]=0x00;
  buf[5]=0x01;
  buf[6]='\0';
  
  crc16=crc16_ccitt(buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16_ccitt=	    0x%X\n",crc16);
  
  crc16=crc16_func((unsigned char *)buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16 modbus rtu= 0x%X\n",crc16);
  buf[6]=crc16;
  buf[7]=crc16>>8;
  buf[8]='\0';
  
  log_printf(fplog,LOG_DEBUG2,"modbus rtu frame power_factor= 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
  
  write_rs232(fplog,fd, buf,8);
  int i=0;
  while(read_rs232_timeout(NULL,fd, (unsigned char*) buf,100,200)>0){
    buffer[i]=buf[0];
    i++;
  }    
  char power_factor[10];  
  float PF;
  if(buffer[0]==ADDRESS && buffer[1]==FUNCTION_CODE)
  {
    if(buffer[2]==0x02)
    {
      sprintf(power_factor,"%X%X",buffer[3],buffer[4]);      
      log_printf(fplog,LOG_DEBUG,"power_factor =0x%s",power_factor);      
      PF=(float)(buffer[3]<<8|buffer[4])/(float)1000;
      
      log_printf(fplog,LOG_DEBUG,"power_factor =%.2f",PF);
    }
  }
}


float reactive_power_func(void)
{
  buf[0]=ADDRESS;
  buf[1]=FUNCTION_CODE;
  buf[2]=0x00;
  buf[3]=0x04;
  buf[4]=0x00;
  buf[5]=0x01;
  buf[6]='\0';
  
  crc16=crc16_ccitt(buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16_ccitt=	    0x%X\n",crc16);
  
  crc16=crc16_func((unsigned char *)buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16 modbus rtu= 0x%X\n",crc16);
  buf[6]=crc16;
  buf[7]=crc16>>8;
  buf[8]='\0';
  
  log_printf(fplog,LOG_DEBUG2,"modbus rtu frame reactive_power_func= 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
  
  write_rs232(fplog,fd, buf,8);
  int i=0;
  while(read_rs232_timeout(NULL,fd, (unsigned char*) buf,100,200)>0){
    buffer[i]=buf[0];
    i++;
  }    
  char reactive_power_func[10];  
  float Var;
  if(buffer[0]==ADDRESS && buffer[1]==FUNCTION_CODE)
  {
    if(buffer[2]==0x02)
    {
      sprintf(reactive_power_func,"%X%X",buffer[3],buffer[4]);      
      log_printf(fplog,LOG_DEBUG,"reactive_power_func =0x%s",reactive_power_func);      
      Var=(float)(buffer[3]<<8|buffer[4])/(float)10;
      
      log_printf(fplog,LOG_DEBUG,"reactive_power_func =%.2f Var",Var);
    }
  }
}

float power_func()
{
  buf[0]=ADDRESS;
  buf[1]=FUNCTION_CODE;
  buf[2]=0x00;
  buf[3]=0x03;
  buf[4]=0x00;
  buf[5]=0x01;
  buf[6]='\0';
  
  crc16=crc16_ccitt(buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16_ccitt=	    0x%X\n",crc16);
  
  crc16=crc16_func((unsigned char *)buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16 modbus rtu= 0x%X\n",crc16);
  buf[6]=crc16;
  buf[7]=crc16>>8;
  buf[8]='\0';
  
  log_printf(fplog,LOG_DEBUG2,"modbus rtu frame POWER= 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
  
  write_rs232(fplog,fd, buf,8);
  int i=0;
  while(read_rs232_timeout(NULL,fd, (unsigned char*) buf,100,200)>0){
    buffer[i]=buf[0];
    i++;
  }    
  char POWER[10];  
  float P;
  if(buffer[0]==ADDRESS && buffer[1]==FUNCTION_CODE)
  {
    if(buffer[2]==0x02)
    {
      sprintf(POWER,"%X%X",buffer[3],buffer[4]);      
      log_printf(fplog,LOG_DEBUG,"POWER =0x%s",POWER);      
      P=(float)(buffer[3]<<8|buffer[4])/(float)10;
      
      log_printf(fplog,LOG_DEBUG,"POWER =%.2f W",P);
    }
  }
}
float volts_func(void)
{
  buf[0]=ADDRESS;
  buf[1]=FUNCTION_CODE;
  buf[2]=0x00;
  buf[3]=0x00;
  buf[4]=0x00;
  buf[5]=0x01;
  buf[6]='\0';
    
  crc16=crc16_ccitt(buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16_ccitt=	    0x%X\n",crc16);
  
  crc16=crc16_func((unsigned char *)buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16 modbus rtu= 0x%X\n",crc16);
  buf[6]=crc16;
  buf[7]=crc16>>8;
  buf[8]='\0';
  
  log_printf(fplog,LOG_DEBUG2,"modbus rtu frame V L-N= 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
  
  write_rs232(fplog,fd, buf,8);
  int i=0;
  while(read_rs232_timeout(NULL,fd, (unsigned char*) buf,100,200)>0){
    buffer[i]=buf[0];
    i++;
  }    
  char V_L_N[10];  
  float V;
  if(buffer[0]==ADDRESS && buffer[1]==FUNCTION_CODE)
  {
    if(buffer[2]==0x02)
    {
      sprintf(V_L_N,"%X%X",buffer[3],buffer[4]);      
      log_printf(fplog,LOG_DEBUG,"V phase neutro =0x%s",V_L_N);      
      V=(float)(buffer[3]<<8|buffer[4])/(float)10;
      
      log_printf(fplog,LOG_DEBUG,"V phase neutro =%.2f Volts",V);
    }
  }
}

float amps_func()
{
  buf[0]=ADDRESS;
  buf[1]=FUNCTION_CODE;
  buf[2]=0x00;
  buf[3]=0x01;
  buf[4]=0x00;
  buf[5]=0x01;
  buf[6]='\0';
  
  crc16=crc16_ccitt(buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16_ccitt=	    0x%X\n",crc16);
  
  crc16=crc16_func((unsigned char *)buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16 modbus rtu= 0x%X\n",crc16);
  buf[6]=crc16;
  buf[7]=crc16>>8;
  buf[8]='\0';
  
  log_printf(fplog,LOG_DEBUG2,"modbus rtu frame Amps= 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
  
  write_rs232(fplog,fd, buf,8);
  int i=0;
  while(read_rs232_timeout(NULL,fd, (unsigned char*) buf,100,200)>0){
    buffer[i]=buf[0];
    i++;
  }    
  char Amps[10];  
  float A;
  if(buffer[0]==ADDRESS && buffer[1]==FUNCTION_CODE)
  {
    if(buffer[2]==0x02)
    {
      sprintf(Amps,"%X%X",buffer[3],buffer[4]);      
      log_printf(fplog,LOG_DEBUG,"Amps =0x%s",Amps);      
      A=(float)(buffer[3]<<8|buffer[4])/(float)10;
      
      log_printf(fplog,LOG_DEBUG,"Amps =%.2f Amps",A);
    }
  }
}

float hz_func()
{
  buf[0]=ADDRESS;
  buf[1]=FUNCTION_CODE;
  buf[2]=0x00;
  buf[3]=0x02;
  buf[4]=0x00;
  buf[5]=0x01;
  buf[6]='\0';
  
  crc16=crc16_ccitt(buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16_ccitt=	    0x%X\n",crc16);
  
  crc16=crc16_func((unsigned char *)buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16 modbus rtu= 0x%X\n",crc16);
  buf[6]=crc16;
  buf[7]=crc16>>8;
  buf[8]='\0';
  
  log_printf(fplog,LOG_DEBUG2,"modbus rtu frame Hz= 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
  
  write_rs232(fplog,fd, buf,8);
  int i=0;
  while(read_rs232_timeout(NULL,fd, (unsigned char*) buf,100,200)>0){
    buffer[i]=buf[0];
    i++;
  }    
  char Hz[10];  
  float H;
  if(buffer[0]==ADDRESS && buffer[1]==FUNCTION_CODE)
  {
    if(buffer[2]==0x02)
    {
      sprintf(Hz,"%X%X",buffer[3],buffer[4]);      
      log_printf(fplog,LOG_DEBUG,"Hz power grid =0x%s",Hz);      
      H=(float)(buffer[3]<<8|buffer[4])/(float)10;      
      log_printf(fplog,LOG_DEBUG,"Hz power grid =%.2f Hz",H);
    }
  }
}

float energy_func()
{
  buf[0]=ADDRESS;
  buf[1]=FUNCTION_CODE;
  buf[2]=0x00;
  buf[3]=0x07;
  buf[4]=0x00;
  buf[5]=0x0a;
  buf[6]='\0';
  
  crc16=crc16_func((unsigned char *)buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16 modbus rtu= 0x%X\n",crc16);
  buf[6]=crc16;
  buf[7]=crc16>>8;
  buf[8]='\0';
  
  log_printf(fplog,LOG_DEBUG2,"modbus rtu frame kWh= 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
  int i=0;
  write_rs232(fplog,fd, buf,8);
  while(read_rs232_timeout(NULL,fd, (unsigned char*) buf,100,200)>0){
    buffer[i]=buf[0];
    i++;
  }    
  char Energy[40];  
  float kWh;
  if(buffer[0]==ADDRESS && buffer[1]==FUNCTION_CODE)
  {
    if(buffer[2]==0x14)
    {
      sprintf(Energy,"%X%X%X%X",buffer[3],buffer[4],buffer[5],buffer[6]);      
      log_printf(fplog,LOG_DEBUG,"Energy =0x%s",Energy);      
      kWh=(float)(buffer[3]<<24|buffer[4]<<16|buffer[5]<<8|buffer[6])/(float)10;      
      log_printf(fplog,LOG_DEBUG,"Energy =%.2f kWh",kWh);
    }
  }
}

float serial_func()
{
  buf[0]=ADDRESS;
  buf[1]=FUNCTION_CODE;
  buf[2]=0x00;
  buf[3]=0x2b;
  buf[4]=0x00;
  buf[5]=0x01;
  buf[6]='\0';
  
  crc16=crc16_func((unsigned char *)buf, 6);
  log_printf(fplog,LOG_DEBUG2,"crc16 modbus rtu= 0x%X\n",crc16);
  buf[6]=crc16;
  buf[7]=crc16>>8;
  buf[8]='\0';
  
  log_printf(fplog,LOG_DEBUG2,"modbus rtu frame Serial= 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
  
  int i=0;
  write_rs232(fplog,fd, buf,8);
  while(read_rs232_timeout(fplog,fd, (unsigned char*) buf,100,200)>0){
    buffer[i]=buf[0];
    i++;
  }    
  
}
