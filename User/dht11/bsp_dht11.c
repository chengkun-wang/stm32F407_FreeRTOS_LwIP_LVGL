
#include "bsp_dht11.h"


/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_dht11.h"
#include "./dwt_delay/core_delay.h"  
#include "FreeRTOS.h"
#include "task.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
static uint8_t DHT11_ReadByte(void);

/* ������ --------------------------------------------------------------------*/
/**
  * ��������: 
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
//static void Delay_us(uint16_t time)
//{
//        uint8_t i;

//  while(time)
//  {    
//          for (i = 0; i < 10; i++)
//    {
//      
//    }
//    time--;
//  }
//}

/**
  * ��������: DHT11 ��ʼ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void DHT11_Init ( void )
{
  DHT11_Dout_GPIO_CLK_ENABLE();
  
  DHT11_Mode_Out_PP();
        
  DHT11_Dout_HIGH();  // ����GPIO
}

/**
  * ��������: ʹDHT11-DATA���ű�Ϊ��������ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void DHT11_Mode_IPU(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* �������蹦��GPIO���� */
  GPIO_InitStruct.Pin   = DHT11_Dout_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_Dout_PORT, &GPIO_InitStruct);
        
}

/**
  * ��������: ʹDHT11-DATA���ű�Ϊ�������ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void DHT11_Mode_Out_PP(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* �������蹦��GPIO���� */
  GPIO_InitStruct.Pin = DHT11_Dout_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT11_Dout_PORT, &GPIO_InitStruct);          
}


/**
  * ��������: ��DHT11��ȡһ���ֽڣ�MSB����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static uint8_t DHT11_ReadByte ( void )
{
    uint8_t i, temp=0;
    uint16_t count;
    for(i=0;i<8;i++)    
    {         
        /*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/  
        while(DHT11_Data_IN()==GPIO_PIN_RESET)
        {
          count++;
          if(count>1000)  
            return 0;
          Delay_us(1); 
        }

        /*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
         *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
         */
        Delay_us(50); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��                     
        count = 0;
        if(DHT11_Data_IN()==GPIO_PIN_SET)/* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
        {
              /* �ȴ�����1�ĸߵ�ƽ���� */
              while(DHT11_Data_IN()==GPIO_PIN_SET)
              {
                count++;
                if(count>1000)  
                  return 0;
                Delay_us(1); 
              }
              count = 0;
              temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ��1��MSB���� 
        }
        else         // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
        {                           
           temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ��0��MSB����
        }
    }
    return temp;
}


/*
 * 
 * 
 */
/**
  * ��������: һ�����������ݴ���Ϊ40bit����λ�ȳ�
  * �������: DHT11_Data:DHT11��������
  * �� �� ֵ: ERROR��  ��ȡ����
  *           SUCCESS����ȡ�ɹ�
  * ˵    ����8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
  */
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{  
  uint8_t temp;
  uint16_t humi_temp;
  uint16_t count;

  /*���ģʽ*/
  DHT11_Mode_Out_PP();
  /*��������*/
  DHT11_Dout_LOW();
  /*��ʱ18ms*/
  Delay_ms(18);
  
  /*�������� ������ʱ30us*/
  DHT11_Dout_HIGH(); 

  Delay_us(30);   //��ʱ30us

  /*������Ϊ���� �жϴӻ���Ӧ�ź�*/ 
  DHT11_Mode_IPU();

//  /*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������*/   
  if(DHT11_Data_IN()==GPIO_PIN_RESET)
  {
    count=0;
    /*��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���*/  
    while(DHT11_Data_IN()==GPIO_PIN_RESET)
    {
      count++;
      if(count>100)  
        return 0;
      Delay_us(1); 
    }
    count=0;
    /*��ѯֱ���ӻ������� 80us �ߵ�ƽ�����źŽ���*/
    while(DHT11_Data_IN()==GPIO_PIN_SET)
    {
      count++;
      if(count>100)  
        return 0;
      Delay_us(1); 
    }
    
    /*��ʼ��������*/   
    DHT11_Data->humi_high8bit= DHT11_ReadByte();
    DHT11_Data->humi_low8bit = DHT11_ReadByte();
    DHT11_Data->temp_high8bit= DHT11_ReadByte();
    DHT11_Data->temp_low8bit = DHT11_ReadByte();
    DHT11_Data->check_sum    = DHT11_ReadByte();

    /* �����ݽ��д��� */
    humi_temp=DHT11_Data->humi_high8bit*100+DHT11_Data->humi_low8bit;
    DHT11_Data->humidity =(double)humi_temp/100;
    
    humi_temp=DHT11_Data->temp_high8bit*100+DHT11_Data->temp_low8bit;
    DHT11_Data->temperature=(double)humi_temp/100;    
    
    /*����ȡ�������Ƿ���ȷ*/
    temp = DHT11_Data->humi_high8bit + DHT11_Data->humi_low8bit + 
           DHT11_Data->temp_high8bit+ DHT11_Data->temp_low8bit;
    
    if(DHT11_Data->check_sum==temp)
    { 
      return SUCCESS;
    }
    else 
      return ERROR;
  }        
  else
    return ERROR;
}


