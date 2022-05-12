#include <stdio.h>
#include <stdint.h>
#include <enet/enet.h>

typedef struct SPacketTransmission {
    uint16_t signature;
    uint16_t packettype;
    uint32_t u4extra;
    uint64_t u8extra;
}PacketTransmission;
#define LANBROADCASTSIGNATURE 0xA21F

int main()
{
    ENetSocket scanner;

    enet_initialize();

    scanner = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
    if (scanner == ENET_SOCKET_NULL) {
        printf("bad socket\n");
        exit(EXIT_FAILURE);
    }

    enet_socket_set_option(scanner, ENET_SOCKOPT_BROADCAST, 1);

    ENetAddress addr = { 0 };
    addr.host = ENET_HOST_BROADCAST;
    addr.port = 6789;

    //throw out broadcasts
    while (1) {
        PacketTransmission pt = { 0 };
        pt.signature = LANBROADCASTSIGNATURE;
        ENetBuffer sendbuf;
        sendbuf.data = (char *)&pt;
        sendbuf.dataLength = sizeof(PacketTransmission);
        enet_socket_send(scanner, &addr, &sendbuf, sizeof(PacketTransmission));
        Sleep(1000);
    }

    enet_socket_destroy(scanner);
    enet_deinitialize();
}