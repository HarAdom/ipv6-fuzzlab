#ifndef IPV6_FUZZLAB_IPV6_H
#define IPV6_FUZZLAB_IPV6_H

#include <stdint.h>

#define IPV6_HEADER_LEN 40U

struct ipv6_header_view {
    uint8_t version;
    uint8_t traffic_class;
    uint32_t flow_label;
    uint16_t payload_length;
    uint8_t next_header;
    uint8_t hop_limit;
    uint8_t src[16];
    uint8_t dst[16];
};

#endif
