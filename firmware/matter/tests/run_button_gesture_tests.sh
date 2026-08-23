#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
output="${TMPDIR:-/tmp}/scoopy_button_gesture_tests"
trap 'rm -f "${output}"' EXIT

c++ \
    -std=c++17 \
    -Wall \
    -Wextra \
    -Werror \
    -I"${script_dir}/../main" \
    "${script_dir}/test_scoopy_buttons.cpp" \
    "${script_dir}/../main/scoopy_buttons.cpp" \
    -o "${output}"

"${output}"
