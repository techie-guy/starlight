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

void send_packet(ENetHost* host, ENetPeer* peer, int channel, void* data, size_t packet_size);
void send_int_packet(ENetHost* host, ENetPeer* peer, int channel, int data);
void broadcast_packet(ENetHost* host, int channel_id, ENetPacket* packet);

void init_network_server(Server* server, int port, int max_clients, int channels);
void destroy_network_server(Server* server);

void init_network_client(Client* client, int channels);
void network_client_connect_peer(Client* client, const char* address_str, int port, int channels, ENetPeer** peer, ENetEvent* event);
void destroy_network_client(Client* client);
