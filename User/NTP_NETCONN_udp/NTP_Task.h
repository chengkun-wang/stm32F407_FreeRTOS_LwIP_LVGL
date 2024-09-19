/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef _CLIENT_H
#define _CLIENT_H


#define NTP_DEMO_RX_BUFSIZE   2000  /* ����udp���������ݳ��� */
#define NTP_DEMO_PORT         123   /* ����udp���ӵı��ض˿ں� */

typedef struct _NPTformat
{
    char    version;            /* �汾�� */
    char    leap;               /* ʱ��ͬ�� */
    char    mode;               /* ģʽ */
    char    stratum;            /* ϵͳʱ�ӵĲ��� */
    char    poll;               /* ���¼�� */
    signed char  precision;     /* ���ܶ� */
    unsigned int   rootdelay;   /* ���ص����ο�ʱ��Դ������ʱ�� */
    unsigned int   rootdisp;    /* ͳʱ����������ο�ʱ�ӵ������� */
    char    refid;              /* �ο�ʶ���� */
    unsigned long long  reftime;/* �ο�ʱ�� */
    unsigned long long  org;    /* ��ʼ��ʱ��� */
    unsigned long long  rec;    /* �յ���ʱ��� */
    unsigned long long  xmt;    /* ����ʱ��� */
} NPTformat;

typedef struct /*�˽ṹ�嶨����NTPʱ��ͬ������ر���*/
{
    int  year;        /* �� */
    int  month;       /* �� */
    int  day;         /* �� */
    int  hour;        /* ʱ */
    int  minute;      /* �� */
    int  second;      /* �� */
} DateTime;

#define SECS_PERDAY     86400UL         /* һ���еļ����� = 60*60*24 */
#define UTC_ADJ_HRS     8               /* SEOUL : GMT+8��������������  */
#define EPOCH           1900            /* NTP ��ʼ��  */
//#define HOST_NAME  "ntp1.aliyun.com"    /*������NTP���������� */


void NTP_init(void *thread_param);
#endif /* _CLIENT_H */
