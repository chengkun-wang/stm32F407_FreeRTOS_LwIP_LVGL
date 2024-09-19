/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS V9.0.0 + STM32 LwIP
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "main.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "cJSON_Process.h"
#include "bsp_dht11.h"
#include "bsp_debug_usart.h"
/* lvglͷ�ļ� */
#include "lvgl.h" // ��Ϊ����LVGL�ṩ�˸�������ͷ�ļ�����
#include "lv_port_disp.h" // LVGL����ʾ֧��
#include "lv_port_indev.h" // LVGL�Ĵ���֧��

#include "mqttclient.h" //MQTT��������
#include "NTP_Task.h" //NTP�����Ʒ�����ʱ��
/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t DHT11_Task_Handle = NULL;/* LED������ */
static TaskHandle_t Test2_Task_Handle = NULL;/* KEY������ */
static TaskHandle_t NTP_Task_Handle = NULL; /*��ȡNTPʱ��������*/
/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
 
QueueHandle_t MQTT_DHT11_Data_Queue =NULL;
QueueHandle_t LVGL_DHT11_Data_Queue =NULL;


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
DHT11_Data_TypeDef DHT11_Data;

/******************************* �궨�� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩ�궨�塣
 */
#define  MQTT_DHT11_QUEUE_LEN    4   /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define  MQTT_DHT11_QUEUE_SIZE   4   /* ������ÿ����Ϣ��С���ֽڣ� */

#define  LVGL_DHT11_QUEUE_LEN    4
#define  LVGL_DHT11_QUEUE_SIZE   4
/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void DHT11_Task(void* pvParameters);/* Test1_Task����ʵ�� */
static void Test2_Task(void* pvParameters);/* Test1_Task����ʵ�� */

extern void TCPIP_Init(void);


/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();
	lv_init(); // LVGL ��ʼ��
	lv_port_disp_init(); // ע��LVGL����ʾ����
	lv_port_indev_init(); // ע��LVGL�Ĵ����������
  /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
 
  /* ����Test_Queue */
  MQTT_DHT11_Data_Queue = xQueueCreate((UBaseType_t ) MQTT_DHT11_QUEUE_LEN,/* ��Ϣ���еĳ��� */
                                 (UBaseType_t ) MQTT_DHT11_QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
  if(NULL != MQTT_DHT11_Data_Queue)
    PRINT_DEBUG("����MQTT_DHT11_Data_Queue��Ϣ���гɹ�!\r\n");
  
  /* ����LVGL_DHT11_Data_Queue */
  LVGL_DHT11_Data_Queue = xQueueCreate((UBaseType_t ) LVGL_DHT11_QUEUE_LEN,/* ��Ϣ���еĳ��� */
                                 (UBaseType_t ) LVGL_DHT11_QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
  if(NULL != LVGL_DHT11_Data_Queue)
    PRINT_DEBUG("����LVGL_DHT11_Data_Queue��Ϣ���гɹ�!\r\n");
  
  
  TCPIP_Init();
  
  mqtt_thread_init();
  
  
  taskENTER_CRITICAL();           //�����ٽ���
 
  /* ����Test1_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )DHT11_Task, /* ������ں��� */
                        (const char*    )"DHT11_Task",/* �������� */
                        (uint16_t       )1024,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&DHT11_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    PRINT_DEBUG("Create DHT11_Task sucess...\r\n");
  
  /* ����Test2_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Test2_Task,  /* ������ں��� */
                        (const char*    )"Test2_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&Test2_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    PRINT_DEBUG("Create Test2_Task sucess...\n\n");
  
  xReturn = xTaskCreate((TaskFunction_t )NTP_init,  /* ������ں��� */
                        (const char*    )"NTP_init",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )4, /* ��������ȼ� */
                        (TaskHandle_t*  )&NTP_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    PRINT_DEBUG("Create NTP_init sucess...\n\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



/**********************************************************************
  * @ ������  �� Test1_Task
  * @ ����˵���� Test1_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void DHT11_Task(void* parameter)
{	
  uint8_t res;
  //ϵͳ��ʱ��ʼ��
  CPU_TS_TmrInit();
  //DHT11��ʼ��
  DHT11_Init();
  DHT11_Data_TypeDef* send_data;
  while (1)
  {
    taskENTER_CRITICAL();           //�����ٽ���
    res = DHT11_Read_TempAndHumidity(&DHT11_Data);
    taskEXIT_CRITICAL();            //�˳��ٽ���
    send_data = &DHT11_Data;
    if(SUCCESS == res)
    {
      xQueueSend( MQTT_DHT11_Data_Queue, /* ��Ϣ���еľ�� */
                            &send_data,/* ���͵���Ϣ���� */
                            0 );        /* �ȴ�ʱ�� 0 */
		
	  xQueueSend( LVGL_DHT11_Data_Queue, /* ��Ϣ���еľ�� */
                            &send_data,/* ���͵���Ϣ���� */
                            0 );        /* �ȴ�ʱ�� 0 */
    }

    vTaskDelay(8000);/* ��ʱ3000��tick */
  }
}

/**********************************************************************
  * @ ������  �� Test2_Task
  * @ ����˵���� Test2_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Test2_Task(void* parameter)
{	 
	    // ��ť
    lv_obj_t *myBtn = lv_btn_create(lv_scr_act());                               // ������ť; �����󣺵�ǰ���Ļ
    lv_obj_set_pos(myBtn, 10, 10);                                               // ��������
    lv_obj_set_size(myBtn, 120, 50);                                             // ���ô�С

    // ��ť�ϵ��ı�
    lv_obj_t *label_btn = lv_label_create(myBtn);                                // �����ı���ǩ�������������btn��ť
    lv_obj_align(label_btn, LV_ALIGN_CENTER, 0, 0);                              // �����ڣ�������
    lv_label_set_text(label_btn, "Test");                                        // ���ñ�ǩ���ı�

    // �����ı�ǩ
    lv_obj_t *myLabel = lv_label_create(lv_scr_act());                           // �����ı���ǩ; �����󣺵�ǰ���Ļ
    lv_label_set_text(myLabel, "Hello world!");                                  // ���ñ�ǩ���ı�
    lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 0);                                // �����ڣ�������
    lv_obj_align_to(myBtn, myLabel, LV_ALIGN_OUT_TOP_MID, 0, -20);               // �����ڣ�ĳ����

  while (1)
  {
//    PRINT_DEBUG("LED2_TOGGLE\n");
	lv_task_handler();
    vTaskDelay(5);/* ��ʱ2000��tick */
  }
}



/********************************END OF FILE****************************/
