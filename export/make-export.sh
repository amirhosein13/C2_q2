#!/bin/bash
# Usage: export/make-export.sh <task-name>   (e.g. conveyor)
# Generates tests, exports for CMS (TPS protocol 1), adds statements/fa.pdf, writes export/<task>-CMS.zip
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
task="$1"
cd "${root}/${task}"
tps gen
tmp="$(mktemp -d)"
tps export CMS 1 -a none -o "${task}-CMS-tmp"
mv "${task}-CMS-tmp" "${tmp}/${task}"
mkdir -p "${tmp}/${task}/statements"
cp "statement/${task}.pdf" "${tmp}/${task}/statements/fa.pdf"
rm -f "${root}/export/${task}-CMS.zip"
(cd "${tmp}" && zip -qr "${root}/export/${task}-CMS.zip" "${task}")
rm -r "${tmp}"
echo "wrote ${root}/export/${task}-CMS.zip"
