#!/bin/bash
# Builds conveyor-fa.pdf from index.md (needs pandoc and node + playwright with chromium).
set -euo pipefail
cd "$(dirname "$0")"
pandoc index.md -s --katex=pdf-assets/katex/ -f markdown -t html5 --metadata title=conveyor --metadata lang=fa --metadata dir=rtl \
	-c pdf-assets/style.css -o index.html
node -e '
const { chromium } = require("playwright");
(async () => {
	const b = await chromium.launch();
	const p = await b.newPage();
	await p.goto("file://" + process.cwd() + "/index.html", { waitUntil: "networkidle" });
	await p.waitForFunction(() => document.querySelectorAll(".math").length === document.querySelectorAll(".math .katex").length);
	await p.evaluate(() => document.fonts.ready);
	await p.pdf({ path: "conveyor-fa.pdf", format: "A4", printBackground: true, preferCSSPageSize: true });
	await b.close();
})();'
rm -f index.html
echo "built $(pwd)/conveyor-fa.pdf"
