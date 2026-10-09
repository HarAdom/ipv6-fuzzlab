#include "ipv6_fuzzlab/parser.h"

#include <stddef.h>
#include <stdint.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    struct ipv6_parse_result result;
    (void)ipv6_parse(data, size, &result);
    return 0;
}
