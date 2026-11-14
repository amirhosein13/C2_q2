# راهنمای کامل آپلود سوال «تسمه‌نقاله» (`conveyor`) در CMS 1.5.1 — فقط از پنل ادمین

همه‌ی کارهایی که باید انجام دهید، به ترتیب. مقدارها و برچسب‌ها از کد CMS نسخه‌ی v1.5.1 بررسی شده‌اند.

---

## قدم ۰: فایل‌های لازم را آماده کنید

به این ۸ فایل نیاز دارید (همه در پوشه‌ی `conveyor/cms-upload/`):

| فایل | کاربرد |
|---|---|
| `statement-fa.pdf` | متن فارسی سوال |
| `attachments/conveyor.zip` | بسته‌ی شرکت‌کننده‌ها (grader نمونه، هدر، قالب کد، نمونه‌ها) |
| `managers/checker` | checker کامپایل‌شده (لینوکس x86-64، static) |
| `managers/grader.cpp` | grader داوری |
| `managers/conveyor.h` | هدر |
| `testcases-part1.zip` | هر ۶۹ تست (۰٫۲۵ مگابایت) |
| `score_type_parameters.json` | پارامترهای زیرمسئله‌ها (محتوایش پایین همین فایل هم هست) |
| `SETTINGS.md` | خلاصه‌ی تنظیمات |

**ساده‌ترین راه:** فایل `conveyor-cms-upload.zip` را از ریشه‌ی ریپو (شاخه‌ی `ccr-d4f5319e-f5pjzs`) دانلود و باز کنید. همه‌چیز داخل آن است:
- همین ۸ فایل
- همین راهنما
- پوشه‌ی `test-solutions/` با راه‌حل‌های ارسال آزمایشی (قدم ۷)

اگر خواستید بسته را خودتان دوباره بسازید، روی یک لینوکس:

```bash
git clone -b ccr-d4f5319e-f5pjzs https://github.com/amirhosein13/C2_q2
cd C2_q2
git submodule update --init                     # پوشه‌ی tps/ را پر می‌کند
(cd tps && sudo ./install-tps.sh)               # فرمان tps
pip install psutil && sudo apt install dos2unix
cd conveyor
tps gen                                          # ساخت ۶۹ تست
statement/build-pdf.sh                           # PDF (به pandoc و node + playwright نیاز دارد)
python3 ../tools/cms_upload.py --pdf statement/conveyor-fa.pdf
# خروجی: conveyor/cms-upload/
```

---

## قدم ۱: ساخت سوال

1. وارد پنل ادمین شوید. حساب شما باید دسترسی **all** داشته باشد.
2. منوی چپ → **Tasks** → ساخت سوال جدید.
3. name را `conveyor` بگذارید و بسازید.

CMS خودش یک dataset به اسم `Default` می‌سازد با تنظیمات Batch، alone، diff و Score type = Sum. این‌ها را در قدم‌های بعد درست می‌کنیم.

---

## قدم ۲: بالای صفحه‌ی سوال (Task configuration)

| فیلد پنل | مقدار |
|---|---|
| Name | `conveyor` |
| Title | `تسمه‌نقاله` |
| Statements → [add] | Language code: `fa`، فایل `statement-fa.pdf` → Upload. بعد تیک **Primary?** کنار `fa` |
| Attachments → [add] | فایل `attachments/conveyor.zip` → Upload |
| Submission format | `conveyor.%l` (معمولاً خودکار پر شده) |
| Feedback level | `Full` (پیش‌فرض CMS `Restricted` است) |
| Token mode | `Disabled` |
| Maximum number of submissions | `50` |
| Maximum number of user tests | خالی |
| Minimum interval between submissions | `60` |
| Minimum interval between user tests | خالی |
| Score decimal places | `2` |
| **Score mode** | **Use the sum over each subtask of the best result for that subtask across all submissions (IOI 2017-)** |

> **مهم:** پیش‌فرض Score mode در CMS گزینه‌ی IOI 2010-2012 است. حتماً گزینه‌ی IOI 2017- را انتخاب کنید.

