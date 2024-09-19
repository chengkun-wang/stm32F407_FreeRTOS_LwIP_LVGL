#include "cJSON_Process.h"
#include "main.h"

/*******************************************************************
 *                          ��������                               
 *******************************************************************/



cJSON* cJSON_Data_Init(void)
{
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
  
    cJSON* cJSON_Root = cJSON_CreateObject();    // json���ڵ�  
    cJSON* cJSON_Params = cJSON_CreateObject();  
    cJSON* cJSON_CURRENT_Temperature = cJSON_CreateObject();  
    cJSON* cJSON_CURRENT_Humidity = cJSON_CreateObject();  
  
    if (NULL == cJSON_Root || NULL == cJSON_Params || NULL == cJSON_CURRENT_Temperature || NULL == cJSON_CURRENT_Humidity) {  
        // �����ѷ������Դ  
        cJSON_Delete(cJSON_Root);  
        cJSON_Delete(cJSON_Params);  
        cJSON_Delete(cJSON_CURRENT_Temperature);  
        cJSON_Delete(cJSON_CURRENT_Humidity);  
        return NULL;  
    }  
  
    // ���JSON����  
    cJSON_AddStringToObject(cJSON_Root, ID, DEFAULT_ID);  
    cJSON_AddStringToObject(cJSON_Root, VERSION, DEFAULT_VERSION);  
  
    cJSON_AddNumberToObject(cJSON_CURRENT_Humidity, VALUE, DEFAULT_HUM_NUM);  
    cJSON_AddItemToObject(cJSON_Params, HUM, cJSON_CURRENT_Humidity);  
  
    cJSON_AddNumberToObject(cJSON_CURRENT_Temperature, VALUE, DEFAULT_TEMP_NUM);  
    cJSON_AddItemToObject(cJSON_Params, TEMP, cJSON_CURRENT_Temperature);  
  
    cJSON_AddItemToObject(cJSON_Root, PARAMS, cJSON_Params); 
 
  
  char* p = cJSON_Print(cJSON_Root);  /*p ָ����ַ�����json��ʽ��*/
  

  printf("%s\n",p);
  
  vPortFree(p);
  p = NULL;
  
  return cJSON_Root;
  
}
uint8_t cJSON_Update(const cJSON * const object,const char * const string,void *d)
{
  cJSON* node = NULL;    //json���ڵ�
  node = cJSON_GetObjectItem(object,string);
  if(node == NULL)
    return NULL;
  if(cJSON_IsBool(node))
  {
    int *b = (int*)d;
//    printf ("d = %d",*b);
    cJSON_GetObjectItem(object,string)->type = *b ? cJSON_True : cJSON_False;
//    char* p = cJSON_Print(object);    /*p ָ����ַ�����json��ʽ��*/
    return 1;
  }
  else if(cJSON_IsString(node))
  {
    cJSON_GetObjectItem(object,string)->valuestring = (char*)d;
//    char* p = cJSON_Print(object);    /*p ָ����ַ�����json��ʽ��*/
    return 1;
  }
  else if(cJSON_IsNumber(node))
  {
    double *num = (double*)d;
//    printf ("num = %f",*num);
//    cJSON_GetObjectItem(object,string)->valueint = (double)*num;
    cJSON_GetObjectItem(object,string)->valuedouble = (double)*num;
//    char* p = cJSON_Print(object);    /*p ָ����ַ�����json��ʽ��*/
    return 1;
  }
  else
    return 1;
}

void Process(void* data)
{
  PRINT_DEBUG("��ʼ����JSON����");
  cJSON *root;
  double temp_num,hum_num;
  root = cJSON_Parse((char*)data); //������json��ʽ
  
    
    cJSON *params = cJSON_GetObjectItemCaseSensitive(root, PARAMS);  
    if (params != NULL) {  
        cJSON *currentHumidity = cJSON_GetObjectItemCaseSensitive(params, "currentHumidity"); 
//		printf("#########722\n");
        if (currentHumidity != NULL) {  
            cJSON *value = cJSON_GetObjectItemCaseSensitive(currentHumidity, "value");  
//			printf("#########725\n");
            if (value != NULL && cJSON_IsNumber(value)) {  
                temp_num = value->valuedouble;
//				printf("#########728\n");
            }  
        }

		 // ���Ҳ�����currentTemperature��value   
    }  
	cJSON *currentTemperature = cJSON_GetObjectItemCaseSensitive(params, "currentTemperature");  
    if (currentTemperature != NULL) {  
        cJSON *value = cJSON_GetObjectItemCaseSensitive(currentTemperature, "value");  
        if (value != NULL && cJSON_IsNumber(value)) {  
            hum_num = value->valuedouble;
        }  
    }
  


  PRINT_DEBUG("temp_num:%f,hum_num:%f\n",
              temp_num, hum_num);

  cJSON_Delete(root);  //�ͷ��ڴ� 
}








