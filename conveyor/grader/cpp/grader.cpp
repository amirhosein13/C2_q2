#include "conveyor.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

// Input:  T, then T lines "N M U R D L".
// Output: for each call, a line with k = number of returned rows,
//         followed by the k rows.
//         k = -1 means a returned row was empty or had a character
//         outside printable non-space ASCII (it cannot be printed safely).

static bool printable_row(const std::string& s) {
	if (s.empty()) return false;
	for (unsigned char c : s)
		if (c < 33 || c > 126) return false;
	return true;
}

int main() {
	int T;
	if (scanf("%d", &T) != 1) { fprintf(stderr, "grader: bad input\n"); return 1; }
	std::vector<int> q(6 * (size_t)T);
	for (int i = 0; i < 6 * T; i++)
		if (scanf("%d", &q[i]) != 1) { fprintf(stderr, "grader: bad input\n"); return 1; }
	fclose(stdin);

	for (int t = 0; t < T; t++) {
		const int* a = &q[6 * (size_t)t];
		std::vector<std::string> res = design(a[0], a[1], a[2], a[3], a[4], a[5]);
		bool ok = true;
		for (const std::string& row : res)
			if (!printable_row(row)) { ok = false; break; }
		if (!ok) {
			printf("-1\n");
			continue;
		}
		printf("%d\n", (int)res.size());
		for (const std::string& row : res) {
			fwrite(row.data(), 1, row.size(), stdout);
			putchar('\n');
		}
	}
	fclose(stdout);
	return 0;
}
