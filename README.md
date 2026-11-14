# C2_q2 — سوال‌های آزمون هفتگی دوره‌ی طلا (TPS)

قراردادها و درس‌ها: [`CLAUDE.md`](CLAUDE.md). ساختار هم‌سان با ریپوی `amirhosein13/c2_q1`.

| مسیر | توضیح |
|---|---|
| `tps/` | git submodule از [ioi-2017/tps](https://github.com/ioi-2017/tps) روی commit `fd2b587` (اسکریپت‌ها 1.25) |
| `common/testlib.h` | نسخه‌ی TPS از testlib؛ در `gen/`، `validator/` و `checker/` هر سوال symlink شده |
| `<task>/scripts` | symlink به `../tps/scripts` |
| `tools/statement/` | ساخت PDF فارسی راست‌به‌چپ از `statement/index.md` |
| `tools/make_cms_upload.py` | بسته‌ی آپلود دستی از پنل ادمین CMS |
| `tools/cms_loader_dryrun.py` | اجرای loader واقعی `tps_task` روی خروجی export، بدون پایگاه داده |
| `docs/cms-admin-only-fa.md` | راهنمای آپلود از پنل ادمین (CMS 1.5.1) |
| `conveyor/` | سوال «تسمه‌نقاله» (ICPC NAC 2025 F, Mob Grinder) |

## راه‌اندازی

```bash
git submodule update --init
(cd tps && sudo ./install-tps.sh)          # فرمان tps
pip install psutil && sudo apt install dos2unix
(cd tools/statement && npm install)        # فقط برای ساخت PDF متن
```

## کار با سوال

```bash
cd conveyor
tps gen                                    # ۶۹ تست
tps verify
tps invoke solution/model.cpp              # حکم مورد انتظار همه‌ی راه‌حل‌ها در solutions.json
bash checker/tests/run.sh                  # ۲۲ خروجی دست‌ساز برای checker
node ../tools/statement/build.js statement/index.md statement/conveyor-fa.pdf
python3 ../tools/make_cms_upload.py statement/conveyor-fa.pdf   # → conveyor/cms-upload/ (در git نیست)
```

بعد `conveyor/cms-upload/SETTINGS.md` و [`docs/cms-admin-only-fa.md`](docs/cms-admin-only-fa.md) را دنبال کنید.
