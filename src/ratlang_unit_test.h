#ifndef RATLANG_UNIT_TEST_H
#define RATLANG_UNIT_TEST_H

#define test_begin int main (int argc, char** argv) { int __test__index = 0; \
	int __test__failed = 0; do {
#define test_finish  } while (0); return __test__failed != 0; }
#define test {
#define end_test }

#define mu_assert(message, test) do { int __status = (test); if (!__status) \
	__test__failed++; fprintf (stdout, (__status ? "-PASS: %d %s\n" \
	: "-FAIL: %d %s\n"), ++__test__index, message); } while (0)

#endif
