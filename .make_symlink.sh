#!/usr/bin/env bash
set -euo pipefail

# Process both directories automatically

relpath() {
  python3 - <<'EOF' "$1" "$2"
import os, sys
print(os.path.relpath(sys.argv[1], sys.argv[2]))
EOF
}

symlink_duplicates() {
  local source_dir="$1"
  local target_dir="$2"

  find "$source_dir" -type f -print0 | while IFS= read -r -d '' file; do
    rel="${file#$source_dir/}"
    target_file="$target_dir/$rel"

    [[ -f "$target_file" ]] || continue

    if diff -q "$file" "$target_file" >/dev/null; then
      file_dir="$(dirname "$file")"
      target="$(relpath "$target_file" "$file_dir")"
      rm -f "$file"
      ln -s "$target" "$file"
      echo "symlinked: $file -> $target"
    fi
  done
}

# Process solution directory (top level)
if [[ -d "solution" ]]; then
  cd solution
  solutions=(runtime control_model data_model distributed onnode_parallelism)

  # Symlink between consecutive solution directories
  for i in {1..4}; do
    symlink_duplicates "${solutions[$i]}" "${solutions[$((i-1))]}"
  done
  cd ..
fi

# Process 2_hours directory
cd 2_hours
symlink_duplicates "exercise_1" "../solution/distributed"
symlink_duplicates "exercise_2" "exercise_1"
symlink_duplicates "exercise_2" "../solution/distributed"
