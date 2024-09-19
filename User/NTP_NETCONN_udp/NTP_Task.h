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


#define NTP_DEMO_RX_BUFSIZE   2000  /* 定义udp最大接收数据长度 */
#define NTP_DEMO_PORT         123   /* 定义udp连接的本地端口号 */

typedef struct _NPTformat
{
    char    version;            /* 版本号 */
    char    leap;               /* 时钟同步 */
    char    mode;               /* 模式 */
    char    stratum;            /* 系统时钟的层数 */
    char    poll;               /* 更新间隔 */
    signed char  precision;     /* 精密度 */
    unsigned int   rootdelay;   /* 本地到主参考时钟源的往返时间 */
    unsigned int   rootdisp;    /* 统时钟相对于主参考时钟的最大误差 */
    char    refid;              /* 参考识别码 */
    unsigned long long  reftime;/* 参考时间 */
    unsigned long long  org;    /* 开始的时间戳 */
    unsigned long long  rec;    /* 收到的时间戳 */
    unsigned long long  xmt;    /* 传输时间戳 */
} NPTformat;

typedef struct /*此结构体定义了NTP时间同步的相关变量*/
{
    int  year;        /* 年 */
    int  month;       /* 月 */
    int  day;         /* 天 */
    int  hour;        /* 时 */
    int  minute;      /* 分 */
    int  second;      /* 秒 */
} DateTime;

#define SECS_PERDAY     86400UL         /* 一天中的几秒钟 = 60*60*24 */
#define UTC_ADJ_HRS     8               /* SEOUL : GMT+8（东八区北京）  */
#define EPOCH           1900            /* NTP 起始年  */
//#define HOST_NAME  "ntp1.aliyun.com"    /*阿里云NTP服务器域名 */


void NTP_init(void *thread_param);
#endif /* _CLIENT_H */
