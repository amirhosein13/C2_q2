#!/bin/bash
# Runs the checker on hand-made contestant outputs and compares with the expected verdict.
set -u
cd "$(dirname "$0")"
g++ -std=gnu++14 -O2 -o /tmp/conveyor_chk ../checker.cpp || exit 1
pass=0; fail=0
t() { # name expected(1|0|FAIL) content [input] [answer]
	local in="${4:-in.txt}" ans="${5:-ans.txt}"
	printf -- "$3" > /tmp/conveyor_out
	/tmp/conveyor_chk "$in" "$ans" /tmp/conveyor_out > /tmp/conveyor_so 2> /tmp/conveyor_se
	local score verdict got
	score="$(sed -n 1p /tmp/conveyor_so)"; verdict="$(sed -n 1p /tmp/conveyor_se)"
	got="$score"; [[ "$verdict" == Judge* ]] && got="FAIL"
	if [ "$got" == "$2" ]; then pass=$((pass+1)); r=ok; else fail=$((fail+1)); r=MISMATCH; fi
	printf "%-9s %-32s expect=%-4s got=%-4s %s\n" "$r" "$1" "$2" "$got" "$(sed -n 2p /tmp/conveyor_se)"
}
# in.txt: 3 cases: (2x2, U1 R1 D1 L0), (1x1), (2x2, U0 R2 D0 L1 -> impossible)
t "same as jury"               1 '2\nD*\nRU\n1\n*\n0\n'
t "trailing whitespace"        1 '2\nD*  \nRU\n1\n*\n0'
t "points outside grid"        0 '2\nR*\nDU\n1\n*\n0\n'
t "wrong counts"               0 '2\nR*\nUU\n1\n*\n0\n'
t "bad character"              0 '2\nX*\nRU\n1\n*\n0\n'
t "lowercase"                  0 '2\nd*\nRU\n1\n*\n0\n'
t "missing star"               0 '2\nDR\nRU\n1\n*\n0\n'
t "short row"                  0 '2\nD\nRU\n1\n*\n0\n'
t "long row"                   0 '2\nD*R\nRU\n1\n*\n0\n'
t "wrong k"                    0 '1\nD*\n1\n*\n0\n'
t "k = -1 (unprintable row)"   0 '-1\n1\n*\n0\n'
t "false impossible"           0 '0\n1\n*\n0\n'
t "design for impossible"      0 '2\nD*\nRU\n1\n*\n2\nR*\nRL\n'
t "truncated"                  0 '2\nD*\nRU\n1\n'
t "empty output"               0 ''
t "extra output"               0 '2\nD*\nRU\n1\n*\n0\nhello\n'
t "non-integer k"              0 'abc\n'
t "huge k"                     0 '99999999999\n'
t "negative k"                 0 '-5\n'
# cycle with correct counts: 2x3, U1 R3 D0 L1
printf '1\n2 3 1 3 0 1\n' > /tmp/conveyor_in2; printf '2\nRR*\nRRU\n' > /tmp/conveyor_ans2
t "cycle (counts ok)"          0 '2\nRR*\nRLU\n' /tmp/conveyor_in2 /tmp/conveyor_ans2
# different valid design than jury: 2x3, U2 R2 D0 L1
printf '1\n2 3 2 2 0 1\n' > /tmp/conveyor_in3; printf '2\nRR*\nULU\n' > /tmp/conveyor_ans3
t "different valid design"     1 '2\nRR*\nUUL\n' /tmp/conveyor_in3 /tmp/conveyor_ans3
# contestant valid but jury said impossible -> judge failure
printf '1\n2 2 1 1 1 0\n' > /tmp/conveyor_in4; printf '0\n' > /tmp/conveyor_ans4
t "better than jury"           FAIL '2\nD*\nRU\n' /tmp/conveyor_in4 /tmp/conveyor_ans4
echo "passed=$pass failed=$fail"
[ "$fail" -eq 0 ]
