#pragma once
#include <stddef.h>
#include <stdint.h>
#include "mdns.h"

#ifdef __cplusplus
extern "C" {
#endif

	int send_mdns_query(const char* service);
	int open_client_sockets(int* sockets, int max_sockets, int port);
	mdns_string_t ip_address_to_string(char* buffer, size_t capacity, const struct sockaddr* addr, size_t addrlen);
	mdns_string_t ipv6_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in6* addr,
		size_t addrlen);
	mdns_string_t ipv4_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in* addr,
		size_t addrlen);
#ifdef __cplusplus
}
#endif
