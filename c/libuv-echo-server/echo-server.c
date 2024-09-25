#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define PORT        7000
#define QUIT        "bye"
#define QUIT_SZ     (sizeof(QUIT) - 1)
#define OUT(client) client->data
#define BUF_SZ      4096

uv_tcp_t server;
uv_tty_t *stdin_tty = NULL, stdout_tty;

void alloc_buf(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf)
    { *buf = uv_buf_init(malloc(BUF_SZ), BUF_SZ); }
void free_buf(uv_write_t *req, int status) { free(((uv_buf_t *)req->data)->base); free(req); }
void free_client(uv_handle_t *client)      { free(client); }

void shutdown_client(uv_handle_t *client, const uv_buf_t *buf) {
    free(buf->base);
    uv_close(client, free_client);
    if (client == (uv_handle_t *)stdin_tty) {
        uv_close((uv_handle_t *)&server,     NULL);
        uv_close((uv_handle_t *)&stdout_tty, NULL);
    }
}

void process(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0 || (nread >= QUIT_SZ && strncmp(buf->base, QUIT, QUIT_SZ) == 0))
        { shutdown_client((uv_handle_t *)client, buf); return; }
    if (nread == 0) { free(buf->base); return; }
    uv_write_t *req = malloc(sizeof(uv_write_t));
    req->data = (void *)buf;
    ((uv_buf_t *)buf)->len = nread;
    uv_write(req, OUT(client), buf, 1, free_buf);
}

void accept_client(uv_stream_t *server, int status) {
    uv_tcp_t *client = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), client);
    OUT(client) = client;
    uv_accept(server, (uv_stream_t *)client);
    uv_read_start((uv_stream_t *)client, alloc_buf, process);
}

int main(int argc, char **argv) {
    uv_tcp_init(uv_default_loop(), &server);
    struct sockaddr_in addr;
    uv_ip4_addr("127.0.0.1", (argc > 1 ? atoi(argv[1]) : PORT), &addr);
    uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0);
    uv_listen((uv_stream_t *)&server, SOMAXCONN, accept_client);

    stdin_tty = malloc(sizeof(uv_tty_t));
    uv_tty_init(uv_default_loop(),  stdin_tty,  0, 0);
    uv_tty_init(uv_default_loop(), &stdout_tty, 1, 0);
    OUT(stdin_tty) = &stdout_tty;
    uv_read_start((uv_stream_t *)stdin_tty, alloc_buf, process);

    return uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
