#include <stdlib.h>
#include <string.h>
#include <uv.h>

#define PORT 7000

void alloc_read_buf(uv_handle_t *stream, size_t suggested_size, uv_buf_t *buf) {
    void *memory = malloc(suggested_size);
    *buf = uv_buf_init(memory, suggested_size);
}

void after_write(uv_write_t *req, int status) {
    free(((uv_buf_t *)req->data)->base);
    free(req);
}

void after_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 1 || strncmp(buf->base, "bye", 3) == 0) {
        free(buf->base);
        uv_close((uv_handle_t *)stream, NULL);
        free(stream);
        return;
    }

    uv_write_t *req = malloc(sizeof(uv_write_t));
    req->data = buf;
    uv_write(req, stream, buf, 1, after_write);
}

void on_client(uv_stream_t *server, int status) {
    uv_tcp_t *stream = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), stream);
    stream->data = server;
    uv_accept(server, (uv_stream_t *)stream);
    uv_read_start((uv_stream_t *)stream, alloc_read_buf, after_read);
}


int main(int argc, char **argv) {
    unsigned port = argc > 1 ? atoi(argv[1]) : PORT;

    uv_tcp_t tcp_server;
    uv_tcp_init(uv_default_loop(), &tcp_server);

    struct sockaddr_in addr;
    uv_ip4_addr("127.0.0.1", port, &addr);
    uv_tcp_bind(&tcp_server, (const struct sockaddr *)&addr, 0);
    uv_listen((uv_stream_t *)&tcp_server, SOMAXCONN, on_client);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}
