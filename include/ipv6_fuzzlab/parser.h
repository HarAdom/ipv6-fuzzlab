#ifndef IPV6_FUZZLAB_PARSER_H
#define IPV6_FUZZLAB_PARSER_H

#include <stddef.h>
#include <stdint.h>

#include "ipv6.h"

enum ipv6_parse_status {
    IPV6_PARSE_OK = 0,
    IPV6_PARSE_TOO_SHORT,
    IPV6_PARSE_BAD_VERSION,
    IPV6_PARSE_LENGTH_MISMATCH,
    IPV6_PARSE_EXTENSION_TRUNCATED,
    IPV6_PARSE_EXTENSION_LOOP
};

struct ipv6_parse_result {
    enum ipv6_parse_status status;
    struct ipv6_header_view header;
    size_t packet_length;
    size_t payload_offset;
    size_t upper_layer_offset;
    uint8_t upper_layer_protocol;
    unsigned extension_count;
};

enum ipv6_parse_status
ipv6_parse(const uint8_t *packet, size_t length,
           struct ipv6_parse_result *result);

const char *ipv6_parse_status_string(enum ipv6_parse_status status);

void ipv6_print_result(const struct ipv6_parse_result *result);

#endif
