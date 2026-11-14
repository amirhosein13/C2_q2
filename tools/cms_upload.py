#!/usr/bin/env python3
"""Build <task>/cms-upload/ for adding a TPS task to CMS 1.5.1 by hand, through the admin panel only.

usage (inside the task directory, after `tps gen`):
    python3 ../tools/cms_upload.py [--pdf statement/<task>-fa.pdf] [--model solution/model.cpp]

Everything is taken from `tps export CMS 1 -a none` (protocol 1; CMS's own tps_task loader
cannot read protocol 2).  Field names, labels and file names follow CMS v1.5.1 source:
  - cms/grading/tasktypes/Batch.py   : managers "grader.cpp" (+ headers), comparator named "checker",
                                       parameters [compilation, [infile, outfile], output_eval]
  - cms/grading/steps/trusted.py     : ./checker input.txt correct_output.txt <output>,
                                       outcome = float on stdout line 1, text = stderr line 1
  - cms/grading/scoretypes/abc.py    : GroupMin with string parameters uses re.compile(p).match(codename)
  - cmscommon/importers.py + admin handler : "Add multiple testcases" turns template "*.in" into
                                       regex "(.*)\\.in$" matched against zip member names
  - cms/server/admin/templates/task.html : panel labels used in SETTINGS.md
Only the Batch task type is supported; the script stops for any other type.
"""
import argparse, json, os, re, shutil, struct, subprocess, sys, tempfile, zipfile

ZIP_LIMIT = 70 * 1024 * 1024        # admin panel (Tornado) rejects request bodies > 100 MB by default
CHECKER_FLAGS = ["-x", "c++", "-std=gnu++14", "-O2", "-static"]           # cmscontrib/loaders/tps.py v1.5.1
CMS_CPP14 = ["-DEVAL", "-std=gnu++14", "-O2", "-pipe", "-static", "-s"]   # cms/grading/languages/cpp14_gpp.py


def die(msg):
    sys.exit("cms_upload: " + msg)


def run(cmd, **kw):
    r = subprocess.run(cmd, **kw)
    if r.returncode != 0:
        die("command failed: " + " ".join(cmd))
    return r


