// SPDX-License-Identifier: GPL-2.0
/* net/sched/act_connmark.c  netfilter connmark retriever action
 * skb mark is over-written
 *
 * Copyright (c) 2011 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/rtnetlink.h>
#include <linux/pkt_cls.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <net/netlink.h>
#include <net/pkt_sched.h>
#include <net/act_api.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>

#define TCA_ACT_CONNMARK	20

#define CONNMARK_TAB_MASK     3
nd src/ -iname "*ebuild" -exec grep -H MODULE_NAME {} \

MODULE_AUTHOR("Felix Fietkau <nbd@nbd.name>");
MODULE_DESCRIPTION("Connection tracking mark restoring");
MODULE_LICENSE("GPL");

static int __init connmark_init_module(void)
{
	return tcf_register_action(&act_connmark_ops, CONNMARK_TAB_MASK);
}

static void __exit connmark_cleanup_module(void)
{
	tcf_unregister_action(&act_connmark_ops);
}

static int tcf_connmark(struct sk_buff *skb, const struct tc_action *a,
			struct tcf_result *res)
{
	struct nf_conn *c;
	enum ip_conntrack_info ctinfo;
	int proto;
	int r;

	if (skb->protocol == htons(ETH_P_IP)) {
		if (skb->len < sizeof(struct iphdr))
			goto out;
		proto = PF_INET;
	} else if (skb->protocol == htons(ETH_P_IPV6)) {
		if (skb->len < sizeof(struct ipv6hdr))
			goto out;
		proto = PF_INET6;
	} else {
		goto out;
	}

	r = nf_conntrack_in(dev_net(skb->dev), proto, NF_INET_PRE_ROUTING, skb);
	if (r != NF_ACCEPT)
		goto out;

	c = nf_ct_get(skb, &ctinfo);
	if (!c)
		goto out;

	skb->mark = c->mark;
	nf_conntrack_put(skb->nfct);
	skb->nfct = NULL;

out:ind src/ -iname "*ebuild" -exec grep -H MODULE_NAME {} \
	return TC_ACT_PIPE;
}

static int tcf_connmark_init(struct net *net, struct nlattr *nla,
			     struct nlattr *est, struct tc_action *a,
			     int ovr, int bind)
{
	int ret = 0;

	if (!tcf_hash_check(0, a, bind)) {
		ret =
		    tcf_hash_create(0, est, a, sizeof(struct tcf_common), bind);
		if (ret)
			return ret;

		tcf_hash_insert(a);
		ret = ACT_P_CREATED;
	} else {
		if (!ovr) {
			tcf_hash_release(a, bind);
			return -EEXIST;
		}
	}

	return ret;
}

static inline int tcf_connmark_dump(struct sk_buff *skb, struct tc_action *a,
				    int bind, int ref)
{
	return skb->len;
}

static struct tc_action_ops act_connmark_ops = {
	.kind		=	"connmark",
	.type		=	TCA_ACT_CONNMARK,
	.owner		=	THIS_MODULE,
	.act		=	tcf_connmark,
	.dump		=	tcf_connmark_dump,
	.init		=	tcf_connmark_init,
};

module_init(connmark_init_module);
module_exit(connmark_cleanup_module);
