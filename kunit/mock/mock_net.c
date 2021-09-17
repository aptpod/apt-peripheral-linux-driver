#include <linux/netdevice.h>

/* include/linux/netdevice.h */
void netdev_info(const struct net_device *dev, const char *format, ...)
{
}

void netdev_err(const struct net_device *dev, const char *format, ...)
{
}

/* net/core/dev.c */
void unregister_netdev(struct net_device *dev)
{
}

int netif_rx(struct sk_buff *skb)
{
}

void netif_tx_wake_queue(struct netdev_queue *dev_queue)
{
}

/* net/core/skbuff.c */
void kfree_skb(struct sk_buff *skb)
{
}