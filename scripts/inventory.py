from pathlib import Path
from collections import Counter
import hashlib
import json

root = Path(__file__).resolve().parents[1]
extracted = root / "extracted"
reports = root / "reports"
reports.mkdir(exist_ok=True)

files = []
extensions = Counter()

for path in sorted(extracted.rglob("*")):
    if not path.is_file():
        continue

    data = path.read_bytes()
    relative = path.relative_to(extracted).as_posix()
    ext = path.suffix.lower() or "(no extension)"

    extensions[ext] += 1
    files.append({
        "path": relative,
        "size": len(data),
        "sha256": hashlib.sha256(data).hexdigest(),
        "first_16_bytes": data[:16].hex(" ")
    })

report = {
    "game": "Harvest Moon DS: Sunshine Islands",
    "region": "USA (to verify)",
    "file_count": len(files),
    "total_bytes": sum(f["size"] for f in files),
    "extensions": dict(extensions.most_common()),
    "files": files
}

output = reports / "inventory.json"
output.write_text(
    json.dumps(report, indent=2, ensure_ascii=False),
    encoding="utf-8"
)

print(f"Files found: {len(files)}")
print(f"Report saved: {output}")

for ext, count in extensions.most_common(15):
    print(f"{ext}: {count}")
