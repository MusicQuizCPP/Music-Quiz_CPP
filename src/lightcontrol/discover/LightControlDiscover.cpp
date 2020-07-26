#include "LightControlDiscover.hpp"

#include "mdns.h"

#include "lightcontrol/discover/mdns_helper.h"

#include "common/Log.hpp"


static int queryCallback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
               uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data,
               size_t size, size_t name_offset, size_t name_length, size_t record_offset,
               size_t record_length, void* user_data);


using namespace LightControl;

LightControlDiscover::LightControlDiscover() :
	shouldStop(false), discover_thread(&LightControlDiscover::run, this)
{
	init();
}

LightControlDiscover::~LightControlDiscover()
{
	shouldStop = true;
	discover_thread.join();
}

int queryCallback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
               uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data,
               size_t size, size_t name_offset, size_t name_length, size_t record_offset,
               size_t record_length, void* user_data) 
{
	(void)(sizeof(sock));
	(void)(sizeof(entry));
	(void)(sizeof(query_id));
	(void)(sizeof(from));
	(void)(sizeof(addrlen));
	(void)(sizeof(ttl));
	(void)(sizeof(name_length));
	(void)(sizeof(user_data));
	(void)(sizeof(rclass));

	char entrybuffer[256];
	char namebuffer[256];
    LightControlDiscover* instance = static_cast<LightControlDiscover*>(user_data);


	mdns_string_t entrystr = mdns_string_extract(data, size, &name_offset, entrybuffer, sizeof(entrybuffer));
	if (rtype == MDNS_RECORDTYPE_A) {
		struct sockaddr_in addr;
		mdns_record_parse_a(data, size, record_offset, record_length, &addr);
		mdns_string_t addrstr =
		    ipv4_address_to_string(namebuffer, sizeof(namebuffer), &addr, sizeof(addr));
		instance->addDevice(std::string(entrystr.str, entrystr.length), std::string(addrstr.str, addrstr.length));
	} else if (rtype == MDNS_RECORDTYPE_AAAA) {
		struct sockaddr_in6 addr;
		mdns_record_parse_aaaa(data, size, record_offset, record_length, &addr);
		mdns_string_t addrstr =
		    ipv6_address_to_string(namebuffer, sizeof(namebuffer), &addr, sizeof(addr));
		instance->addDevice(std::string(entrystr.str, entrystr.length), std::string(addrstr.str, addrstr.length));
	}

	return 0;
}

void LightControlDiscover::clearDevices()
{
	const std::lock_guard<std::mutex> lock(_deviceLock);
	_devices.clear();
}

void LightControlDiscover::addDevice(std::string name, std::string address)
{
	const std::lock_guard<std::mutex> lock(_deviceLock);
	_devices[name] = address;
}


std::map<std::string, std::string> LightControlDiscover::getDevices()
{
	return _devices;
}

void LightControlDiscover::init()
{
	//Initialize windows socket API
	#ifdef _WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return;
	}
	#endif
}

void LightControlDiscover::run()
{
	const char* service = "_ws._tcp.local.";
	int sockets[32];
	int query_id[32];
	int num_sockets = open_client_sockets(sockets, sizeof(sockets) / sizeof(sockets[0]), 0);
	if (num_sockets <= 0) {
		LOG_INFO("Failed to open any client sockets\n");
		return;
	}
	size_t capacity = 2048;
	void* buffer = malloc(capacity);
	void* user_data = this;
	size_t records;

	while(!shouldStop)
	{
		for (int isock = 0; isock < num_sockets; ++isock) {
			query_id[isock] = mdns_query_send(sockets[isock], MDNS_RECORDTYPE_PTR, service,
											strlen(service), buffer, capacity, 0);
		}

		// This is a simple implementation that loops for 5 seconds or as long as we get replies
		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		int nfds = 0;
		fd_set readfs;
		FD_ZERO(&readfs);
		for (int isock = 0; isock < num_sockets; ++isock) {
			if (sockets[isock] >= nfds)
				nfds = sockets[isock] + 1;
			FD_SET(sockets[isock], &readfs);
		}

		records = 0;
		int res = select(nfds, &readfs, 0, 0, &timeout);
		if (res > 0) {
			for (int isock = 0; isock < num_sockets; ++isock) {
				if (FD_ISSET(sockets[isock], &readfs)) {
					records += mdns_query_recv(sockets[isock], buffer, capacity, queryCallback,
												user_data, query_id[isock]);
				}
				FD_SET(sockets[isock], &readfs);
			}
		}
	}


	free(buffer);

	for (int isock = 0; isock < num_sockets; ++isock)
		mdns_socket_close(sockets[isock]);
}