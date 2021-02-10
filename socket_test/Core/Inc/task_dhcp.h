/*
 * task_dhcp.h
 *
 *  Created on: Feb 9, 2021
 *      Author: pawel
 */

#ifndef INC_TASK_DHCP_H_
#define INC_TASK_DHCP_H_

#include "stm32f429xx.h"

/* DHCP process states */
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5


/* Static IP */
#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 8
#define IP_ADDR3 10

#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 255
#define NETMASK_ADDR3 0

#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 8
#define GW_ADDR3 1



void StartDhcpTask();
int GetDhcpState(void);



#endif /* INC_TASK_DHCP_H_ */
