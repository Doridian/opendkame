from sys import argv
fh_in = open(argv[1], 'r')

SUFFIX = None
PREFIX = '101010101010101010101010000000000' # 23 TE 50% preamble, 10 TE 0% header
CODES = []
code_set = set()
for line in fh_in.readlines():
    line = line.strip()
    if not line:
        continue
    code = '{:066b}'.format(int(line))
    if code in code_set:
        print("Saw repeat after %d codes!" % len(CODES))
        break

    code_set.add(code)

    if not SUFFIX:
        SUFFIX = code[32:]

    if code[32:] != SUFFIX:
        raise ValueError("Inconsistent suffix")

    CODES.append(code[:32])

fh_in.close()

fh_out = open('include/codes.h', 'w')
fh_out.write(f'''#pragma once
#include <Arduino.h>

#define PREFIX_BITS {len(PREFIX)}
#define PREFIX 0b{PREFIX}

#define SUFFIX_BITS {len(SUFFIX)}
#define SUFFIX 0b{SUFFIX}

#define CODE_BITS {len(CODES[0])}
#define CODE_COUNT {len(CODES)}

extern const uint64_t PROGMEM CODES[];
''')
fh_out.close()

fh_out = open('src/codes.cpp', 'w')
fh_out.write(f'''#include <Arduino.h>
#include "codes.h"

const uint64_t PROGMEM CODES[] = {'{'}
''')
for code in CODES:
    fh_out.write(f'\t0b{code},\n')
fh_out.write('};\n')
fh_out.close()