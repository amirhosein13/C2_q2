#include "testlib.h"
#include <string>
using namespace std;

const int MAXT = 100000;
const int MAXNM = 100000;

int main(int argc, char* argv[]) {
	registerValidation(argc, argv);
	string g = validator.group();
	bool small = g == "small", onlyUR = g == "onlyUR", tworows = g == "tworows", minUR = g == "minUR";
	if (!(g == "" || g == "samples" || g == "full" || small || onlyUR || tworows || minUR))
		quitf(_fail, "unknown group '%s'", g.c_str());

	int T = inf.readInt(1, small ? 100 : MAXT, "T");
	inf.readEoln();
	long long sum = 0;
	for (int t = 0; t < T; t++) {
		int N = inf.readInt(1, MAXNM, "N"); inf.readSpace();
		int M = inf.readInt(1, MAXNM, "M"); inf.readSpace();
		long long nm = (long long)N * M;
		ensuref(nm <= MAXNM, "N*M > %d", MAXNM);
		sum += nm;
		ensuref(sum <= MAXNM, "sum of N*M > %d", MAXNM);
		int lim = (int)nm - 1;
		int U = inf.readInt(0, lim, "U"); inf.readSpace();
		int R = inf.readInt(0, lim, "R"); inf.readSpace();
		int D = inf.readInt(0, lim, "D"); inf.readSpace();
		int L = inf.readInt(0, lim, "L"); inf.readEoln();
		ensuref((long long)U + R + D + L == nm - 1, "U+R+D+L != N*M-1");
		if (small) ensuref(nm <= 10, "N*M > 10");
		if (onlyUR) ensuref(D == 0 && L == 0, "D, L must be 0");
		if (tworows) ensuref(N <= 2, "N > 2");
		if (minUR) ensuref(U <= N - 1 && R <= M - 1, "U > N-1 or R > M-1");
	}
	inf.readEof();
}
