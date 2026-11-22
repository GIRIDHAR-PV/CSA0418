#!/usr/bin/env bash
# Disk-space alert: warn when any mounted filesystem crosses THRESHOLD percent.
set -euo pipefail
THRESHOLD=${1:-80}
df -P -x tmpfs -x devtmpfs | awk 'NR>1 {gsub("%","",$5); print $5, $6}' |
while read -r use mount; do
  if [ "$use" -ge "$THRESHOLD" ]; then
    echo "$(date '+%F %T') ALERT: $mount is ${use}% full (threshold ${THRESHOLD}%)"
  else
    echo "$(date '+%F %T') OK: $mount is ${use}% full"
  fi
done