---

## قدم ۳: بخش dataset (پایین همان صفحه)

| فیلد پنل | مقدار |
|---|---|
| Time limit | `1` second(s) |
| Memory limit | `256` MiB |
| Task type | `Batch` |
| Compilation | `Submissions are compiled with a grader` |
| I/O (blank for stdin/stdout): Input file و Output file | هر دو **خالی** |
| Output evaluation | `Outputs are compared by a comparator` |
| Managers → [add] (سه بار، یکی‌یکی) | `checker`، `grader.cpp`، `conveyor.h` |
| Score Type | `GroupMin` |

اسم managerها نباید عوض شود. CMS دقیقاً دنبال `checker` و `grader.cpp` می‌گردد.

در این مرحله **Score Parameters را هنوز تغییر ندهید**. بعد دکمه‌ی **Update** پایین صفحه را بزنید.

---

## قدم ۴: آپلود تست‌ها

در بخش dataset → **Add multiple testcases**:

- Archive: `testcases-part1.zip`
- Input file names template: `*.in` (پیش‌فرض پنل `input.*` است؛ حتماً عوض کنید)
- Output file names template: `*.out` (پیش‌فرض `output.*`)
- Public: بدون تیک
- Overwrite: بدون تیک
- Upload

در پایان باید **۶۹ تست** در لیست باشد. اسم‌ها شبیه `0-01`، `1-01`، … و `line-01` تا `line-04` هستند.

---

## قدم ۵: پارامترهای زیرمسئله‌ها

> این قدم باید **بعد از آپلود تست‌ها** انجام شود. CMS هنگام Update پارامترها را با تست‌های موجود چک می‌کند و اگر تستی پیدا نکند، خطای «No testcase matches against the regexp» می‌دهد.

1. Score Type: `GroupMin`
2. در جعبه‌ی **Score Parameters** کل متن زیر را paste کنید. این همان محتوای `score_type_parameters.json` است:

```
[[0, "^(0\\-01|0\\-02)$"], [9, "^(1\\-01|1\\-02|1\\-03|1\\-04|1\\-05|1\\-06|1\\-07|1\\-08|1\\-09|1\\-10|1\\-11|1\\-12|1\\-13|1\\-14|1\\-15)$"], [11, "^(2\\-01|2\\-02|2\\-03|2\\-04|2\\-05|2\\-06|2\\-07|2\\-08|2\\-09|2\\-10|2\\-11|line\\-01|line\\-02|line\\-03|line\\-04)$"], [17, "^(3\\-01|3\\-02|3\\-03|3\\-04|3\\-05|3\\-06|3\\-07|3\\-08|3\\-09|3\\-10|line\\-01|line\\-02|line\\-03|line\\-04)$"], [23, "^(4\\-01|4\\-02|4\\-03|4\\-04|4\\-05|4\\-06|4\\-07|4\\-08|4\\-09|line\\-01|line\\-02|line\\-03|line\\-04)$"], [40, "^(0\\-01|0\\-02|1\\-01|1\\-02|1\\-03|1\\-04|1\\-05|1\\-06|1\\-07|1\\-08|1\\-09|1\\-10|1\\-11|1\\-12|1\\-13|1\\-14|1\\-15|2\\-01|2\\-02|2\\-03|2\\-04|2\\-05|2\\-06|2\\-07|2\\-08|2\\-09|2\\-10|2\\-11|3\\-01|3\\-02|3\\-03|3\\-04|3\\-05|3\\-06|3\\-07|3\\-08|3\\-09|3\\-10|4\\-01|4\\-02|4\\-03|4\\-04|4\\-05|4\\-06|4\\-07|4\\-08|4\\-09|5\\-01|5\\-02|5\\-03|5\\-04|5\\-05|5\\-06|5\\-07|5\\-08|5\\-09|5\\-10|5\\-11|5\\-12|5\\-13|5\\-14|5\\-15|5\\-16|5\\-17|5\\-18|line\\-01|line\\-02|line\\-03|line\\-04)$"]]
```

