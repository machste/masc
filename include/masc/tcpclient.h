#ifndef _MASC_TCPCLIENT_H_
#define _MASC_TCPCLIENT_H_

#include <netinet/in.h>

#include <masc/object.h>


typedef enum {
    TCPCLIENT_SUCCESS,
    TCPCLIENT_INVALID_ADDR,
    TCPCLIENT_SOCKET_ERR,
    TCPCLIENT_CONNECT_ERR,
    TCPCLIENT_SERV_HUP_ERR,
    TCPCLIENT_ERROR,
} TcpClientError;

typedef struct TcpClient TcpClient;

typedef void (*tcpclient_data_cb)(TcpClient *self, void *data, size_t size);
typedef void (*tcpclient_hup_cb)(TcpClient *self);


struct TcpClient {
    Object;
    char sentinel;
    struct sockaddr_in addr;
    int fd;
    void *data;
    size_t size;
    tcpclient_data_cb cli_data_cb;
    tcpclient_data_cb cli_packet_cb;
    tcpclient_hup_cb serv_hup_cb;
    TcpClientError err;
};


extern const void *TcpClientCls;


void tcpclient_init(TcpClient *self, const char *ip, in_port_t port);

void tcpclient_destroy(TcpClient *self);

TcpClientError tcpclient_start(TcpClient *self);
void tcpclient_stop(TcpClient *self);

size_t tcpclient_to_cstr(TcpClient *self, char *cstr, size_t size);


#endif /* _MASC_TCPCLIENT_H_ */