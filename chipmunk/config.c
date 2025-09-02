#include "platform.h"
#include "config.h"
#include "uopt.h"
#include "util.h"

#define CONFIG_FILE_NAME "udpxy.ini"
#define MAX_LINE_LEN 256

static char* trim_whitespace(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

int load_config(struct udpxy_opt* uo) {
    FILE* fp;
    char line[MAX_LINE_LEN];
    char* key;
    char* value;

    fp = fopen(CONFIG_FILE_NAME, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        value = strchr(line, '=');
        if (value == NULL) {
            continue;
        }
        *value = '\0';
        value++;
        key = trim_whitespace(line);
        value = trim_whitespace(value);

        if (strcmp(key, "verbose") == 0) {
            uo->is_verbose = atoi(value) ? uf_TRUE : uf_FALSE;
        } else if (strcmp(key, "client_stats") == 0) {
            uo->cl_tpstat = atoi(value) ? uf_TRUE : uf_FALSE;
        } else if (strcmp(key, "max_clients") == 0) {
            uo->max_clients = atoi(value);
        } else if (strcmp(key, "buffer_size_kb") == 0) {
            ssize_t size_bytes = atoi(value) * 1024;
            if (size_bytes > 0) uo->rbuf_len = size_bytes;
        } else if (strcmp(key, "buffer_messages") == 0) {
            uo->rbuf_msgs = atoi(value);
        } else if (strcmp(key, "nice_increment") == 0) {
            uo->nice_incr = atoi(value);
        } else if (strcmp(key, "mc_refresh_sec") == 0) {
            uo->mcast_refresh = (u_short)atoi(value);
        }
    }

    fclose(fp);
    return 0;
}

int generate_default_config(void) {
    FILE* fp = fopen(CONFIG_FILE_NAME, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error creating config file: %s\n", strerror(errno));
        return -1;
    }

    fprintf(fp, "# udpxy Configuration File\n");
    fprintf(fp, "verbose = 0\n");
    fprintf(fp, "client_stats = 0\n");
    fprintf(fp, "max_clients = %d\n", DEFAULT_CLIENT_COUNT);
    fprintf(fp, "buffer_size_kb = %ld\n", (long)DEFAULT_CACHE_LEN / 1024);
    fprintf(fp, "buffer_messages = 1\n");
    fprintf(fp, "nice_increment = 0\n");
    fprintf(fp, "mc_refresh_sec = %d\n", (int)DEFAULT_MCAST_REFRESH);

    fclose(fp);
    printf("Default configuration file '%s' created successfully.\n", CONFIG_FILE_NAME);
    return 0;
}