def elf_static_x86_64(path):
    with open(path, "rb") as f:
        h = f.read(64)
        if h[:4] != b"\x7fELF" or h[4] != 2 or h[5] != 1:
            return False
        if struct.unpack_from("<H", h, 18)[0] != 0x3E:          # e_machine = EM_X86_64
            return False
        phoff, = struct.unpack_from("<Q", h, 32)
        phentsize, phnum = struct.unpack_from("<HH", h, 54)
        for i in range(phnum):
            f.seek(phoff + i * phentsize)
            if struct.unpack("<I", f.read(4))[0] == 3:          # PT_INTERP => dynamically linked
                return False
    return True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pdf", help="Persian statement PDF")
    ap.add_argument("--model", default=None, help="model solution (default: from solutions.json)")
    a = ap.parse_args()

    if not os.path.isfile("problem.json"):
        die("run inside a TPS task directory")
    if not os.path.isfile("tests/mapping"):
        die("tests/ not generated; run `tps gen` first")
    task_dir = os.getcwd()
    out = os.path.join(task_dir, "cms-upload")
    shutil.rmtree(out, ignore_errors=True)
    tmp = tempfile.mkdtemp(prefix="cms_upload_")
    exp = os.path.join(tmp, "export")
    run(["tps", "export", "CMS", "1", "-a", "none", "-o", exp])

    prob = json.load(open(os.path.join(exp, "problem.json")))
    name, ttype = prob["code"], prob["task_type"]
    if prob.get("protocol_version") != 1:
        die("export is not protocol 1")
    if ttype != "Batch":
        die("task type %s is not supported by this script (Batch only)" % ttype)
    tt = json.loads(prob["task_type_params"])                   # protocol 1: a JSON *string*
    compilation = tt.get("task_type_parameters_Batch_compilation", "alone")
    has_checker = os.path.isfile(os.path.join(exp, "checker", "checker.cpp"))

    # ---- managers
    mdir = os.path.join(out, "managers")
    os.makedirs(mdir)
    managers = []
    if compilation == "grader":
        for f in sorted(os.listdir(os.path.join(exp, "graders"))):
            if not f.endswith((".cpp", ".h")):
                die("unexpected grader file %s (only C++ is enabled)" % f)
            shutil.copy(os.path.join(exp, "graders", f), mdir)
            managers.append(f)
        if "grader.cpp" not in managers:
            die("graders/grader.cpp missing (Batch looks for grader.%l)")
        if name + ".h" not in managers:
            die("graders/%s.h missing" % name)
    if has_checker:
        chk = os.path.join(mdir, "checker")
        run(["g++"] + CHECKER_FLAGS + ["-o", chk, os.path.join(exp, "checker", "checker.cpp")])
        if not elf_static_x86_64(chk):
            die("checker is not a static x86-64 ELF")
        managers.append("checker")
    managers.sort()

    # ---- testcases (flat zips)
    tests = sorted(f[:-3] for f in os.listdir(os.path.join(exp, "tests")) if f.endswith(".in"))
    for t in tests:
        if not os.path.isfile(os.path.join(exp, "tests", t + ".out")):
            die("missing output for test " + t)
    zips, z, size = [], None, 0
    for t in tests:
        pair = [os.path.join(exp, "tests", t + e) for e in (".in", ".out")]
        add = sum(os.path.getsize(p) for p in pair)
        if z is None or size + add > ZIP_LIMIT:
            if z:
                z.close()
            zips.append(os.path.join(out, "testcases-part%d.zip" % (len(zips) + 1)))
            z, size = zipfile.ZipFile(zips[-1], "w", zipfile.ZIP_DEFLATED), 0
        for p in pair:
            z.write(p, os.path.basename(p))
        size += add
    z.close()
    in_re = re.compile(re.escape("*.in").replace("\\*", "(.*)") + "$")    # what the panel builds
    out_re = re.compile(re.escape("*.out").replace("\\*", "(.*)") + "$")
    uploaded = set()
    for zp in zips:
        if os.path.getsize(zp) >= ZIP_LIMIT:
            die("%s is too large" % zp)
        names = zipfile.ZipFile(zp).namelist()
        if any("/" in n for n in names):
            die("zip members must be flat")
        ins = {in_re.match(n).group(1) for n in names if in_re.match(n)}
        outs = {out_re.match(n).group(1) for n in names if not in_re.match(n) and out_re.match(n)}
        if ins != outs:
            die("unpaired .in/.out in " + zp)
        uploaded |= ins
    if uploaded != set(tests):
        die("zips do not contain exactly the exported tests")

    # ---- GroupMin parameters, in subtask index order, checked against tests/mapping
    mapping = {}
    for line in open("tests/mapping"):
        st, t = line.split()
        mapping.setdefault(st, set()).add(t.lower().replace(" ", "_"))
    sub_json = json.load(open("subtasks.json"))["subtasks"]
    params, sub_rows = [], []
    for f in sorted(os.listdir(os.path.join(exp, "subtasks"))):
        st = json.load(open(os.path.join(exp, "subtasks", f)))
        sname = f[3:-5]                                          # "NN-name.json"
        rx = "^(" + "|".join(re.escape(t) for t in sorted(st["testcases"])) + ")$"
        got = {c for c in uploaded if re.compile(rx).match(c)}
        if got != mapping[sname] or st["score"] != sub_json[sname]["score"]:
            die("regex/score check failed for subtask " + sname)
        params.append([st["score"], rx])
        sub_rows.append((sub_json[sname]["index"], sname, st["score"], len(got)))
    if sum(p[0] for p in params) != 100:
        die("scores do not sum to 100")
    json.dump(params, open(os.path.join(out, "score_type_parameters.json"), "w"))

    # ---- attachment (tps make-public output, via the exporter)
    att_src = os.path.join(exp, "attachments", name + ".zip")
    if not os.path.isfile(att_src):
        die("no public attachment produced")
    os.makedirs(os.path.join(out, "attachments"))
    shutil.copy(att_src, os.path.join(out, "attachments"))
    with zipfile.ZipFile(att_src) as az:
        att_names = az.namelist()
        for n in att_names:
            data = az.read(n)
            if b"SECRET" in data or b"secret" in data:
                die("attachment member %s mentions a secret" % n)
            if n.startswith("tests/") or (n.endswith((".in", ".out")) and not n.startswith("examples/")):
                die("attachment contains test data: " + n)

    # ---- statement
    if a.pdf:
        shutil.copy(a.pdf, os.path.join(out, "statement-fa.pdf"))

    # ---- compile grader + model like CMS's "C++14 / g++" and run it on the public examples
    model = a.model
    if model is None:
        sols = json.load(open("solutions.json"))
        model = os.path.join("solution", [s for s, v in sols.items() if v["verdict"] == "model_solution"][0])
    cdir = os.path.join(tmp, "compile")
    os.makedirs(cdir)
    for m in managers:
        shutil.copy(os.path.join(mdir, m), cdir)
    shutil.copy(model, os.path.join(cdir, name + ".cpp"))
    srcs = (["grader.cpp"] if compilation == "grader" else []) + [name + ".cpp"]
    run(["g++"] + CMS_CPP14 + ["-o", name] + srcs, cwd=cdir)
    ex_ok = 0
    for f in sorted(os.listdir("public/examples")):
        if not f.endswith(".in"):
            continue
        inp = os.path.join(task_dir, "public/examples", f)
        ans = inp[:-3] + ".out"
        res = os.path.join(cdir, "out.txt")
        with open(inp) as fi, open(res, "w") as fo:
            run([os.path.join(cdir, name)], stdin=fi, stdout=fo)
        r = subprocess.run([os.path.join(mdir, "checker"), inp, ans, res], capture_output=True, text=True)
        if r.returncode != 0 or float(r.stdout.splitlines()[0]) != 1.0:
            die("model solution (compiled as in CMS) fails example %s: %s" % (f, r.stderr.strip()))
        ex_ok += 1

    mem_mib = prob["memory_limit"] // (1024 * 1024)
    expected = {"name": name, "title": prob["name"], "task_type": ttype,
                "task_type_parameters": [compilation, ["", ""], "comparator" if has_checker else "diff"],
                "time_limit": prob["time_limit"], "memory_limit_bytes": prob["memory_limit"],
                "managers": managers, "testcases": len(tests), "score_type": "GroupMin",
                "score_type_parameters": params, "submission_format": [name + ".%l"],
                "attachments": [name + ".zip"]}
    json.dump(expected, open(os.path.join(tmp, "expected.json"), "w"))
    shutil.copy(os.path.join(tmp, "expected.json"), os.path.join(out, ".expected.json"))

    comp_label = {"grader": "Submissions are compiled with a grader",
                  "alone": "Submissions are self-sufficient"}[compilation]
    eval_label = "Outputs are compared by a comparator" if has_checker else "Outputs compared with white diff"
    rows = "\n".join("| %d | %s | %d | %d |" % r for r in sorted(sub_rows))
    zips_txt = "\n".join("   - `%s` (%.2f MB)" % (os.path.basename(p), os.path.getsize(p) / 2 ** 20) for p in zips)
    with open(os.path.join(out, "SETTINGS.md"), "w") as s:
        s.write(f"""# تنظیمات پنل ادمین CMS 1.5.1 برای `{name}`

برچسب‌ها همان متن فرم‌های پنل در CMS 1.5.1 است. ترتیب قدم‌ها مهم است: **تست‌ها قبل از Score Parameters**،
چون CMS پارامترهای GroupMin را هنگام Update با تست‌های موجود چک می‌کند و اگر تستی پیدا نکند خطای
«No testcase matches against the regexp» می‌دهد.

## ۱. ساخت سوال
Tasks → ساخت سوال → name = `{name}`.
(CMS خودش یک dataset به نام `Default` با Batch/alone/diff و Score type = Sum می‌سازد؛ پایین درستش می‌کنیم.)

## ۲. Task configuration (بالای صفحه)
| فیلد پنل | مقدار |
|---|---|
| Name | `{name}` |
| Title | `{prob['name']}` |
| Statements → [add] | Language code: `fa`، فایل `statement-fa.pdf`، Upload؛ بعد تیک **Primary?** کنار `fa` |
| Attachments → [add] | `attachments/{name}.zip` |
| Submission format | `{name}.%l` |
| Feedback level | `Full` (یا `Restricted` اگر جزئیات تست‌ها نباید دیده شود) |
| Token mode | `Disabled` |
| Maximum number of submissions | `50` (پیشنهادی؛ خالی = نامحدود) |
| Maximum number of user tests | خالی |
| Minimum interval between submissions | `60` (ثانیه، پیشنهادی) |
| Minimum interval between user tests | خالی |
| Score decimal places | `{prob.get('score_precision', 2)}` |
| Score mode | **Use the sum over each subtask of the best result for that subtask across all submissions (IOI 2017-)** |

## ۳. Dataset «Default» (پایین صفحه)
| فیلد پنل | مقدار |
|---|---|
| Time limit | `{prob['time_limit']}` second(s) |
| Memory limit | `{mem_mib}` MiB |
| Task type | `{ttype}` |
| Compilation | `{comp_label}` |
| I/O (blank for stdin/stdout) → Input file / Output file | هر دو **خالی** |
| Output evaluation | `{eval_label}` |
| Managers → [add] (یکی‌یکی) | {', '.join('`%s`' % m for m in managers)} (اسم فایل‌ها نباید عوض شود) |
| Score Type | `GroupMin` (در این مرحله هنوز Score Parameters را عوض نکنید) |

دکمه‌ی **Update** پایین صفحه.

## ۴. تست‌ها
Dataset → **Add multiple testcases**، برای هر فایل:
{zips_txt}
- Input file names template: `*.in` (پیش‌فرض پنل `input.*` است، عوض کنید)
- Output file names template: `*.out` (پیش‌فرض `output.*`)
- Public: بدون تیک؛ Overwrite: بدون تیک
- انتظار: **{len(tests)}** تست در لیست.

## ۵. Score Parameters
حالا Score Type = `GroupMin` و در **Score Parameters** کل محتوای `score_type_parameters.json` را paste کنید → **Update**.

| index | زیرمسئله | امتیاز | تعداد تست |
|---|---|---|---|
{rows}

جمع امتیاز: {sum(p[0] for p in params)}

## ۶. افزودن به آزمون
صفحه‌ی آزمون → Tasks → افزودن `{name}`. زبان‌های مجاز آزمون فقط C++ (grader فقط C++ دارد).

---
بررسی‌های خودکار این بسته: checker = ELF static x86-64 (g++ {' '.join(CHECKER_FLAGS)})؛
grader + `{model}` با دستور C++14 خود CMS کامپایل و روی {ex_ok} نمونه‌ی public با checker قبول شد؛
هر regex دقیقاً تست‌های همان زیرمسئله را می‌گیرد؛ هر zip کمتر از ۷۰MB و تخت است؛ پیوست بدون داده‌ی مخفی.
""")
    shutil.rmtree(tmp)
    print("written", out)


if __name__ == "__main__":
    main()
