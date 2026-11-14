#!/usr/bin/env python3
"""Run CMS's own TPS loader (cmscontrib/loaders/tps.py of a CMS checkout, e.g. v1.5.1) on a fresh
`tps export CMS 1` of the current task, with the cms.* modules replaced by recording stubs
(no database, no file storage).  Then compare what the loader would create with the values that
tools/cms_upload.py wrote to cms-upload/.expected.json (= what SETTINGS.md tells you to type).

usage (inside the task directory, after `tps gen` and `cms_upload.py`):
    python3 ../tools/cms_loader_check.py <cms-checkout> [statement.pdf]

This checks the loader's interpretation of the package; it is not a full CMS import.
"""
import importlib.util, json, os, re, shutil, subprocess, sys, tempfile, types

cms_src = os.path.abspath(sys.argv[1])
pdf = sys.argv[2] if len(sys.argv) > 2 else None
expected = json.load(open("cms-upload/.expected.json"))


class Obj:
    def __init__(self, *args, **kw):
        self.args, self.kw = args, kw


def module(name, **attrs):
    m = types.ModuleType(name)
    m.__dict__.update(attrs)
    sys.modules[name] = m
    return m


src = open(os.path.join(cms_src, "cmscontrib/loaders/tps.py")).read()
consts = {n: n.lower() for n in re.findall(r"\b((?:SCORE_MODE|FEEDBACK_LEVEL)_[A-Z_]+)\b", src)}
consts.update(SCORE_MODE_MAX_SUBTASK="max_subtask", FEEDBACK_LEVEL_FULL="full",
              FEEDBACK_LEVEL_RESTRICTED="restricted")
module("cms", **consts)
module("cms.db", **{n: type(n, (Obj,), {}) for n in
                    ("Task", "Dataset", "Manager", "Testcase", "Attachment", "Statement")})
module("cmscommon")
module("cmscommon.constants", **consts)
module("cmscontrib")
module("cmscontrib.loaders")


class TaskLoader:
    def __init__(self, path, file_cacher):
        self.path, self.file_cacher = path, file_cacher


module("cmscontrib.loaders.base_loader", TaskLoader=TaskLoader)
spec = importlib.util.spec_from_file_location(
    "cmscontrib.loaders.tps", os.path.join(cms_src, "cmscontrib/loaders/tps.py"))
tps = importlib.util.module_from_spec(spec)
spec.loader.exec_module(tps)


class FileCacher:
    def put_file_from_path(self, path, description):
        return "file:" + path

    def put_file_content(self, content, description):
        return "content"


tmp = tempfile.mkdtemp(prefix="cms_loader_check_")
exp = os.path.join(tmp, expected["name"])
subprocess.run(["tps", "export", "CMS", "1", "-a", "none", "-o", exp], check=True, capture_output=True)
if pdf:
    os.makedirs(os.path.join(exp, "statements"))
    shutil.copy(pdf, os.path.join(exp, "statements", "fa.pdf"))

assert tps.TpsTaskLoader.detect(exp), "loader.detect() rejected the export"
task = tps.TpsTaskLoader(exp, FileCacher()).get_task(get_statement=True)
assert task is not None, "loader returned None"
t, d = task.kw, task.active_dataset.kw

codenames = sorted(d["testcases"])
def chosen(rx):
    return [c for c in codenames if re.compile(rx).match(c)]

loader_sets = [chosen(p[1]) for p in d["score_type_parameters"]]
ours_sets = [chosen(p[1]) for p in expected["score_type_parameters"]]
checks = [
    ("name", t["name"], expected["name"]),
    ("title", t["title"], expected["title"]),
    ("submission_format", t["submission_format"], expected["submission_format"]),
    ("attachments", sorted(t["attachments"]), expected["attachments"]),
    ("statements", sorted(t.get("statements", {})), ["fa"] if pdf else []),
    ("task_type", d["task_type"], expected["task_type"]),
    ("task_type_parameters", d["task_type_parameters"], expected["task_type_parameters"]),
    ("time_limit", d["time_limit"], expected["time_limit"]),
    ("memory_limit (bytes)", d["memory_limit"], expected["memory_limit_bytes"]),
    ("managers", sorted(d["managers"]), expected["managers"]),
    ("testcases", len(codenames), expected["testcases"]),
    ("score_type", d["score_type"], expected["score_type"]),
    ("subtask scores", [p[0] for p in d["score_type_parameters"]],
     [p[0] for p in expected["score_type_parameters"]]),
    ("subtask test sets", loader_sets, ours_sets),
]
bad = 0
for what, got, want in checks:
    ok = got == want
    bad += not ok
    shown = got if what != "subtask test sets" else [len(s) for s in got]
    print("%-4s %-22s loader=%s" % ("OK" if ok else "DIFF", what, shown) + ("" if ok else "  expected=%s" % want))
print("loader-only settings (not in the package):",
      "score_mode=%s" % t.get("score_mode"),
      "max_submission_number=%s" % t.get("max_submission_number"),
      "min_submission_interval=%s" % t.get("min_submission_interval"))
shutil.rmtree(tmp)
sys.exit(1 if bad else 0)