3. **Update** را بزنید.

زیرمسئله‌هایی که با این پارامترها تعریف می‌شوند:

| زیرمسئله | امتیاز | قید | تعداد تست |
|---|---|---|---|
| ۰ | ۰ | نمونه‌ها | ۲ |
| ۱ | ۹ | $NM\le10$ و $T\le100$ | ۱۵ |
| ۲ | ۱۱ | $D=L=0$ | ۱۵ |
| ۳ | ۱۷ | $N\le2$ | ۱۴ |
| ۴ | ۲۳ | $U\le N-1$ و $R\le M-1$ | ۱۳ |
| ۵ | ۴۰ | بدون قید اضافه | ۶۹ |

جمع امتیازها: ۱۰۰.

---

## قدم ۶: افزودن سوال به آزمون

1. صفحه‌ی آزمون → **Tasks** → افزودن `conveyor`.
2. در تنظیمات آزمون، **Allowed languages** فقط C++ باشد. grader سوال فقط C++ دارد.
3. اگر آزمون جدید است یا عوضش کرده‌اید، از مدیر سرور بخواهید سرویس‌ها را برای آن آزمون ری‌استارت کند.

---

## قدم ۷: ارسال‌های آزمایشی (حتماً قبل از آزمون)

با یک کاربر آزمایشی که در participation تیک **Hidden** خورده، این فایل‌ها را ارسال کنید. همه در پوشه‌ی `test-solutions/` داخل `conveyor-cms-upload.zip` هستند (و در `conveyor/solution/` ریپو). قالب خالی در zip اسم `empty_template.cpp` دارد. هر کدام را با اسم `conveyor.cpp` ارسال کنید.

| فایل | امتیاز مورد انتظار |
|---|---|
| `model.cpp` | **۱۰۰** |
| `alt.cpp` | ۱۰۰ |
| `sub_dl.cpp` | ۲۳ |
| `wrong_sum.cpp` | ۲۳ |
| `sub_two.cpp` | ۱۷ |
| `wrong_offby1.cpp` | ۱۷ |
| `sub_ur.cpp` | ۱۱ |
| `wrong_greedy.cpp` | ۱۱ |
| `brute.cpp` | ۹ |
| `slow.cpp` | ۹ (TLE در زیرمسئله‌های ۲ تا ۵) |
| `wrong_ronly.cpp` | ۰ |
| `public/cpp/conveyor.cpp` (قالب خالی) | ۰، بدون خطای کامپایل |

اگر نتیجه فرق داشت:
- **همه‌ی ارسال‌ها Compilation failed:** زبان مجاز آزمون یا اسم managerها (`grader.cpp`، `conveyor.h`) را چک کنید.
- **همه ۰ با پیام checker:** فایل `checker` درست آپلود نشده یا Output evaluation روی comparator نیست.
- **خطای «No testcase matches»:** تست‌ها کامل آپلود نشده‌اند (قدم ۴) یا الگوها `*.in` و `*.out` نبوده‌اند.

همچنین با همین کاربر در صفحه‌ی شرکت‌کننده‌ها چک کنید که PDF متن باز می‌شود و `conveyor.zip` دانلود می‌شود.

---

## قدم ۸: زمان‌ها را روی سرور چک کنید

در پنل ادمین، صفحه‌ی ارسال `model.cpp` زمان هر تست را نشان می‌دهد:
- روی ماشین آزمایش، بدترین تست ۰٫۰۸۷ ثانیه بود. محدودیت ۱ ثانیه است.
- `slow.cpp` باید در زیرمسئله‌های ۲ تا ۵ TLE بگیرد. روی ماشین آزمایش بدترین تستش در هر زیرمسئله دست‌کم ۲٫۸ ثانیه طول می‌کشید.

اگر روی سرور `model.cpp` به بیش از حدود ۰٫۵ ثانیه رسید، یا `slow.cpp` جایی قبول شد، TL را بازبینی کنید. برای این کار در dataset **Clone** بزنید، در کپی TL را عوض کنید و آن را **Activate** کنید.

