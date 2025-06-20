#if defined(_PLATFORM_WEB)
#include <emscripten.h>
#include <emscripten/websocket.h>
#include <emscripten/threading.h>
#include <emscripten/posix_socket.h>

static EMSCRIPTEN_WEBSOCKET_T bridgeSocket = 0;
#endif

#define ENET_IMPLEMENTATION
#include "network.h"
#include "../utils/utils.h"
#include "net_packer.h"


#include <stdlib.h>
#include <time.h>


void init_network()
{
	if(enet_initialize() != 0)
    {
        log_error("An error occurred while initializing ENet.\n");

		exit(EXIT_FAILURE);
    }
}

void init_network_server(Server* server, int port, int max_clients, int channels)
{
	ENetAddress address = {0};
	address.host = ENET_HOST_ANY;
	address.port = port;

	server->host = enet_host_create(&address, max_clients, channels, 0, 0);

	if(server == NULL)
	{
	log_debug("Hey!\n");
		log_error("Error in creating server!\n");
		exit(EXIT_FAILURE);
	}
}

void init_network_client(Client* client, int channels)
{
	client->host = enet_host_create(NULL, 2, 2, 0, 0);

	if(client->host == NULL)
	{
		log_error("Error in creating client!\n");
		exit(EXIT_FAILURE);
	}
}

void network_client_connect_peer(Client* client, const char* address_str, int port, int channels, ENetPeer** peer, ENetEvent* event)
{
	ENetAddress address;
	enet_address_set_host(&address, address_str);
	address.port = port;

	*peer = enet_host_connect(client->host, &address, channels, 0);

	if(peer == NULL)
	{
		log_debug("No available peers for initiating an ENet connection\n");
		exit(EXIT_FAILURE);
	}
}

void send_packet(ENetHost* host, ENetPeer* peer, int channel, void* data, size_t packet_size)
{
	ENetPacket* packet = enet_packet_create(data, packet_size, ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(peer, channel, packet);
}

void send_int_packet(ENetHost* host, ENetPeer* peer, int channel, int data)
{
	unsigned char buf[2];
	int size = pack(buf, "h", data);
	send_packet(host, peer, channel, buf, size);
}

void broadcast_packet(ENetHost* host, int channel_id, ENetPacket* packet)
{
	enet_host_broadcast(host, channel_id, packet);
}

void disconnect_peer(ENetPeer* peer)
{
	enet_peer_disconnect(peer, 0);
}

void destroy_network()
{
	enet_deinitialize();
}

void destroy_network_server(Server* server)
{
	enet_host_destroy(server->host);
}

void destroy_network_client(Client* client)
{
	enet_host_destroy(client->host);
}
