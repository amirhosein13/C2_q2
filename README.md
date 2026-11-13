# سوال‌های آزمون هفتگی (TPS)

هر سوال یک پوشه‌ی TPS است. TPS نسخه‌ی `ioi-2017/tps` commit `fd2b587` (اسکریپت‌ها داخل هر سوال کپی شده‌اند).

## conveyor — تسمه‌نقاله (ICPC NAC 2025 F, Mob Grinder)

- نوع: Batch با grader، checker اختصاصی، فقط C++.
- متن: `conveyor/statement/index.md` و `conveyor/statement/conveyor.pdf` (ساخت دوباره: `conveyor/statement/build-pdf.sh`).
- خروجی آماده‌ی CMS: `export/conveyor-CMS.zip` (پروتکل 1، متن PDF در `statements/fa.pdf` اضافه شده).

### Import در CMS

```bash
scp export/conveyor-CMS.zip  cms-server:
ssh cms-server
unzip conveyor-CMS.zip            # پوشه‌ی conveyor/ ساخته می‌شود
cmsImportTask -L tps_task conveyor                  # فقط ساخت task
cmsImportTask -L tps_task -c <contest_id> conveyor  # یا مستقیم اضافه به یک contest
# اگر task قبلاً import شده: -u
```

نکته‌ها:
- loader روی سرور `checker/checker.cpp` را با `g++ -static` کامپایل می‌کند؛ `g++` و کتابخانه‌های static (`libstdc++` static) باید روی سرور باشد.
- loader زبان PDF را از اسم فایل برمی‌دارد (`fa`) و primary statement را فقط برای `en_US` ست می‌کند؛ در پنل admin زبان اصلی متن را `fa` کنید.
- زمان‌بندی روی سرور CMS دوباره چک شود (راه‌حل اصلی روی این ماشین حداکثر 0.03 ثانیه، محدودیت 1 ثانیه).

### ساخت دوباره‌ی خروجی

```bash
export/make-export.sh conveyor    # tps gen + tps export CMS 1 + افزودن statements/fa.pdf → export/conveyor-CMS.zip
```

### بررسی‌ها

```bash
cd conveyor
tps verify
tps invoke solution/<sol>.cpp        # حکم مورد انتظار هر راه‌حل در solutions.json
bash checker-tests/run.sh            # تست‌های دست‌ساز checker
```
