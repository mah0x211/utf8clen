# utf8clen

[![test](https://github.com/mah0x211/utf8clen/actions/workflows/test.yml/badge.svg)](https://github.com/mah0x211/utf8clen/actions/workflows/test.yml)
[![codecov](https://codecov.io/gh/mah0x211/utf8clen/branch/master/graph/badge.svg)](https://codecov.io/gh/mah0x211/utf8clen)

utf8clen is a header-only library for getting the byte length of a UTF-8 character.

## Overview

This library provides a single inline function that calculates the byte length of a UTF-8 character and validates its format according to the Unicode Standard Version 15.0 specifications. It properly handles all valid UTF-8 sequences and detects illegal sequences.

## Features

- Header-only implementation - no compilation needed
- Strict Unicode 15.0 compliant validation
- Detailed error reporting
- No external dependencies
- Thoroughly tested with 100% code coverage


## Installation

Since utf8clen is a header-only library, simply copy the `utf8clen.h` file to your project or include it from this repository.

```bash
# Clone the repository
git clone https://github.com/mah0x211/utf8clen.git

# Copy the header file to your include path
cp utf8clen/src/utf8clen.h /path/to/your/project/includes/
```

## Usage

```c
#include <stdio.h>
#include "utf8clen.h"

int main() {
    const unsigned char *text = (const unsigned char *)"Hello, 世界!";
    size_t pos = 0;
    size_t illlen = 0;
    size_t len = 0;
    
    // Process each UTF-8 character
    while (text[pos]) {
        len = utf8clen(&text[pos], &illlen);
        
        if (len == 0) {
            // Handle invalid UTF-8 sequence
            printf("Invalid UTF-8 sequence at position %zu (length: %zu)\n", pos, illlen);
            pos += illlen;
        } else if (len == SIZE_MAX) {
            // Handle error
            printf("Error: %s\n", strerror(errno));
            return 1;
        } else {
            // Process valid UTF-8 character
            printf("Character at position %zu has length: %zu bytes\n", pos, len);
            pos += len;
        }
    }
    
    return 0;
}
```


## API Reference


### size_t utf8clen(const unsigned char *s, size_t *illlen)

Determines the byte length of a single UTF-8 character.

**Arguments**

- `s`: Pointer to a NULL-terminated string containing a UTF-8 character
- `illlen`: Pointer to a size_t that will receive the number of illegal bytes if an invalid UTF-8 sequence is detected

**Return Value**

- `1-4`: The length of the UTF-8 character in bytes if valid
- `0`: The UTF-8 sequence is invalid (illlen is set to the number of illegal bytes)
- `SIZE_MAX`: Parameters are invalid (errno is set to EINVAL)


### UTF-8 Validation Rules

The function follows the Unicode Standard Version 15.0 (Table 3-7) for well-formed UTF-8 byte sequences:

- 1 byte: 00-7F (ASCII)
- 2 bytes: C2-DF 80-BF
- 3 bytes:
    - E0: A0-BF 80-BF
    - E1-EC: 80-BF 80-BF
    - ED: 80-9F 80-BF (to exclude surrogate pairs)
    - EE-EF: 80-BF 80-BF
- 4 bytes:
    - F0: 90-BF 80-BF 80-BF
    - F1-F3: 80-BF 80-BF 80-BF
    - F4: 80-8F 80-BF 80-BF (to not exceed U+10FFFF)

Invalid sequences include:

- Bytes outside valid starting byte ranges (C0, C1, F5-FF)
- Missing or invalid continuation bytes
- Overlong encodings
- UTF-16 surrogate code points (U+D800-U+DFFF)
- Values outside Unicode range (>U+10FFFF)


## Testing

To run the tests and generate coverage reports:

```bash
git clone https://github.com/mah0x211/utf8clen.git
cd utf8clen
make coverage
```

Required tools:

- gcc
- make
- lcov (for coverage reports)


## License

This library is released under the MIT License - see the LICENSE file for details.


## References

- [Unicode Standard Version 15.0](https://www.unicode.org/versions/Unicode15.0.0/)
- [UTF-8 Encoding Table](https://www.unicode.org/versions/Unicode15.0.0/ch03.pdf) (Table 3-7)
