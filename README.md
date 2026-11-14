# C2_q2 — سوال «تسمه‌نقاله» (TPS)

| مسیر | توضیح |
|---|---|
| `tps/` | git submodule از [ioi-2017/tps](https://github.com/ioi-2017/tps) روی commit `fd2b587` (اسکریپت‌ها 1.25) |
| `common/testlib.h` | testlib نسخه‌ی TPS (کپی از `tps/task-templates/default/checker/testlib.h`)؛ در `gen/`، `validator/`، `checker/` symlink شده |
| `conveyor/` | سوال (ICPC NAC 2025 F, Mob Grinder)؛ جزئیات در `conveyor/README.md` |
| `conveyor/statement/build-pdf.sh` | ساخت `conveyor-fa.pdf` از `index.md` (pandoc + KaTeX + Vazirmatn + Chromium) |
| `tools/cms_upload.py` | ساخت `conveyor/cms-upload/` برای آپلود دستی از پنل ادمین CMS 1.5.1 |
| `tools/cms_loader_check.py` | اجرای loader رسمی `tps_task` یک checkout از CMS روی export و مقایسه با بسته |

## راه‌اندازی (یک بار)

```bash
git submodule update --init                 # اگر tps/ خالی است
(cd tps && sudo ./install-tps.sh)           # فرمان tps
pip install psutil && sudo apt install dos2unix
# برای PDF: pandoc و node + playwright (با Chromium)
```

## ساخت بسته‌ی آپلود CMS

```bash
cd conveyor
tps gen                                                    # ۶۹ تست
statement/build-pdf.sh                                     # → statement/conveyor-fa.pdf
python3 ../tools/cms_upload.py --pdf statement/conveyor-fa.pdf   # → cms-upload/ (در git نیست)
# اختیاری، تأیید با loader واقعی CMS 1.5.1:
git clone --depth 1 --branch v1.5.1 https://github.com/cms-dev/cms /tmp/cms151
python3 ../tools/cms_loader_check.py /tmp/cms151 statement/conveyor-fa.pdf
```

**همه‌ی مراحل آپلود در پنل ادمین، قدم‌به‌قدم: [`CMS-UPLOAD-STEPS-fa.md`](CMS-UPLOAD-STEPS-fa.md).**

## بررسی سوال

```bash
cd conveyor
tps verify
tps invoke solution/<sol>.cpp       # حکم مورد انتظار همه در solutions.json
bash checker/tests/run.sh           # ۲۲ خروجی دست‌ساز برای checker
```
