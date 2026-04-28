#include "unity.h"
#include "utils/fs.h"

void setUp(void) {}
void tearDown(void) {}

void test_join_path_basic(void) {
  char buf[64];
  TEST_ASSERT_EQUAL_INT(0, vls_join_path(buf, sizeof buf, "/a", "b"));
  TEST_ASSERT_EQUAL_STRING("/a/b", buf);
}

void test_join_path_dir_with_trailing_slash(void) {
  char buf[64];
  TEST_ASSERT_EQUAL_INT(0, vls_join_path(buf, sizeof buf, "/a/", "b"));
  TEST_ASSERT_EQUAL_STRING("/a/b", buf);
}

void test_join_path_root_dir(void) {
  char buf[64];
  TEST_ASSERT_EQUAL_INT(0, vls_join_path(buf, sizeof buf, "/", ".vls"));
  TEST_ASSERT_EQUAL_STRING("/.vls", buf);
}

void test_join_path_aliased(void) {
  char buf[64] = ".vls/stage";
  TEST_ASSERT_EQUAL_INT(0, vls_join_path(buf, sizeof buf, buf, "stage.json"));
  TEST_ASSERT_EQUAL_STRING(".vls/stage/stage.json", buf);
}

void test_join_path_overflow(void) {
  char buf[5];
  TEST_ASSERT_EQUAL_INT(-1, vls_join_path(buf, sizeof buf, "/a", "bbbb"));
}

void test_join_path_exactly_fits(void) {
  char buf[5];
  TEST_ASSERT_EQUAL_INT(0, vls_join_path(buf, sizeof buf, "/a", "b"));
  TEST_ASSERT_EQUAL_STRING("/a/b", buf);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_join_path_basic);
  RUN_TEST(test_join_path_dir_with_trailing_slash);
  RUN_TEST(test_join_path_root_dir);
  RUN_TEST(test_join_path_aliased);
  RUN_TEST(test_join_path_overflow);
  RUN_TEST(test_join_path_exactly_fits);
  return UNITY_END();
}
