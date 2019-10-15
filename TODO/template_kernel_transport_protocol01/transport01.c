// transport01.c

/*
  compile:
  gcc -I/lib/modules/'uname -r'/build/include -c transport01.c
*/
#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#include <linux/in.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/version.h>

MODULE_AUTHOR("Me, Myself & I(my@emai.com)");
MODULE_DESCRIPTION("layer 3 protocol demo - transport");

#define TEST_PROTO_ID 0x1234

int test_pack_rcv(struct sk_buff *skb, struct net_device *dev,
                  struct packet_type *pt);

static struct packet_type test_protocol = {
    __constant_htons(TEST_PROTO_ID), NULL, test_pack_rcv, ( void * )1, NULL};


int test_pack_rcv(struct sk_buff *skb, struct net_device *dev,
                  struct packet_type *pt)
{
  printk(KERN_DEBUG "Test-Protocol: Packet Received with length: %u\n",
         skb->len);
  return skb->len;
}


int init_module(void)
{
  dev_add_pack(&test_protocol);
  return 0;
}


void cleanup_module(void) { dev_remove_pack(&test_protocol); }
