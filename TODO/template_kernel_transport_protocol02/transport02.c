// transport02.c

/*
  compile:
  gcc -I/lib/modules/'uname -r'/build/include/ -c transport02.c
*/

#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/in.h>
#include <net/protocol.h>

MODULE_AUTHOR("Me, Myself & I (My@Email.com)");
MOUDLE_DESCRIPTION("layer 4 protocol demo - transport");


int test_proto_rcv(struct sk_buff* skb);


static struct inet_protocol test_protocol = {
  &test_proto_rcv,  // protocl handler
  NULL,             // error control
  NULL,             // next
  IPPROTO_TCP,      // protocl ID
  0,                // copy
  NULL,             // data
  "Test_Protocol"   // name
};


int test_proto_rcv(struct sk_buff* skb)
{
  printk(KERN_DEBUG "Test-Protocol: Packet Received with length %u\n", skb->len);
  return skb->len;
}


int init_module(void)
{
  inet_add_protocol(&test_protocol);
  return 0;
}


void cleanup_module(void)
{
  inet_del_protocol(&test_protocol);
}



					       
