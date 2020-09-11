/******************************************************************************
 * Copyright (c) 2013-2016 Realtek Semiconductor Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/


#define CONFIG_LWIP_LAYER 1

#include <lwip_intf_tizenrt.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <osdep_service.h>
#include "autoconf.h"
//----- ------------------------------------------------------------------
// External Reference
//----- ------------------------------------------------------------------
#if (CONFIG_LWIP_LAYER == 1)
extern struct netif xnetif[]; //LWIP netif
#endif

/**
 *      rltk_wlan_set_netif_info - set netif hw address and register dev pointer to netif device
 *      @idx_wlan: netif index
 *			    0 for STA only or SoftAP only or STA in STA+SoftAP concurrent mode,
 *			    1 for SoftAP in STA+SoftAP concurrent mode
 *      @dev: register netdev pointer to LWIP. Reserved.
 *      @dev_addr: set netif hw address
 *
 *      Return Value: None
 */
#include <net/if.h>
extern struct netif *netdev_findbyname(FAR const char *ifname);
static void _netlib_setmacaddr(const char *ifname, const uint8_t *macaddr)
{
	struct netif *dev = netdev_findbyname(ifname);
	if (dev) {
		memcpy(dev->d_mac.ether_addr_octet, macaddr, IFHWADDRLEN);
	}
}

void rltk_wlan_set_netif_info(int idx_wlan, void *dev, unsigned char *dev_addr)
{
#if (CONFIG_LWIP_LAYER == 1)
#if defined(CONFIG_MBED_ENABLED)
#else
	_netlib_setmacaddr(xnetif[idx_wlan].name, dev_addr);
	rtw_memcpy(xnetif[idx_wlan].hwaddr, dev_addr, 6);
	xnetif[idx_wlan].state = dev;
#endif
#endif
}

/**
 *      rltk_wlan_send - send IP packets to WLAN. Called by low_level_output().
 *      @idx: netif index
 *      @sg_list: data buffer list
 *      @sg_len: size of each data buffer
 *      @total_len: total data len
 *
 *      Return Value: None
 */
int rltk_wlan_send(int idx, struct eth_drv_sg *sg_list, int sg_len, int total_len)
{
#if (CONFIG_LWIP_LAYER == 1)
	struct eth_drv_sg *last_sg;
	struct sk_buff *skb = NULL;
	int ret = 0;

#ifdef CONFIG_TX_ZERO_COPY
	u8 *data = NULL;
	u8 *Mac_data = NULL;
#endif

	if (idx == -1) {
		DBG_ERR("netif is DOWN");
		return -1;
	}
	DBG_TRACE("%s is called", __FUNCTION__);

	save_and_cli();
	if (rltk_wlan_check_isup(idx)) {
		rltk_wlan_tx_inc(idx);
	} else {
		DBG_ERR("netif is DOWN");
		restore_flags();
		return -1;
	}
	restore_flags();

#ifdef CONFIG_TX_ZERO_COPY
	data = rtw_malloc(1640);
	if (data == NULL) {
		ndbg("\r\n###########ERR_MEM at %s\r\n", __func__);
		ret = -1;
		goto exit;
	}

	Mac_data = data + 80;

	for (last_sg = &sg_list[sg_len]; sg_list < last_sg; ++sg_list) {
		MEMCPY(Mac_data, (void *)(sg_list->buf), sg_list->len);
		Mac_data += sg_list->len;
	}

	skb = (struct sk_buff *)rltk_wlan_alloc_skb_0copy();
	if (skb == NULL) {
		ndbg("\r\nrltk_wlan_alloc_skb()failed!\r\n");
		goto exit;
	}
	skb->data = data + 80;
	skb->head = data;
	skb->end = data + 1640;
	skb->tail = data + 80 + total_len;
	skb->len = total_len;
	ret = 0;
#else
	skb = rltk_wlan_alloc_skb(total_len);
#endif

	if (skb == NULL) {
		ret = -1;
		goto exit;
	}

#ifndef CONFIG_TX_ZERO_COPY
	for (last_sg = &sg_list[sg_len]; sg_list < last_sg; ++sg_list) {
		MEMCPY(skb->tail, (void *)(sg_list->buf), sg_list->len);
		skb_put(skb, sg_list->len);
	}
#endif

	rltk_wlan_send_skb(idx, skb);

exit:
	save_and_cli();
	rltk_wlan_tx_dec(idx);
	restore_flags();
	return ret;
#endif
}

/**
 *      rltk_wlan_recv - indicate packets to LWIP. Called by ethernetif_recv().
 *      @idx: netif index
 *      @sg_list: data buffer list
 *      @sg_len: size of each data buffer
 *
 *      Return Value: None
 */
