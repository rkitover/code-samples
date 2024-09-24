#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define PORT       7000
#define OUT_STREAM client->data

void alloc_read_buf(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf) {
    void *memory = malloc(suggested_size);
    *buf = uv_buf_init(memory, suggested_size);
}

void after_write(uv_write_t *req, int status) {
    free(((uv_buf_t *)req->data)->base);
    free(req);
}

void shutdown_client(uv_handle_t *client, const uv_buf_t *buf) {
    bool is_tty = false;

    free(buf->base);

    if (client != OUT_STREAM) {
        is_tty = true;
        uv_close(OUT_STREAM, NULL);
        free(OUT_STREAM);
    }

    uv_close(client, NULL);
    free(client);

    if (is_tty)
        uv_stop(uv_default_loop());
}

void after_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 1 || strncmp(buf->base, "bye", 3) == 0) {
        shutdown_client((uv_handle_t *)client, buf);
        return;
    }

    uv_write_t *req = malloc(sizeof(uv_write_t));
    req->data = buf;
    uv_write(req, OUT_STREAM, buf, 1, after_write);
}

void on_client(uv_stream_t *server, int status) {
    uv_tcp_t *client = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), client);
    OUT_STREAM = client;
    uv_accept(server, (uv_stream_t *)client);
    uv_read_start((uv_stream_t *)client, alloc_read_buf, after_read);
}


int main(int argc, char **argv) {
    unsigned port = argc > 1 ? atoi(argv[1]) : PORT;

    uv_tcp_t server;
    uv_tcp_init(uv_default_loop(), &server);

    struct sockaddr_in addr;
    uv_ip4_addr("127.0.0.1", port, &addr);
    uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0);
    uv_listen((uv_stream_t *)&server, SOMAXCONN, on_client);

    uv_tty_t *stdin_tty  = malloc(sizeof(uv_tty_t));
    uv_tty_t *stdout_tty = malloc(sizeof(uv_tty_t));

    uv_tty_init(uv_default_loop(), stdin_tty,  0, 0);
    uv_tty_init(uv_default_loop(), stdout_tty, 1, 0);

    stdin_tty->data = stdout_tty;

    uv_read_start((uv_stream_t *)stdin_tty, alloc_read_buf, after_read);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}
