#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>

#include <libmnl/libmnl.h>
#include <libnftnl/set.h>
#include "nft_ops.h"


int main() {
	int i = 1;
	nft_map_elem_do(NFT_MAP_ADD_ELEM, "2002::13", "2002::14", "natcap", "simplemap");	
	return 0;
}

int nft_map_elem_do(int action, const char* key, const char* val, const char* table, const char* map)
{
	struct mnl_socket *nl;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct mnl_nlmsg_batch *batch;
	struct nlmsghdr *nlh;
	uint32_t portid, seq, family;
	struct nftnl_set *s;
	struct nftnl_set_elem *e;
	uint16_t data;
	int ret;
	s = nftnl_set_alloc();
	if (s == NULL) {
		perror("OOM");
		exit(EXIT_FAILURE);
	}

	seq = time(NULL);
	family = NFPROTO_IPV6;

	nftnl_set_set(s, NFTNL_SET_TABLE, table);
	nftnl_set_set(s, NFTNL_SET_NAME, map);

	e = nftnl_set_elem_alloc();
	if (e == NULL) {
		perror("OOM");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in6 sa1,sa2;
	inet_pton(AF_INET6, key, &(sa1.sin6_addr));
	inet_pton(AF_INET6, val, &(sa2.sin6_addr));
	nftnl_set_elem_set(e, NFTNL_SET_ELEM_KEY, &(sa1.sin6_addr), 16);
	nftnl_set_elem_set(e, NFTNL_SET_ELEM_DATA, &(sa2.sin6_addr), 16);
	nftnl_set_elem_add(s, e);

	batch = mnl_nlmsg_batch_start(buf, sizeof(buf));

	nftnl_batch_begin(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);
	switch(action){
		case NFT_MAP_ADD_ELEM:
			nft_map_elem_do(NFT_MAP_DEL_ELEM, key, val, table, map);
			nlh = nftnl_nlmsg_build_hdr(mnl_nlmsg_batch_current(batch),
				    NFT_MSG_NEWSETELEM, family,
				    NLM_F_CREATE | NLM_F_REPLACE | NLM_F_ACK,
				    seq++);
			break;
		case NFT_MAP_DEL_ELEM:
			nlh = nftnl_nlmsg_build_hdr(mnl_nlmsg_batch_current(batch),
				    NFT_MSG_DELSETELEM, family,
				    NLM_F_ACK,
				    seq++);

			break;
	}
	nftnl_set_elems_nlmsg_build_payload(nlh, s);
	nftnl_set_free(s);
	mnl_nlmsg_batch_next(batch);

	nftnl_batch_end(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);

	nl = mnl_socket_open(NETLINK_NETFILTER);
	if (nl == NULL) {
		perror("mnl_socket_open");
		exit(EXIT_FAILURE);
	}

	if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
		perror("mnl_socket_bind");
		exit(EXIT_FAILURE);
	}
	portid = mnl_socket_get_portid(nl);

	if (mnl_socket_sendto(nl, mnl_nlmsg_batch_head(batch),
			      mnl_nlmsg_batch_size(batch)) < 0) {
		perror("mnl_socket_send");
		exit(EXIT_FAILURE);
	}

	mnl_nlmsg_batch_stop(batch);

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, 0, portid, NULL, NULL);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}
	if (ret == -1) {
		//perror("error");
		//exit(EXIT_FAILURE);
	}
	mnl_socket_close(nl);

	return EXIT_SUCCESS;
}


