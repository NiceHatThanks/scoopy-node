#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
output="${TMPDIR:-/tmp}/scoopy_button_gesture_tests"
trap 'rm -f "${output}"' EXIT

# ESP-IDF prepends several cross-toolchain bin directories to PATH. Ubuntu's
# host c++ compiler then finds an Espressif assembler named `as` before
# /usr/bin/as and fails on the normal x86-64 `--64` option. Keep the IDF
# environment active, but use the normal host toolchain only for this test.
PATH=/usr/bin:/bin /usr/bin/c++ \
    -std=c++17 \
    -Wall \
    -Wextra \
    -Werror \
    -I"${script_dir}/../main" \
    "${script_dir}/test_scoopy_buttons.cpp" \
    "${script_dir}/../main/scoopy_buttons.cpp" \
    -o "${output}"

"${output}"
