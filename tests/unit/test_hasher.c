#include "unity.h"
#include "utils/hasher.h"
#include "vls_types.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_hash_to_string_zero(void) {
  vls_md_hash_t h = {0};
  char out[33];
  TEST_ASSERT_EQUAL_INT(0, hash_to_string(&h, out));
  TEST_ASSERT_EQUAL_STRING("00000000000000000000000000000000", out);
}

void test_hash_to_string_known_bytes(void) {
  vls_md_hash_t h = {{0x00, 0x01, 0x0f, 0x10, 0xab, 0xcd, 0xef, 0xff, 0x12,
                      0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0}};
  char out[33];
  TEST_ASSERT_EQUAL_INT(0, hash_to_string(&h, out));
  TEST_ASSERT_EQUAL_STRING("00010f10abcdefff123456789abcdef0", out);
}

void test_hash_round_trip(void) {
  vls_md_hash_t a = {{0xde, 0xad, 0xbe, 0xef, 0x00, 0x11, 0x22, 0x33, 0x44,
                      0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb}};
  char buf[33];
  TEST_ASSERT_EQUAL_INT(0, hash_to_string(&a, buf));

  vls_md_hash_t b;
  TEST_ASSERT_EQUAL_INT(0, hash_from_string(buf, &b));
  TEST_ASSERT_EQUAL_MEMORY(a.bytes, b.bytes, MD_SIZE);
}

void test_hash_from_string_uppercase(void) {
  char in[] = "DEADBEEF00112233445566778899AABB";
  vls_md_hash_t h;
  TEST_ASSERT_EQUAL_INT(0, hash_from_string(in, &h));
  TEST_ASSERT_EQUAL_HEX8(0xde, h.bytes[0]);
  TEST_ASSERT_EQUAL_HEX8(0xbb, h.bytes[15]);
}

void test_hash_from_string_invalid_char(void) {
  char in[] = "zz000000000000000000000000000000";
  vls_md_hash_t h;
  TEST_ASSERT_EQUAL_INT(-1, hash_from_string(in, &h));
}

void test_hash_from_string_null_args(void) {
  vls_md_hash_t h;
  TEST_ASSERT_EQUAL_INT(-1, hash_from_string(NULL, &h));
  TEST_ASSERT_EQUAL_INT(-1, hash_from_string("00", NULL));
}

void test_is_identical_same(void) {
  vls_md_hash_t a = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
  vls_md_hash_t b = a;
  TEST_ASSERT_EQUAL_INT(0, is_identical(&a, &b));
}

void test_is_identical_diff(void) {
  vls_md_hash_t a = {{0}};
  vls_md_hash_t b = {{0}};
  b.bytes[15] = 1;
  TEST_ASSERT_NOT_EQUAL_INT(0, is_identical(&a, &b));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_hash_to_string_zero);
  RUN_TEST(test_hash_to_string_known_bytes);
  RUN_TEST(test_hash_round_trip);
  RUN_TEST(test_hash_from_string_uppercase);
  RUN_TEST(test_hash_from_string_invalid_char);
  RUN_TEST(test_hash_from_string_null_args);
  RUN_TEST(test_is_identical_same);
  RUN_TEST(test_is_identical_diff);
  return UNITY_END();
}
