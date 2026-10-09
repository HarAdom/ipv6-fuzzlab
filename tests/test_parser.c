#include "ipv6_fuzzlab/parser.h"

#include <assert.h>
#include <string.h>

static void basic_packet(unsigned char *p)
{
    memset(p, 0, 48);
    p[0] = 0x60;
    p[5] = 8;
    p[6] = 58;
    p[7] = 64;
    p[8] = 0x20;
    p[9] = 0x01;
    p[10] = 0x0d;
    p[11] = 0xb8;
    p[24] = 0x20;
    p[25] = 0x01;
    p[26] = 0x0d;
    p[27] = 0xb8;
    p[40] = 128;
}

int main(void)
{
    unsigned char p[64];
    struct ipv6_parse_result r;

    basic_packet(p);
    assert(ipv6_parse(p, 48, &r) == IPV6_PARSE_OK);
    assert(r.header.version == 6);
    assert(r.header.payload_length == 8);
    assert(r.upper_layer_offset == 40);
    assert(r.upper_layer_protocol == 58);

    assert(ipv6_parse(p, 10, &r) == IPV6_PARSE_TOO_SHORT);

    p[0] = 0x40;
    assert(ipv6_parse(p, 48, &r) == IPV6_PARSE_BAD_VERSION);

    basic_packet(p);
    p[5] = 100;
    assert(ipv6_parse(p, 48, &r) == IPV6_PARSE_LENGTH_MISMATCH);

    /* Valid Fragment Header: 8-byte extension followed by ICMPv6. */
    memset(p, 0, sizeof(p));
    p[0] = 0x60;
    p[5] = 16;
    p[6] = 44;
    p[7] = 64;
    p[40] = 58;
    assert(ipv6_parse(p, 56, &r) == IPV6_PARSE_OK);
    assert(r.extension_count == 1);
    assert(r.upper_layer_offset == 48);
    assert(r.upper_layer_protocol == 58);

    /* Valid AH: (payload_len + 2) * 4 = 12 bytes. */
    memset(p, 0, sizeof(p));
    p[0] = 0x60;
    p[5] = 12;
    p[6] = 51;
    p[7] = 64;
    p[40] = 58;
    p[41] = 1;
    assert(ipv6_parse(p, 52, &r) == IPV6_PARSE_OK);
    assert(r.extension_count == 1);
    assert(r.upper_layer_offset == 52);
    assert(r.upper_layer_protocol == 58);

    /* Truncated AH must be rejected. */
    p[5] = 12;
    assert(ipv6_parse(p, 48, &r) == IPV6_PARSE_LENGTH_MISMATCH);

    return 0;
}
