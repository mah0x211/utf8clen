#include "../src/utf8clen.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test helper function
static void test_case(const char *desc, const unsigned char *input,
                      size_t expected_len, size_t expected_illlen)
{
    size_t illlen = 0;
    size_t len    = utf8clen(input, &illlen);

    if (len == expected_len && illlen == expected_illlen) {
        printf("PASS: %s\n", desc);
    } else {
        printf("FAIL: %s\n", desc);
        printf("  Expected len: %zu, got: %zu\n", expected_len, len);
        printf("  Expected illlen: %zu, got: %zu\n", expected_illlen, illlen);
        exit(1);
    }
}

// Test parameter error handling
static void test_parameter_errors(void)
{
    size_t illlen = 0;

    printf("\n=== Testing parameter errors ===\n");
    assert(utf8clen(NULL, &illlen) == SIZE_MAX && errno == EINVAL);
    printf("PASS: NULL string parameter\n");

    unsigned char test_str[] = "test";
    assert(utf8clen(test_str, NULL) == SIZE_MAX && errno == EINVAL);
    printf("PASS: NULL illlen parameter\n");
}

// Test 1-byte (ASCII) sequences
static void test_1byte_sequences(void)
{
    printf("\n=== Testing 1-byte UTF-8 sequences (ASCII) ===\n");

    // Valid ASCII characters
    test_case("ASCII character 'A'", (const unsigned char *)"A", 1, 0);
    test_case("ASCII character '1'", (const unsigned char *)"1", 1, 0);
    test_case("ASCII character ' '", (const unsigned char *)" ", 1, 0);
    test_case("ASCII character NUL", (const unsigned char *)"\0", 1, 0);

    // Illegal single bytes
    test_case("Illegal starting byte 0x80", (const unsigned char *)"\x80", 0,
              1);
    test_case("Illegal starting byte 0xBF", (const unsigned char *)"\xBF", 0,
              1);
    test_case("Continuation bytes without start byte",
              (const unsigned char *)"\x80\x80\x80", 0, 3);
}

// Test 2-byte sequences
static void test_2byte_sequences(void)
{
    printf("\n=== Testing 2-byte UTF-8 sequences ===\n");

    // Valid 2-byte sequences
    test_case("2-byte: Latin small letter a with acute (á)",
              (const unsigned char *)"\xC3\xA1", 2, 0);
    test_case("2-byte: Latin small letter e with acute (é)",
              (const unsigned char *)"\xC3\xA9", 2, 0);
    test_case("2-byte: Copyright sign (©)", (const unsigned char *)"\xC2\xA9",
              2, 0);

    // Valid 2-byte followed by another character
    test_case("Valid 2-byte followed by ASCII",
              (const unsigned char *)"\xC3\xA1Z", 2, 0);

    // Illegal 2-byte sequences
    test_case("Illegal 2-byte: missing continuation byte",
              (const unsigned char *)"\xC3", 0, 1);
    test_case("Illegal 2-byte: invalid continuation byte",
              (const unsigned char *)"\xC3\x00", 0, 1);
    test_case("Illegal 2-byte: continuation byte out of range",
              (const unsigned char *)"\xC3\xC0", 0, 2);
}

// Test 3-byte sequences
static void test_3byte_sequences(void)
{
    printf("\n=== Testing 3-byte UTF-8 sequences ===\n");

    // Valid E0 prefix 3-byte sequences
    test_case("3-byte: Thai character SO SO (ซ) with E0 prefix",
              (const unsigned char *)"\xE0\xB8\x8B", 3, 0);

    // Valid E1-EC prefix 3-byte sequences
    test_case("3-byte: Japanese Hiragana letter A (あ) with E3 prefix",
              (const unsigned char *)"\xE3\x81\x82", 3, 0);
    test_case("3-byte: Chinese character (中) with E4 prefix",
              (const unsigned char *)"\xE4\xB8\xAD", 3, 0);

    // Valid ED prefix 3-byte sequences
    printf("\n--- Testing ED prefix 3-byte sequences ---\n");
    test_case("3-byte: Character in range U+D000-U+D7FF",
              (const unsigned char *)"\xED\x80\x80", 3, 0);
    test_case("3-byte: Character at boundary U+D7FF",
              (const unsigned char *)"\xED\x9F\xBF", 3, 0);

    // Valid EE-EF prefix 3-byte sequences
    test_case("3-byte: Symbol with EE prefix",
              (const unsigned char *)"\xEE\x80\x80", 3, 0);
    test_case("3-byte: BOM marker with EF prefix",
              (const unsigned char *)"\xEF\xBB\xBF", 3, 0);

    // Illegal E0 prefix 3-byte sequences
    test_case("Illegal 3-byte: E0 with invalid second byte",
              (const unsigned char *)"\xE0\x70\x82", 0, 1);
    test_case("Illegal 3-byte: E0 with missing bytes",
              (const unsigned char *)"\xE0", 0, 1);

    // Illegal E1-EC prefix 3-byte sequences
    test_case("Illegal 3-byte: E3 with missing continuation byte",
              (const unsigned char *)"\xE3\x81", 0, 2);
    test_case("Illegal 3-byte: E3 with invalid third byte",
              (const unsigned char *)"\xE3\x81\x00", 0, 2);

    // Illegal ED prefix 3-byte sequences
    test_case("Illegal 3-byte: ED with surrogate range second byte (U+D800)",
              (const unsigned char *)"\xED\xA0\x80", 0, 3);
    test_case("Illegal 3-byte: ED with missing continuation bytes",
              (const unsigned char *)"\xED", 0, 1);

    // Illegal EE-EF prefix 3-byte sequences
    test_case("Illegal 3-byte: EE with invalid third byte",
              (const unsigned char *)"\xEE\x81\x00", 0, 2);
    test_case("Illegal 3-byte: EF with invalid second byte",
              (const unsigned char *)"\xEF\x00\x80", 0, 1);
}

