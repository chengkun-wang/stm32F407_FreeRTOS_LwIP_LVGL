#ifndef _CJSON_PROCESS_H_
#define _CJSON_PROCESS_H_
#include "cJSON.h"
#include "stdint.h"


#define   ID              "id"    
#define   VERSION         "version"   
#define   TEMP        "currentTemperature"  
#define   HUM         "currentHumidity" 
#define   VALUE           "value" 


#define   DEFAULT_ID            "123"  
#define   DEFAULT_VERSION       "1.0"  

#define   DEFINE_TIME      

#define   DEFAULT_TEMP_NUM       25 
#define   DEFAULT_HUM_NUM        50 


#define   PARAMS         "params" 

#define   UPDATE_SUCCESS       1 
#define   UPDATE_FAIL          0

cJSON* cJSON_Data_Init(void);
uint8_t cJSON_Update(const cJSON * const object,const char * const string,void * d);
void Process(void* data);

//typedef struct JSON_SEND{
//	char* ID;
//	char* VERSION;
//}JSON_SEND_DATA;

//typedef struct CURRENT_VALUE
//{
//	
//}CURRENT_DETECTION_VALUE;

#endif

//{
//"id": "123",
//"version": "1.0",
//"params": {
//"currentHumidity": {
//"value": 99
//},
//"currentTemperature": {
//"value": 79
//}
//}
//}
