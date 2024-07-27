#pragma once

#include <enet.h>

typedef struct
{
	ENetHost* host;

	ENetPeer* peer;
	ENetEvent* event;
} Client;

typedef struct
{
	ENetHost* host;

	Client client;
} Server;

void init_network();
void destroy_network();

void disconnect_peer(ENetPeer* peer);

void send_packet(ENetHost* host, ENetPeer* peer, char* data);

void init_server(Server* server, int port);
void destroy_server(Server* server);

void init_client(Client* client);
void client_connect_peer(Client* client, const char* address_str, int port, ENetPeer** peer, ENetEvent* event);
void destroy_client(Client* client);
