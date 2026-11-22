#!/usr/bin/env bash
# Hourly log analysis: error ratio and top URLs (4xx/5xx) from a sample access log.
set -euo pipefail
LOG=${1:?usage: loganalyze.sh <logfile>}
total=$(wc -l < "$LOG")
errors=$(awk '$7>=400' "$LOG" | wc -l)
echo "Total requests : $total"
echo "Error requests : $errors"
awk -v t="$total" -v e="$errors" 'BEGIN{printf "Error ratio    : %.1f%%\n", (e/t)*100}'
echo "Top failing URLs:"
awk '$7>=400 {gsub("\"","",$6); print $6}' "$LOG" | sort | uniq -c | sort -rn | head -3 | awk '{print "  " $2, $1}'
