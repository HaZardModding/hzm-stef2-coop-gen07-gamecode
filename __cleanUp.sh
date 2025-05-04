#!/bin/bash

BLACKLIST="__cleanUp_blacklist.txt"
LOGFILE="__cleanup_delete.log"
DRYRUN=true

files_deleted=0
dirs_deleted=0

declare -A grouped_log

if [[ ! -f "$BLACKLIST" ]]; then
  echo "Error: $BLACKLIST not found."
  exit 1
fi

> "$LOGFILE"

while IFS= read -r line || [[ -n "$line" ]]; do
  trimmed="$(echo "$line" | xargs)"

  if [[ -z "$trimmed" || "$trimmed" =~ ^# ]]; then
    continue
  fi

  pattern="${trimmed%/}"
  while IFS= read -r match; do
    [[ -z "$match" ]] && continue
    folder="$(dirname "$match")"

    if [[ ! -e "$match" ]]; then
      continue
    fi

    [[ -z "${grouped_log[$folder]}" ]] && grouped_log[$folder]=""

    if [[ -d "$match" ]]; then
      if [[ "$DRYRUN" == true ]]; then
        echo "[DRY-RUN] Would delete directory: $match"
        grouped_log[$folder]+=$'[DRY-RUN] Would delete directory: '"$match"$'\n'
      else
        rm -rf "$match"
        grouped_log[$folder]+=$'Deleted directory: '"$match"$'\n'
        ((dirs_deleted++))
      fi
    elif [[ -f "$match" ]]; then
      if [[ "$DRYRUN" == true ]]; then
        echo "[DRY-RUN] Would delete file: $match"
        grouped_log[$folder]+=$'[DRY-RUN] Would delete file: '"$match"$'\n'
      else
        rm -f "$match"
        grouped_log[$folder]+=$'Deleted file: '"$match"$'\n'
        ((files_deleted++))
      fi
    fi
  done < <(find . -type f -name "$(basename "$pattern")" -o -type d -name "$(basename "$pattern")")

done < "$BLACKLIST"

for dir in "${!grouped_log[@]}"; do
  echo "[$dir]" >> "$LOGFILE"
  echo -e "${grouped_log[$dir]}" >> "$LOGFILE"
  echo "" >> "$LOGFILE"
done

if [[ "$DRYRUN" != true ]]; then
  echo "Deleted files: $files_deleted" >> "$LOGFILE"
  echo "Deleted directories: $dirs_deleted" >> "$LOGFILE"
fi

echo "Cleanup complete. See $LOGFILE."
