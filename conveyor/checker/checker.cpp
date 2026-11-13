#include "testlib.h"
#include <string>
#include <vector>
using namespace std;

// Grader output format, per test case:
//   k            (number of rows returned by design; -1 = unprintable row)
//   k rows
// The jury answer has the same format (k = 0 or k = N).

static const int MAX_K = 1000000;

// Returns an empty string if grid is a valid design, otherwise a reason.
static string validate(int N, int M, int U, int R, int D, int L,
                       const vector<string>& g) {
	long long cnt[4] = {0, 0, 0, 0};  // U R D L
	const char* dirs = "URDL";
	const int dr[4] = {-1, 0, 1, 0};
	const int dc[4] = {0, 1, 0, -1};
	vector<signed char> dir((size_t)N * M, -1);
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++) {
			char c = g[i][j];
			bool target = (i == 0 && j == M - 1);
			if (target) {
				if (c != '*') return "top-right cell must be '*'";
				continue;
			}
			int d = -1;
			for (int t = 0; t < 4; t++) if (dirs[t] == c) d = t;
			if (d < 0) return format("invalid character at row %d column %d", i, j);
			int ni = i + dr[d], nj = j + dc[d];
			if (ni < 0 || ni >= N || nj < 0 || nj >= M)
				return format("belt at row %d column %d points outside the grid", i, j);
			dir[(size_t)i * M + j] = (signed char)d;
			cnt[d]++;
		}
	if (cnt[0] != U || cnt[1] != R || cnt[2] != D || cnt[3] != L)
		return format("wrong number of belts: got U=%lld R=%lld D=%lld L=%lld",
		              cnt[0], cnt[1], cnt[2], cnt[3]);
	// 0 = unknown, 1 = on current walk, 2 = reaches target
	vector<char> state((size_t)N * M, 0);
	state[M - 1] = 2;
	vector<int> walk;
	for (int s = 0; s < N * M; s++) {
		if (state[s]) continue;
		walk.clear();
		int x = s;
		while (state[x] == 0) {
			state[x] = 1;
			walk.push_back(x);
			int d = dir[x];
			x = (x / M + dr[d]) * M + (x % M + dc[d]);
		}
		if (state[x] == 1)
			return format("cycle through row %d column %d", x / M, x % M);
		for (int y : walk) state[y] = 2;
	}
	return "";
}

int main(int argc, char* argv[]) {
	registerChecker("conveyor", argc, argv);
	int T = inf.readInt();
	for (int t = 1; t <= T; t++) {
		int N = inf.readInt(), M = inf.readInt();
		int U = inf.readInt(), R = inf.readInt(), D = inf.readInt(), L = inf.readInt();

		int jk = ans.readInt();
		if (jk != 0 && jk != N) quitf(_fail, "case %d: bad jury answer k=%d", t, jk);
		for (int i = 0; i < jk; i++) ans.readToken();

		int k = ouf.readInt(-1, MAX_K, format("k[%d]", t).c_str());
		if (k == -1) quitf(_wa, "case %d: returned a row that is empty or has invalid characters", t);
		if (k == 0) {
			if (jk != 0) quitf(_wa, "case %d: answered impossible, but a design exists", t);
			continue;
		}
		if (k != N) quitf(_wa, "case %d: returned %d rows, expected %d (or 0)", t, k, N);
		vector<string> g(N);
		for (int i = 0; i < N; i++) {
			g[i] = ouf.readToken();
			if ((int)g[i].size() != M)
				quitf(_wa, "case %d: row %d has length %d, expected %d", t, i, (int)g[i].size(), M);
		}
		string err = validate(N, M, U, R, D, L, g);
		if (!err.empty()) quitf(_wa, "case %d: %s", t, err.c_str());
		if (jk == 0) quitf(_fail, "case %d: contestant found a valid design but jury says impossible", t);
	}
	quitf(_ok, "all %d designs are correct", T);
}
