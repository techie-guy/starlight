#define ENET_IMPLEMENTATION
#include "Network.h"
#include "Utils.h"

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

void init_server(Server* server, int port)
{
	ENetAddress address = {0};
	address.host = ENET_HOST_ANY;
	address.port = port;

	server->host = enet_host_create(&address, 32, 2, 0, 0);

	if(server == NULL)
	{
		log_error("Error in creating server!\n");
		exit(EXIT_FAILURE);
	}
}

void init_client(Client* client)
{
	client->host = enet_host_create(NULL, 1, 2, 0, 0);

	if(client->host == NULL)
	{
		log_error("Error in creating client!\n");
		exit(EXIT_FAILURE);
	}
}

void client_connect_peer(Client* client, const char* address_str, int port, ENetPeer** peer, ENetEvent* event)
{
	ENetAddress address;
	enet_address_set_host(&address, address_str);
	address.port = port;

	*peer = enet_host_connect(client->host, &address, 2, 0);

	if(peer == NULL)
	{
		log_debug("No available peers for initiating an ENet connection\n");
		exit(EXIT_FAILURE);
	}
}

void send_packet(ENetHost* host, ENetPeer* peer, char* data)
{
	ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);

	enet_peer_send(peer, 0, packet);
}

void disconnect_peer(ENetPeer* peer)
{
	enet_peer_disconnect(peer, 0);
}

void destroy_network()
{
	enet_deinitialize();
}

void destroy_server(Server* server)
{
	enet_host_destroy(server->host);
}

void destroy_client(Client* client)
{
	enet_host_destroy(client->host);
}
