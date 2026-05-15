#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$SCRIPT_DIR/../.."
BACKEND_BIN="$ROOT/hulk_backend"
EXPECTED_DIR="$ROOT/tests/expected/eval"
BACKEND_EXPECTED_DIR="$ROOT/tests/expected/backend"

TOTAL=0
PASSED=0
FAILED=0

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
RESET='\033[0m'
BOLD='\033[1m'

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

run_one() {
    local hulk_file="$1"
    local name
    name="$(basename "$hulk_file" .hulk)"
    local exe="$TMP_DIR/$name"
    local actual_file="$TMP_DIR/$name.actual"
    local expected_file="$EXPECTED_DIR/$name.expected"

    TOTAL=$((TOTAL + 1))

    if ! "$BACKEND_BIN" "$hulk_file" -o "$exe" > "$TMP_DIR/$name.compile.out" 2>&1; then
        echo -e "  ${RED}FAIL${RESET} $name"
        echo -e "       ${YELLOW}backend:${RESET}"
        sed 's/^/         /' "$TMP_DIR/$name.compile.out"
        FAILED=$((FAILED + 1))
        return
    fi

    "$exe" > "$actual_file" 2>&1 || true

    if diff -u "$expected_file" "$actual_file" > "$TMP_DIR/$name.diff"; then
        echo -e "  ${GREEN}OK${RESET}  $name"
        PASSED=$((PASSED + 1))
    else
        echo -e "  ${RED}FAIL${RESET} $name"
        sed 's/^/       /' "$TMP_DIR/$name.diff"
        FAILED=$((FAILED + 1))
    fi
}

run_expected_one() {
    local hulk_file="$1"
    local expected_file="$2"
    local name
    name="$(basename "$hulk_file" .hulk)"
    local exe="$TMP_DIR/$name"
    local actual_file="$TMP_DIR/$name.actual"

    TOTAL=$((TOTAL + 1))

    if ! "$BACKEND_BIN" "$hulk_file" -o "$exe" > "$TMP_DIR/$name.compile.out" 2>&1; then
        echo -e "  ${RED}FAIL${RESET} $name"
        echo -e "       ${YELLOW}backend:${RESET}"
        sed 's/^/         /' "$TMP_DIR/$name.compile.out"
        FAILED=$((FAILED + 1))
        return
    fi

    "$exe" > "$actual_file" 2>&1 || true

    if diff -u "$expected_file" "$actual_file" > "$TMP_DIR/$name.diff"; then
        echo -e "  ${GREEN}OK${RESET}  $name"
        PASSED=$((PASSED + 1))
    else
        echo -e "  ${RED}FAIL${RESET} $name"
        sed 's/^/       /' "$TMP_DIR/$name.diff"
        FAILED=$((FAILED + 1))
    fi
}

run_invalid_one() {
    local hulk_file="$1"
    local name
    name="$(basename "$hulk_file" .hulk)"
    local exe="$TMP_DIR/$name"

    TOTAL=$((TOTAL + 1))

    if "$BACKEND_BIN" "$hulk_file" -o "$exe" > "$TMP_DIR/$name.invalid.out" 2>&1; then
        echo -e "  ${RED}FAIL${RESET} $name"
        echo "       backend accepted an invalid extension program"
        FAILED=$((FAILED + 1))
    else
        echo -e "  ${GREEN}OK${RESET}  $name"
        PASSED=$((PASSED + 1))
    fi
}

suite_header() {
    echo ""
    echo -e "${BOLD}$1${RESET}"
}

suite_header "BACKEND C4"
for f in "$ROOT"/tests/eval/c4_*.hulk; do
    [[ -f "$f" ]] || continue
    run_one "$f"
done

suite_header "BACKEND C5"
for f in "$ROOT"/tests/eval/c5_*.hulk; do
    [[ -f "$f" ]] || continue
    run_one "$f"
done

suite_header "BACKEND C6"
for f in "$ROOT"/tests/eval/c6_*.hulk; do
    [[ -f "$f" ]] || continue
    run_one "$f"
done

suite_header "BACKEND TYPE-HOLES VALIDOS"
for f in "$ROOT"/tests/extension/valid_*.hulk; do
    [[ -f "$f" ]] || continue
    run_expected_one "$f" "$BACKEND_EXPECTED_DIR/$(basename "$f" .hulk).expected"
done

suite_header "BACKEND TYPE-HOLES INVALIDOS"
for name in \
    invalid_ambiguous_id \
    invalid_conflict \
    invalid_double_constraint \
    invalid_op_conflict \
    invalid_recursive_type \
    invalid_type_mismatch \
    invalid_unknown_method
do
    run_invalid_one "$ROOT/tests/extension/$name.hulk"
done

echo ""
echo -e "${BOLD}BACKEND RESUMEN${RESET}"
echo "  Total  : $TOTAL"
echo -e "  ${GREEN}Passed${RESET} : $PASSED"
echo "  Failed : $FAILED"

[[ $FAILED -eq 0 ]]
