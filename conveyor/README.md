# conveyor — تسمه‌نقاله (ICPC NAC 2025 F, Mob Grinder)

- نوع: Batch با grader (C++)، checker اختصاصی (testlib). TL = 1s، ML = 256 MiB.
- تابع: `std::vector<std::string> design(int N, int M, int U, int R, int D, int L)`؛ آرایه‌ی خالی یعنی «ممکن نیست».
- ایده: ممکن است ⇔ $U\ge N-1$ و $R\ge M-1$ (برش)؛ ساخت با یک مسیر پله‌ای: بالا-چپ D/R، پایین-راست U/L. $O(NM)$.
- grader secret ندارد عمداً: checker همه‌چیز را از روی ورودی بررسی می‌کند (تعدادها، `*`، خروج از جدول، دور)، پس جعل خروجی معادل حل مسئله است؛ «impossible» فقط وقتی قبول است که جواب داور هم impossible باشد.

## زیرمسئله‌ها

| # | امتیاز | قید |
|---|---|---|
| 0 | 0 | نمونه‌ها |
| 1 | 9 | $NM\le 10$، $T\le 100$ |
| 2 | 11 | $D=L=0$ |
| 3 | 17 | $N\le 2$ |
| 4 | 23 | $U\le N-1$، $R\le M-1$ |
| 5 | 40 | — |

## invoke (همه match with expected)

| راه‌حل | 0 | 1 | 2 | 3 | 4 | 5 | امتیاز در CMS |
|---|---|---|---|---|---|---|---|
| model, alt | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | 100 |
| brute | ✓ | ✓ | WA | WA | WA | WA | 9 |
| sub_ur | WA | WA | ✓ | WA | WA | WA | 11 |
| sub_two | WA | WA | WA | ✓ | WA | WA | 17 |
| sub_dl | WA | WA | WA | WA | ✓ | WA | 23 |
| slow | ✓ | ✓ | TLE | TLE | TLE | TLE | 9 |
| wrong_ronly | WA | WA | WA | WA | WA | WA | 0 |
| wrong_sum | WA | WA | WA | WA | ✓ | WA | 23 |
| wrong_offby1 | ✓ | WA | WA | ✓ | WA | WA | 17 |
| wrong_greedy | WA | WA | ✓ | WA | WA | WA | 11 |
| public/cpp/conveyor.cpp (قالب) | | | | | | | 0 |

زمان بدترین تست راه‌حل اصلی: 0.085s (`line-02`، صدهزار فراخوانی 1×1). کندترین تستِ slow در هر زیرمسئله‌ای که باید رد شود ≥ 2.8s.