void rltk_wlan_recv(int idx, struct eth_drv_sg *sg_list, int sg_len)
{
#if (CONFIG_LWIP_LAYER == 1)
	struct eth_drv_sg *last_sg;
	struct sk_buff *skb;

	DBG_TRACE("%s is called", __FUNCTION__);
	if (idx == -1) {
		DBG_ERR("skb is NULL");
		return;
	}
	skb = rltk_wlan_get_recv_skb(idx);
	DBG_ASSERT(skb, "No pending rx skb");

	for (last_sg = &sg_list[sg_len]; sg_list < last_sg; ++sg_list) {
		if (sg_list->buf != 0) {
			MEMCPY((void *)(sg_list->buf), skb->data, sg_list->len);
			skb_pull(skb, sg_list->len);
		}
	}
#endif
}

int netif_is_valid_IP(int idx, unsigned char *ip_dest)
{
#if defined(CONFIG_MBED_ENABLED)
	return 1;
#else
#if CONFIG_LWIP_LAYER == 1
	struct netif *pnetif = &xnetif[idx];

	ip_addr_t addr = {0};

#ifdef CONFIG_MEMORY_ACCESS_ALIGNED
	unsigned int temp;
	memcpy(&temp, ip_dest, sizeof(unsigned int));
	u32_t *ip_dest_addr = &temp;
#else
	u32_t *ip_dest_addr = (u32_t *)ip_dest;
#endif

#if LWIP_VERSION_MAJOR >= 2
	ip_addr_set_ip4_u32(&addr, *ip_dest_addr);
#else
	addr.addr = *ip_dest_addr;
#endif

#if (LWIP_VERSION_MAJOR >= 2)
	if ((ip_addr_get_ip4_u32(netif_ip_addr4(pnetif))) == 0)
		return 1;
#else

	if (pnetif->ip_addr.addr == 0)
		return 1;
#endif

	if (ip_addr_ismulticast(&addr) || ip_addr_isbroadcast(&addr, pnetif)) {
		return 1;
	}

	if (ip_addr_cmp(&(pnetif->ip_addr), &addr))
		return 1;

	DBG_TRACE("invalid IP: %d.%d.%d.%d ", ip_dest[0], ip_dest[1], ip_dest[2], ip_dest[3]);
#endif
#ifdef CONFIG_DONT_CARE_TP
	if (pnetif->flags & NETIF_FLAG_IPSWITCH)
		return 1;
	else
#endif
		return 0;
#endif
}

#if !defined(CONFIG_MBED_ENABLED)
int netif_get_idx(struct netif *pnetif)
{
#if (CONFIG_LWIP_LAYER == 1)
	int idx = pnetif - xnetif;

	switch (idx) {
	case 0:
		return 0;
	case 1:
		return 1;
	default:
		return -1;
	}
#else
	return -1;
#endif
}

unsigned char *netif_get_hwaddr(int idx_wlan)
{
#if (CONFIG_LWIP_LAYER == 1)
	return xnetif[idx_wlan].hwaddr;
#else
	return NULL;
#endif
}
#endif

#if defined(CONFIG_MBED_ENABLED)
emac_callback emac_callback_func = NULL;
void *emac_callback_data = NULL;
void set_callback_func(emac_callback p, void *data)
{
	emac_callback_func = p;
	emac_callback_data = data;
}
#endif

void netif_rx(int idx, unsigned int len)
{
#if (CONFIG_LWIP_LAYER == 1)
#if defined(CONFIG_MBED_ENABLED)
	emac_callback_func(emac_callback_data, NULL, len);
#else
	ethernetif_recv(&xnetif[idx], len);
#endif
#endif
#if (CONFIG_INIC_EN == 1)
	inic_netif_rx(idx, len);
#endif
}

void netif_post_sleep_processing(void)
{
}

void netif_pre_sleep_processing(void)
{
}

#ifdef CONFIG_WOWLAN
unsigned char *rltk_wlan_get_ip(int idx)
{
#if (CONFIG_LWIP_LAYER == 1)
	return LwIP_GetIP(&xnetif[idx]);
#else
	return NULL;
#endif
}
unsigned char *rltk_wlan_get_gw(int idx)
{
#if (CONFIG_LWIP_LAYER == 1)
	return LwIP_GetGW(&xnetif[idx]);
#else
	return NULL;
#endif
}

unsigned char *rltk_wlan_get_gwmask(int idx)
{
#if (CONFIG_LWIP_LAYER == 1)
	return LwIP_GetMASK(&xnetif[idx]);
#else
	return NULL;
#endif
}
#endif
