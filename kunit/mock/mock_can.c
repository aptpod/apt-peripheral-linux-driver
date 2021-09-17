#include <linux/can/dev.h>

/* drivers/net/can/dev.c */
struct net_device *alloc_candev_mqs(int sizeof_priv, unsigned int echo_skb_max, unsigned int txqs, unsigned int rxqs)
{
}

int register_candev(struct net_device *dev)
{
}

void free_candev(struct net_device *dev)
{
}

struct sk_buff *alloc_can_skb(struct net_device *dev, struct can_frame **cf)
{
}

unsigned int can_get_echo_skb(struct net_device *dev, unsigned int idx)
{
}

int open_candev(struct net_device *dev)
{
}

void close_candev(struct net_device *dev)
{
}

void can_put_echo_skb(struct sk_buff *skb, struct net_device *dev, unsigned int idx)
{
}

int can_change_mtu(struct net_device *dev, int new_mtu)
{
}