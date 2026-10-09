#include "ipv6_fuzzlab/parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PACKET 65535U

static void usage(const char *name)
{
    printf(
        "IPv6-FuzzLab %s\n\n"
        "Usage:\n"
        "  %s --help\n"
        "  %s --version\n"
        "  %s --self-test\n"
        "  %s --fixture NAME\n"
        "  %s --hex HEXSTRING\n"
        "  %s --file FILE\n\n"
        "Fixtures:\n"
        "  basic\n"
        "  malformed-length\n"
        "  truncated-extension\n",
        "0.1.0", name, name, name, name, name, name);
}

static int hex_value(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static size_t hex_decode(const char *s, unsigned char *out, size_t capacity)
{
    size_t count = 0;
    int high = -1;

    for (; *s != '\0'; ++s) {
        int v;

        if (isspace((unsigned char)*s)) continue;

        v = hex_value(*s);
        if (v < 0) return 0;

        if (high < 0) {
            high = v;
        } else {
            if (count >= capacity) return 0;
            out[count++] = (unsigned char)((high << 4) | v);
            high = -1;
        }
    }

    return high < 0 ? count : 0;
}

static size_t make_basic(unsigned char *p, size_t cap)
{
    static const unsigned char src[16] =
        {0x20,0x01,0x0d,0xb8,0,0,0,0,0,0,0,0,0,0,0,1};
    static const unsigned char dst[16] =
        {0x20,0x01,0x0d,0xb8,0,0,0,0,0,0,0,0,0,0,0,2};

    if (cap < 48U) return 0;

    memset(p, 0, 48U);
    p[0] = 0x60;
    p[4] = 0;
    p[5] = 8;
    p[6] = 58; /* ICMPv6 */
    p[7] = 64;
    memcpy(p + 8, src, 16);
    memcpy(p + 24, dst, 16);

    p[40] = 128; /* Echo Request */
    p[41] = 0;

    return 48U;
}

static size_t make_malformed_length(unsigned char *p, size_t cap)
{
    size_t n = make_basic(p, cap);
    if (n == 0) return 0;
    p[4] = 0;
    p[5] = 200;
    return n;
}

static size_t make_truncated_extension(unsigned char *p, size_t cap)
{
    if (cap < 48U) return 0;

    memset(p, 0, 48U);
    p[0] = 0x60;
    p[4] = 0;
    p[5] = 8;
    p[6] = 0;  /* Hop-by-Hop */
    p[7] = 64;

    /* Extension header advertises 24 bytes, but only 8 exist. */
    p[40] = 58;
    p[41] = 2;

    return 48U;
}

static int run_fixture(const char *name)
{
    unsigned char packet[MAX_PACKET];
    size_t length = 0;

    if (strcmp(name, "basic") == 0) {
        length = make_basic(packet, sizeof(packet));
    } else if (strcmp(name, "malformed-length") == 0) {
        length = make_malformed_length(packet, sizeof(packet));
    } else if (strcmp(name, "truncated-extension") == 0) {
        length = make_truncated_extension(packet, sizeof(packet));
    } else {
        fprintf(stderr, "Unknown fixture: %s\n", name);
        return 2;
    }

    struct ipv6_parse_result result;
    ipv6_parse(packet, length, &result);
    ipv6_print_result(&result);
    return result.status == IPV6_PARSE_OK ? 0 : 1;
}

static int parse_file(const char *filename)
{
    unsigned char packet[MAX_PACKET];
    FILE *fp = fopen(filename, "rb");
    size_t n;
    struct ipv6_parse_result result;

    if (fp == NULL) {
        perror(filename);
        return 2;
    }

    n = fread(packet, 1, sizeof(packet), fp);
    fclose(fp);

    if (n == sizeof(packet)) {
        fprintf(stderr, "Input exceeds maximum supported packet size.\n");
        return 2;
    }

    ipv6_parse(packet, n, &result);
    ipv6_print_result(&result);
    return result.status == IPV6_PARSE_OK ? 0 : 1;
}

static int run_self_test(void)
{
    const char *fixtures[] = {
        "basic",
        "malformed-length",
        "truncated-extension"
    };
    const size_t count = sizeof(fixtures) / sizeof(fixtures[0]);
    size_t i;
    int failures = 0;

    for (i = 0; i < count; ++i) {
        int rc = run_fixture(fixtures[i]);

        /* malformed fixtures intentionally return non-zero. */
        if (strcmp(fixtures[i], "basic") == 0 && rc != 0) failures++;
        if (strcmp(fixtures[i], "malformed-length") == 0 && rc == 0) failures++;
        if (strcmp(fixtures[i], "truncated-extension") == 0 && rc == 0) failures++;
    }

    printf("\nSelf-test: %s\n", failures == 0 ? "PASS" : "FAIL");
    return failures == 0 ? 0 : 1;
}

int main(int argc, char **argv)
{
    unsigned char packet[MAX_PACKET];
    size_t length;
    struct ipv6_parse_result result;

    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0) {
        usage(argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0) {
        puts("ipv6-fuzzlab 0.1.0");
        return 0;
    }

    if (strcmp(argv[1], "--self-test") == 0) {
        return run_self_test();
    }

    if (strcmp(argv[1], "--fixture") == 0 && argc == 3) {
        return run_fixture(argv[2]);
    }

    if (strcmp(argv[1], "--hex") == 0 && argc == 3) {
        length = hex_decode(argv[2], packet, sizeof(packet));
        if (length == 0) {
            fprintf(stderr, "Invalid hexadecimal input.\n");
            return 2;
        }

        ipv6_parse(packet, length, &result);
        ipv6_print_result(&result);
        return result.status == IPV6_PARSE_OK ? 0 : 1;
    }

    if (strcmp(argv[1], "--file") == 0 && argc == 3) {
        return parse_file(argv[2]);
    }

    usage(argv[0]);
    return 1;
}
