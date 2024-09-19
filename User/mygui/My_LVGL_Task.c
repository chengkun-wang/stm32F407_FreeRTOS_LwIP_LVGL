#include "FreeRTOS.h"
#include "queue.h"
#include "lvgl.h"
#include "mqttclient.h"
#include "bsp_dht11.h"
#include "My_LVGL_Task.h"
#include "NTP_Task.h"
#include "home_page.h"
extern QueueHandle_t LVGL_DHT11_Data_Queue;
extern DateTime g_nowdate;  

#define DHT11_BUF_SIZE 60   
char   DHT11_Buffer[DHT11_BUF_SIZE];  

#define  NTP_BUFFER_SIZE 100
char   ntp_time_buf[NTP_BUFFER_SIZE];

extern lv_obj_t* Page_1;
extern lv_obj_t* Page_2;

//lv_obj_t* Home_Page_tileview;
//lv_obj_t* lv_label1;
//lv_obj_t* lv_label2;
//lv_obj_t* lv_label3;

void Update_DHT11(void)
{
	DHT11_Data_TypeDef* recv_data;
	BaseType_t xReturn = pdPASS;
	xReturn = xQueueReceive( LVGL_DHT11_Data_Queue,    /* ��Ϣ���еľ�� */
                             &recv_data,      /* ���͵���Ϣ���� */
                             3000); /* �ȴ�ʱ�� 3000ms */
      if(xReturn == pdTRUE)
      {
		snprintf(DHT11_Buffer, DHT11_BUF_SIZE, "temperature = %f, humidity = %f", roundToTwoDecimals(recv_data->temperature), roundToTwoDecimals(recv_data->humidity));
	  }
}

void Update_Current_Time(void)
{
	snprintf(ntp_time_buf, NTP_BUFFER_SIZE, "Beijing time:%02d-%02d-%02d %02d:%02d:%02d", g_nowdate.year, 
																							 g_nowdate.month + 1,
																							 g_nowdate.day,
																							 g_nowdate.hour + 8,
																							 g_nowdate.minute,
																							 g_nowdate.second);
}





/**********************************************************************
  * @ ������  �� My_LVGL_Task
  * @ ����˵���� My_LVGL_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
void My_LVGL_Task(void* parameter)
{	 
	Home_Page();
	lv_obj_t* lv_label1 = lv_label_create(Page_1);
    lv_obj_t* lv_label2 = lv_label_create(Page_1);
    lv_obj_t* lv_label3 = lv_label_create(Page_1);
	lv_obj_align(lv_label1,LV_ALIGN_TOP_MID,0,0);
	lv_obj_align(lv_label2,LV_ALIGN_CENTER,0,0);
	lv_obj_align(lv_label3,LV_ALIGN_BOTTOM_MID,0,0);
	for(;;)
	{
		Update_DHT11();
		lv_label_set_text(lv_label1,"Hello");
        lv_label_set_text(lv_label2, DHT11_Buffer);
		Update_Current_Time();
		lv_label_set_text(lv_label3, ntp_time_buf);
		lv_task_handler();
		vTaskDelay(5);
	}
}