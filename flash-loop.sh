#!/usr/bin/env bash
# Batch-flash IEEEPowerKit boards over DFU.
# Usage:  ./flash-loop.sh              (flashes IEEEPowerKit.bin)
#         ./flash-loop.sh other.bin
# Put each board in DFU mode (hold BOOT, plug USB, release BOOT). You can plug in
# several at once - every DFU device found gets flashed by serial number. Each board
# is flashed once; unplug it after it's reported done. Ctrl+C to stop.

set -u
cd "$(dirname "$0")"
BIN="${1:-IEEEPowerKit.bin}"
PATH="$PWD:$PATH"   # prefer a dfu-util(.exe) sitting next to this script (works in Git Bash on Windows too)

if ! command -v dfu-util >/dev/null; then
    echo "dfu-util not found. Install it first:  sudo apt install dfu-util" >&2
    exit 1
fi
if [ ! -f "$BIN" ]; then
    echo "Not found: $BIN" >&2
    exit 1
fi

# All dfu-util listing lines for the STM32 bootloader
dfu_lines() {
    dfu-util -l 2>/dev/null | grep '\[0483:df11\]'
}

# Usable serial numbers from those lines (may be empty on some bootloaders)
dfu_serials() {
    dfu_lines | sed -n 's/.*serial="\([^"]*\)".*/\1/p' | grep -vE '^(UNKNOWN)?$' | sort -u
}

flash_one() {  # $@ = extra dfu-util args (e.g. -S <serial>)
    dfu-util -d 0483:df11 "$@" -a 0 -s 0x08000000:leave -D "$BIN" 2>&1
}

# STM32 resets on ":leave" before dfu-util can poll status, so it exits non-zero
# with "Error during download get_status" even on success. Trust the download message.
report() {  # $1 = dfu-util output, $2 = label
    if grep -q "File downloaded successfully" <<<"$1"; then
        count=$((count + 1))
        echo "OK - board #$count ($2) done. Unplug it."
        return 0
    else
        echo "FAILED ($2). Unplug, re-enter DFU mode and try again."
        grep -iE "error|fail|cannot|denied|no dfu" <<<"$1" | sed 's/^/  /'
        return 1
    fi
}

declare -A done_serials
count=0
echo "Flashing $BIN. Plug boards in DFU mode; Ctrl+C to quit."
echo "Waiting for boards..."

while true; do
    lines="$(dfu_lines)"
    if [ -z "$lines" ]; then
        sleep 0.5
        continue
    fi

    present="$(dfu_serials)"

    if [ -z "$present" ]; then
        # Board(s) present but no readable serial: flash one at a time without -S,
        # then wait for it to disconnect so we don't flash the same board twice.
        echo
        echo "Board found (no serial reported), flashing..."
        out="$(flash_one)"
        report "$out" "no-serial" || true
        echo "Waiting for it to disconnect..."
        while [ -n "$(dfu_lines)" ]; do sleep 0.5; done
        continue
    fi

    # forget boards that have been unplugged so a re-plug can be flashed again
    for s in "${!done_serials[@]}"; do
        grep -qxF "$s" <<<"$present" || unset "done_serials[$s]"
    done

    flashed_any=0
    while IFS= read -r serial; do
        [ -n "$serial" ] || continue
        [ -n "${done_serials[$serial]:-}" ] && continue

        echo
        echo "Board $serial found, flashing..."
        out="$(flash_one -S "$serial")"
        done_serials[$serial]=1
        flashed_any=1
        report "$out" "$serial" || true
    done <<<"$present"

    [ "$flashed_any" -eq 0 ] && sleep 0.5
done
