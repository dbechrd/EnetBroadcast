#include <stdio.h>
#include <stdint.h>
#include <enet/enet.h>

typedef struct SPacketTransmission {
    uint16_t signature;
    uint16_t packettype;
    uint32_t u4extra;
    uint64_t u8extra;
} PacketTransmission;

ENetHost *host;
ENetSocket listensocket;

void listenforclients()
{
    ENetSocketSet set;
    ENET_SOCKETSET_EMPTY(set);
    ENET_SOCKETSET_ADD(set, listensocket);
    if (enet_socketset_select(listensocket, &set, NULL, 0) <= 0) {
        //nothing, return
        return;
    }

    printf("we got a message?\n");
    ENetAddress scanneraddress = { 0 };

    PacketTransmission pt = { 0 };

    ENetBuffer scannerbuffer = { 0 };
    scannerbuffer.data = (char *)&pt;
    scannerbuffer.dataLength = sizeof(PacketTransmission);

    int recvlen = enet_socket_receive(listensocket, &scanneraddress, &scannerbuffer, sizeof(ENetBuffer));
    if (recvlen <= 0) {
        return;
    }

    typedef struct SPrinableIpInfo {
        char hostname[64];
        uint16_t port;
    }PrinableIpInfo;

    PrinableIpInfo ouripforscanner = { 0 };
    if (enet_address_get_host(&host->address, ouripforscanner.hostname, sizeof(ouripforscanner.hostname)) != 0) {
        return;
    }
    ouripforscanner.port = host->address.port;
    scannerbuffer.data = &ouripforscanner;
    scannerbuffer.dataLength = sizeof(PrinableIpInfo);
}
void checkforconnectionfrompeer()
{
    ENetEvent event;
    int check = enet_host_service(host, &event, 0);
    if (check > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
            {

                printf("got a connection!\n");
            }
            break;
            case ENET_EVENT_TYPE_RECEIVE:
            {

                printf("got a message!\n");

                enet_packet_destroy(event.packet);
            }
            break;
            default:
                break;
        }
    }
}

int main()
{
    listensocket = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
    enet_socket_set_option(listensocket, ENET_SOCKOPT_REUSEADDR, 1);
    ENetAddress listenaddress = { 0 };
    listenaddress.host = ENET_HOST_ANY;
    listenaddress.port = 6789;
    if (enet_socket_bind(listensocket, &listenaddress)) {
        printf("bind error\n");
        exit(EXIT_FAILURE);
    }

    ENetAddress connectionaddress = { 0 };
    connectionaddress.host = ENET_HOST_ANY;
    connectionaddress.port = ENET_PORT_ANY;

    host = enet_host_create(&connectionaddress, 16, 2, 0, 0);
    if (!host) {
        printf("host error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        listenforclients();
        checkforconnectionfrompeer();
    }
}