// Test 4-byte sequences
static void test_4byte_sequences(void)
{
    printf("\n=== Testing 4-byte UTF-8 sequences ===\n");

    // Valid F0 prefix 4-byte sequences
    test_case("4-byte: Emoji face with tears of joy (😂) with F0 prefix",
              (const unsigned char *)"\xF0\x9F\x98\x82", 4, 0);
    test_case("4-byte: Musical note (𝄞) with F0 prefix",
              (const unsigned char *)"\xF0\x9D\x84\x9E", 4, 0);

    // Valid F1-F3 prefix 4-byte sequences
    printf("\n--- Testing F1-F3 prefix 4-byte sequences ---\n");
    test_case("4-byte: Character with F1 prefix",
              (const unsigned char *)"\xF1\x80\x80\x80", 4, 0);
    test_case("4-byte: Character with F2 prefix",
              (const unsigned char *)"\xF2\x80\x80\x80", 4, 0);
    test_case("4-byte: Character with F3 prefix",
              (const unsigned char *)"\xF3\x80\x80\x80", 4, 0);

    // Valid F4 prefix 4-byte sequences
    printf("\n--- Testing F4 prefix 4-byte sequences ---\n");
    test_case("4-byte: Character with F4 prefix",
              (const unsigned char *)"\xF4\x80\x80\x80", 4, 0);
    test_case("4-byte: Character at boundary U+10FFFF",
              (const unsigned char *)"\xF4\x8F\xBF\xBF", 4, 0);

    // Illegal F0 prefix 4-byte sequences
    test_case("Illegal 4-byte: F0 with invalid second byte",
              (const unsigned char *)"\xF0\x70\x98\x82", 0, 1);
    test_case("Illegal 4-byte: F0 with missing continuation bytes",
              (const unsigned char *)"\xF0\x9F\x98", 0, 3);
    test_case("Illegal 4-byte: F0 with invalid third byte",
              (const unsigned char *)"\xF0\x9F\x00\x82", 0, 2);
    test_case("Illegal 4-byte: F0 with invalid fourth byte",
              (const unsigned char *)"\xF0\x9F\x98\x00", 0, 3);

    // Illegal F1-F3 prefix 4-byte sequences
    test_case("Illegal 4-byte: F1 with invalid second byte",
              (const unsigned char *)"\xF1\x00\x80\x80", 0, 1);
    test_case("Illegal 4-byte: F2 with invalid third byte",
              (const unsigned char *)"\xF2\x80\x00\x80", 0, 2);
    test_case("Illegal 4-byte: F3 with invalid fourth byte",
              (const unsigned char *)"\xF3\x80\x80\x00", 0, 3);

    // Illegal F4 prefix 4-byte sequences
    test_case("Illegal 4-byte: F4 with invalid second byte (too high)",
              (const unsigned char *)"\xF4\x90\x80\x80", 0, 4);
    test_case("Illegal 4-byte: F4 with invalid third byte",
              (const unsigned char *)"\xF4\x80\x00\x80", 0, 2);
    test_case("Illegal 4-byte: F4 with invalid fourth byte",
              (const unsigned char *)"\xF4\x80\x80\x00", 0, 3);

    // Illegal starting byte beyond Unicode range
    test_case("Illegal starting byte 0xF5", (const unsigned char *)"\xF5", 0,
              1);
    test_case("Illegal starting byte 0xF8", (const unsigned char *)"\xF8", 0,
              1);
}

// Test special cases
static void test_special_cases(void)
{
    printf("\n=== Testing special cases ===\n");

    // Overlong encodings (redundant representations that are prohibited)
    test_case("Illegal overlong encoding of ASCII 'A'",
              (const unsigned char *)"\xC1\x81", 0, 2);
    test_case("Illegal overlong encoding of slash",
              (const unsigned char *)"\xE0\x80\xAF", 0, 3);

    // UTF-16 surrogate pairs (U+D800 to U+DFFF) are prohibited in UTF-8
    test_case("Illegal surrogate pair (U+D800)",
              (const unsigned char *)"\xED\xA0\x80", 0, 3);
    test_case("Illegal surrogate pair (U+DFFF)",
              (const unsigned char *)"\xED\xBF\xBF", 0, 3);
}

int main(void)
{
    // Run all test categories
    test_parameter_errors();
    test_1byte_sequences();
    test_2byte_sequences();
    test_3byte_sequences();
    test_4byte_sequences();
    test_special_cases();

    printf("\nAll tests passed successfully!\n");
    return 0;
}
