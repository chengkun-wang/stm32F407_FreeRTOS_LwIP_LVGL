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
  * 实验平台:野火  STM32全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
#include "main.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "cJSON_Process.h"
#include "bsp_dht11.h"
#include "bsp_debug_usart.h"
/* lvgl头文件 */
#include "lvgl.h" // 它为整个LVGL提供了更完整的头文件引用
#include "lv_port_disp.h" // LVGL的显示支持
#include "lv_port_indev.h" // LVGL的触屏支持

#include "mqttclient.h" //MQTT发送任务
#include "NTP_Task.h" //NTP阿里云服务器时间
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
static TaskHandle_t DHT11_Task_Handle = NULL;/* LED任务句柄 */
static TaskHandle_t Test2_Task_Handle = NULL;/* KEY任务句柄 */
static TaskHandle_t NTP_Task_Handle = NULL; /*获取NTP时间任务句柄*/
/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */
 
QueueHandle_t MQTT_DHT11_Data_Queue =NULL;
QueueHandle_t LVGL_DHT11_Data_Queue =NULL;


/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
DHT11_Data_TypeDef DHT11_Data;

/******************************* 宏定义 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些宏定义。
 */
#define  MQTT_DHT11_QUEUE_LEN    4   /* 队列的长度，最大可包含多少个消息 */
#define  MQTT_DHT11_QUEUE_SIZE   4   /* 队列中每个消息大小（字节） */

#define  LVGL_DHT11_QUEUE_LEN    4
#define  LVGL_DHT11_QUEUE_SIZE   4
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */

static void DHT11_Task(void* pvParameters);/* Test1_Task任务实现 */
static void Test2_Task(void* pvParameters);/* Test1_Task任务实现 */

extern void TCPIP_Init(void);


/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  /* 开发板硬件初始化 */
  BSP_Init();
	lv_init(); // LVGL 初始化
	lv_port_disp_init(); // 注册LVGL的显示任务
	lv_port_indev_init(); // 注册LVGL的触屏检测任务
  /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
  else
    return -1;  
  
  while(1);   /* 正常不会执行到这里 */    
}


/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
 
  /* 创建Test_Queue */
  MQTT_DHT11_Data_Queue = xQueueCreate((UBaseType_t ) MQTT_DHT11_QUEUE_LEN,/* 消息队列的长度 */
                                 (UBaseType_t ) MQTT_DHT11_QUEUE_SIZE);/* 消息的大小 */
  if(NULL != MQTT_DHT11_Data_Queue)
    PRINT_DEBUG("创建MQTT_DHT11_Data_Queue消息队列成功!\r\n");
  
  /* 创建LVGL_DHT11_Data_Queue */
  LVGL_DHT11_Data_Queue = xQueueCreate((UBaseType_t ) LVGL_DHT11_QUEUE_LEN,/* 消息队列的长度 */
                                 (UBaseType_t ) LVGL_DHT11_QUEUE_SIZE);/* 消息的大小 */
  if(NULL != LVGL_DHT11_Data_Queue)
    PRINT_DEBUG("创建LVGL_DHT11_Data_Queue消息队列成功!\r\n");
  
  
  TCPIP_Init();
  
  mqtt_thread_init();
  
  
  taskENTER_CRITICAL();           //进入临界区
 
  /* 创建Test1_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )DHT11_Task, /* 任务入口函数 */
                        (const char*    )"DHT11_Task",/* 任务名字 */
                        (uint16_t       )1024,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&DHT11_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    PRINT_DEBUG("Create DHT11_Task sucess...\r\n");
  
  /* 创建Test2_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Test2_Task,  /* 任务入口函数 */
                        (const char*    )"Test2_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )3, /* 任务的优先级 */
                        (TaskHandle_t*  )&Test2_Task_Handle);/* 任务控制块指针 */ 
  if(pdPASS == xReturn)
    PRINT_DEBUG("Create Test2_Task sucess...\n\n");
  
  xReturn = xTaskCreate((TaskFunction_t )NTP_init,  /* 任务入口函数 */
                        (const char*    )"NTP_init",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )4, /* 任务的优先级 */
                        (TaskHandle_t*  )&NTP_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    PRINT_DEBUG("Create NTP_init sucess...\n\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}



/**********************************************************************
  * @ 函数名  ： Test1_Task
  * @ 功能说明： Test1_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void DHT11_Task(void* parameter)
{	
  uint8_t res;
  //系统延时初始化
  CPU_TS_TmrInit();
  //DHT11初始化
  DHT11_Init();
  DHT11_Data_TypeDef* send_data;
  while (1)
  {
    taskENTER_CRITICAL();           //进入临界区
    res = DHT11_Read_TempAndHumidity(&DHT11_Data);
    taskEXIT_CRITICAL();            //退出临界区
    send_data = &DHT11_Data;
    if(SUCCESS == res)
    {
      xQueueSend( MQTT_DHT11_Data_Queue, /* 消息队列的句柄 */
                            &send_data,/* 发送的消息内容 */
                            0 );        /* 等待时间 0 */
		
	  xQueueSend( LVGL_DHT11_Data_Queue, /* 消息队列的句柄 */
                            &send_data,/* 发送的消息内容 */
                            0 );        /* 等待时间 0 */
    }

    vTaskDelay(8000);/* 延时3000个tick */
  }
}

/**********************************************************************
  * @ 函数名  ： Test2_Task
  * @ 功能说明： Test2_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Test2_Task(void* parameter)
{	 
	    // 按钮
    lv_obj_t *myBtn = lv_btn_create(lv_scr_act());                               // 创建按钮; 父对象：当前活动屏幕
    lv_obj_set_pos(myBtn, 10, 10);                                               // 设置坐标
    lv_obj_set_size(myBtn, 120, 50);                                             // 设置大小

    // 按钮上的文本
    lv_obj_t *label_btn = lv_label_create(myBtn);                                // 创建文本标签，父对象：上面的btn按钮
    lv_obj_align(label_btn, LV_ALIGN_CENTER, 0, 0);                              // 对齐于：父对象
    lv_label_set_text(label_btn, "Test");                                        // 设置标签的文本

    // 独立的标签
    lv_obj_t *myLabel = lv_label_create(lv_scr_act());                           // 创建文本标签; 父对象：当前活动屏幕
    lv_label_set_text(myLabel, "Hello world!");                                  // 设置标签的文本
    lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 0);                                // 对齐于：父对象
    lv_obj_align_to(myBtn, myLabel, LV_ALIGN_OUT_TOP_MID, 0, -20);               // 对齐于：某对象

  while (1)
  {
//    PRINT_DEBUG("LED2_TOGGLE\n");
	lv_task_handler();
    vTaskDelay(5);/* 延时2000个tick */
  }
}



/********************************END OF FILE****************************/
