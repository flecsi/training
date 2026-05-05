#!/usr/bin/env bash
set -euo pipefail

solutions=(solution_1 solution_2 solution_3 solution_4 solution_5)

relpath() {
  python3 - <<'EOF' "$1" "$2"
import os, sys
print(os.path.relpath(sys.argv[1], sys.argv[2]))
EOF
}

for ((i=1; i<${#solutions[@]}; i++)); do
  prev=3_hours/"${solutions[i-1]}"
  curr=3_hours/"${solutions[i]}"

  find "$curr" -type f -print0 | while IFS= read -r -d '' file; do
    rel="${file#$curr/}"
    prev_file="$prev/$rel"

    [[ -f "$prev_file" ]] || continue

    if diff -q "$file" "$prev_file" >/dev/null; then
      file_dir="$(dirname "$file")"
      target="$(relpath "$prev_file" "$file_dir")"
      rm -f "$file"
      ln -s "$target" "$file"
      echo "symlinked: $file -> $target"
    fi
  done
done
