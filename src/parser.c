#include "ipv6_fuzzlab/parser.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

static uint16_t read_be16(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static int is_extension_header(uint8_t nh)
{
    switch (nh) {
        case 0:   /* Hop-by-Hop Options */
        case 43:  /* Routing */
        case 44:  /* Fragment */
        case 60:  /* Destination Options */
        case 51:  /* Authentication Header */
            return 1;
        case 50:  /* ESP: next-header field is not available to this parser. */
        default:
            return 0;
    }
}

static size_t extension_length(uint8_t nh, const uint8_t *p, size_t remaining)
{
    if (nh == 44) {
        return remaining >= 8U ? 8U : 0U;
    }

    if (nh == 51) {
        /* AH length is expressed in 32-bit words minus two. */
        if (remaining < 2U) {
            return 0U;
        }
        return ((size_t)p[1] + 2U) * 4U;
    }

    if (remaining < 2U) {
        return 0U;
    }

    /* Hop-by-Hop, Routing and Destination Options:
       total length = (Hdr Ext Len + 1) * 8. */
    return ((size_t)p[1] + 1U) * 8U;
}

enum ipv6_parse_status
ipv6_parse(const uint8_t *packet, size_t length,
           struct ipv6_parse_result *result)
{
    size_t offset;
    size_t remaining;
    uint8_t next;

    if (result == NULL) {
        return IPV6_PARSE_TOO_SHORT;
    }

    memset(result, 0, sizeof(*result));

    if (packet == NULL || length < IPV6_HEADER_LEN) {
        result->status = IPV6_PARSE_TOO_SHORT;
        return result->status;
    }

    result->header.version = (uint8_t)(packet[0] >> 4);
    if (result->header.version != 6U) {
        result->status = IPV6_PARSE_BAD_VERSION;
        return result->status;
    }

    result->header.traffic_class =
        (uint8_t)(((packet[0] & 0x0FU) << 4) | (packet[1] >> 4));

    result->header.flow_label =
        ((uint32_t)(packet[1] & 0x0FU) << 16) |
        ((uint32_t)packet[2] << 8) |
        packet[3];

    result->header.payload_length = read_be16(packet + 4);
    result->header.next_header = packet[6];
    result->header.hop_limit = packet[7];

    memcpy(result->header.src, packet + 8, 16);
    memcpy(result->header.dst, packet + 24, 16);

    result->packet_length = length;
    result->payload_offset = IPV6_HEADER_LEN;

    if ((size_t)result->header.payload_length >
        length - IPV6_HEADER_LEN) {
        result->status = IPV6_PARSE_LENGTH_MISMATCH;
        return result->status;
    }

    offset = IPV6_HEADER_LEN;
    remaining = result->header.payload_length;
    next = result->header.next_header;

    while (is_extension_header(next)) {
        size_t ext_len;

        if (result->extension_count++ > 32U) {
            result->status = IPV6_PARSE_EXTENSION_LOOP;
            return result->status;
        }

        if (remaining == 0U || offset >= length) {
            result->status = IPV6_PARSE_EXTENSION_TRUNCATED;
            return result->status;
        }

        ext_len = extension_length(next, packet + offset, remaining);

        if (ext_len == 0U || ext_len > remaining ||
            ext_len > length - offset) {
            result->status = IPV6_PARSE_EXTENSION_TRUNCATED;
            return result->status;
        }

        next = packet[offset];
        offset += ext_len;
        remaining -= ext_len;
    }

    result->upper_layer_offset = offset;
    result->upper_layer_protocol = next;
    result->status = IPV6_PARSE_OK;
    return result->status;
}

const char *ipv6_parse_status_string(enum ipv6_parse_status status)
{
    switch (status) {
        case IPV6_PARSE_OK:
            return "OK";
        case IPV6_PARSE_TOO_SHORT:
            return "packet too short";
        case IPV6_PARSE_BAD_VERSION:
            return "not an IPv6 packet";
        case IPV6_PARSE_LENGTH_MISMATCH:
            return "payload length exceeds supplied buffer";
        case IPV6_PARSE_EXTENSION_TRUNCATED:
            return "truncated or invalid extension header";
        case IPV6_PARSE_EXTENSION_LOOP:
            return "extension-header chain too long";
        default:
            return "unknown parser status";
    }
}

void ipv6_print_result(const struct ipv6_parse_result *result)
{
    char src[INET6_ADDRSTRLEN];
    char dst[INET6_ADDRSTRLEN];

    if (result == NULL) {
        return;
    }

    inet_ntop(AF_INET6, result->header.src, src, sizeof(src));
    inet_ntop(AF_INET6, result->header.dst, dst, sizeof(dst));

    printf("IPv6 packet\n");
    printf("  status:          %s\n",
           ipv6_parse_status_string(result->status));
    printf("  version:         %u\n", result->header.version);
    printf("  traffic class:   0x%02x\n", result->header.traffic_class);
    printf("  flow label:      0x%05x\n", result->header.flow_label);
    printf("  payload length:  %u\n", result->header.payload_length);
    printf("  next header:     %u\n", result->header.next_header);
    printf("  hop limit:       %u\n", result->header.hop_limit);
    printf("  source:          %s\n", src);
    printf("  destination:     %s\n", dst);
    printf("  extension count: %u\n", result->extension_count);
    printf("  upper-layer off: %zu\n", result->upper_layer_offset);
    printf("  upper-layer proto: %u\n", result->upper_layer_protocol);
}
