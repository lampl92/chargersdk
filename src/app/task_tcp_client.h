/**
* @file D:\Documents\Projects\chargersdk\src\app\task_tcp_client.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-21
*/
#ifndef  __TASK_TCP_CLIENT_H
#define  __TASK_TCP_CLIENT_H

#define TCP_CLIENT_BUFSIZE   1500

extern uint8_t  tcp_client_sendbuf[TCP_CLIENT_BUFSIZE]; //TCP客户端发送数据缓冲
extern uint8_t  tcp_client_recvbuf[TCP_CLIENT_BUFSIZE]; //TCP客户端接收数据缓冲区

extern uint32_t recv_len;
extern uint32_t send_len;

#endif