---

## چیزهایی که تأیید نشده‌اند

- **آپلود واقعی:** روی یک CMS 1.5.1 واقعی امتحان نشده است. loader رسمی CMS بدون پایگاه داده روی بسته اجرا شد و همه‌ی مقدارها یکی بودند. فرم‌های پنل از روی کد و قالب‌های CMS بررسی شدند.
- **زمان‌ها:** روی ماشین آزمایش اندازه‌گیری شده‌اند، نه روی سرور شما.
- **معماری سرور:** `checker` برای لینوکس x86-64 ساخته شده. اگر سرور معماری دیگری دارد، باید همان‌جا کامپایل شود.

---

## عیب‌یابی: ارسال‌ها روی «Compiling...» می‌مانند

این وضعیت یعنی هیچ Worker کامپایل را **تمام نکرده** است. فرق دارد با «Compilation failed» که یعنی کامپایل انجام شده و کد خطا داشته. طبق کد CMS 1.5.1 دو علت اصلی دارد.

**اول این‌ها را در پنل نگاه کنید:** صفحه‌ی اول پنل (**Overview**)، سه بخش:
- **Workers status**: اگر «No workers found» است، یا Workerها وصل نیستند → علت ب.
- **Queue status**: اگر ارسال‌ها مدام در صف می‌مانند و کم نمی‌شوند → علت ب.
- **Logs**: دنبال یکی از این پیام‌ها بگردید:
  - `Configuration error: dataset is missing manager 'grader.cpp'` → علت الف
  - `Worker failed when compiling submission ...`
  - `Maximum number of failures reached for the compilation of submission ...`

### الف. اسم managerها درست نیست (مربوط به همین سوال؛ محتمل‌ترین)
اگر `grader.cpp` با اسم دقیق در dataset نباشد، CMS آن را «خطای تنظیمات» حساب می‌کند، نه خطای کامپایل.
۳ بار دوباره تلاش می‌کند و بعد ارسال **برای همیشه** روی «Compiling...» می‌ماند.

این اتفاق معمولاً وقتی می‌افتد که مرورگر هنگام دانلود اسم فایل را عوض کرده باشد، مثلاً `grader (1).cpp`، `grader.cpp.txt` یا `conveyor (1).h`.

رفع:
1. صفحه‌ی سوال → بخش dataset → **Managers**.
2. اسم‌ها باید **دقیقاً** `grader.cpp`، `conveyor.h` و `checker` باشند. هر اسم دیگری را Delete کنید.
3. فایل را با اسم درست دوباره [add] کنید. اگر اسم فایل روی کامپیوتر عوض شده، قبل از آپلود rename کنید.
4. مطمئن شوید managerها در dataset **فعال** آپلود شده‌اند (همان که ارسال‌ها با آن داوری می‌شوند؛ معمولاً `Default`). اگر dataset را Clone کرده‌اید، managerها باید در همان dataset فعال باشند.

### ب. Worker یا سرویس‌ها اجرا نمی‌شوند (به سوال ربطی ندارد)
اگر در Workers status «No workers found» است، یا هیچ سوالی در هیچ آزمونی کامپایل نمی‌شود، مشکل از سرور است.
از مدیر سرور بخواهید:
- سرویس‌ها را برای همین آزمون ری‌استارت کند: `cmsResourceService -a <ID آزمون>`
- Workerها و EvaluationService را چک کند.
- sandbox (isolate) را چک کند.

### بعد از رفع مشکل
ارسال‌هایی که به سقف ۳ تلاش رسیده‌اند **خودکار دوباره کامپایل نمی‌شوند**. باید دستی دوباره داوری شوند:
- **برای همه:** صفحه‌ی dataset سوال → «Reevaluate all … submissions for this dataset» → دکمه‌ی **C** (Compilation).
- **برای یک ارسال:** صفحه‌ی همان ارسال → ستون Reevaluate → دکمه‌ی **C**.
