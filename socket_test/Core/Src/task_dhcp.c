/*
 * task_dhcp.c
 *
 *  Created on: Feb 9, 2021
 *      Author: pawel
 */

#include "task_dhcp.h"
#include "cmsis_os.h"
#include "dhcp.h"
#include "ip_addr.h"
#include "ip4_addr.h"
#include "main.h"
#include "task_uart.h"

#define MAX_DHCP_TRIES  4
__IO uint8_t DHCP_state;

extern struct netif gnetif;

osThreadId_t dhcpTaskHandle;
const osThreadAttr_t dhcpTask_attributes = { .name = "dhcpTask", .priority =
		(osPriority_t) osPriorityNormal, .stack_size = 1024 * 4 };

void Dhcp_thread(void *argument);
void Dhcp_process(void const *argument);

void StartDhcpTask() {
	dhcpTaskHandle = osThreadNew(Dhcp_thread, NULL, &dhcpTask_attributes);
}

void Dhcp_thread(void *argument) {
	while (1) {
		Dhcp_process(&gnetif);

	}
}

void Dhcp_process(void const *argument) {
	struct netif *netif = (struct netif*) argument;
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;
	struct dhcp *dhcp;

	UartDbg("Starting DHCP...\n\r");
	if (netif_is_up(netif)){
	    DHCP_state = DHCP_START;
		UartDbg("Ethernet link up!\n\r");
	}
	else {
	    DHCP_state = DHCP_LINK_DOWN;
		UartDbg("Ethernet link down!\n\r");
	}

	for (;;) {
		switch (DHCP_state) {
		case DHCP_START : {
			UartDbg("Waiting for IP address...\n\r");
			ip_addr_set_zero_ip4(&netif->ip_addr);
			ip_addr_set_zero_ip4(&netif->netmask);
			ip_addr_set_zero_ip4(&netif->gw);
			dhcp_start(netif);
			DHCP_state = DHCP_WAIT_ADDRESS;
		}
			break;
		case DHCP_WAIT_ADDRESS : {
			if (dhcp_supplied_address(netif)) {
				DHCP_state = DHCP_ADDRESS_ASSIGNED;
				uint32_t temp_ip = dhcp->offered_ip_addr.addr;
				UartDbg("IP address assigned by a DHCP server: [%03d.%03d.%03d.%03d]\n\r",
					temp_ip&0xFF, (temp_ip>>8)&0xFF, (temp_ip>>16)&0xFF, (temp_ip>>24)&0xFF);
			} else {
				dhcp = (struct dhcp*) netif_get_client_data(netif,
						LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
				/* DHCP timeout */
				if (dhcp->tries > MAX_DHCP_TRIES) {
					DHCP_state = DHCP_TIMEOUT;
					/* Stop DHCP */
					dhcp_stop(netif);
					/* Static address used */
					IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
					IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1,
							NETMASK_ADDR2, NETMASK_ADDR3);
					IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
					netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),
							ip_2_ip4(&gw));
					UartDbg("Timeout, set to static IP address [%03d.%03d.%03d.%03d]\n\r",IP_ADDR0,
							IP_ADDR1, IP_ADDR2, IP_ADDR3);
				}
			}
		}
			break;
		case DHCP_LINK_DOWN : {
			/* Stop DHCP */
			dhcp_stop(netif);
			DHCP_state = DHCP_OFF;
		}
			break;
		default:
			break;
		}
		/* wait 250 ms */
		osDelay(250);
	}
}


int GetDhcpState(void){
	return DHCP_state;
}
