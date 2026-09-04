# Batch-flash IEEEPowerKit boards over DFU.
# Usage:  .\flash-loop.ps1            (flashes IEEEPowerKit.bin)
#         .\flash-loop.ps1 IEEEPowerKit2.bin
# Put each board in DFU mode (hold BOOT, plug USB, release BOOT). You can plug in
# several at once - every DFU device found gets flashed by serial number. Each board
# is flashed once; unplug it after it's reported done. Ctrl+C to stop.

param([string]$Bin = "IEEEPowerKit.bin")

$Bin = Join-Path $PSScriptRoot $Bin
if (-not (Test-Path $Bin)) { Write-Error "Not found: $Bin"; exit 1 }

# Use a dfu-util.exe sitting next to this script if there is one (portable/shared
# setup - no PlatformIO needed); otherwise fall back to PlatformIO's copy.
$LocalDfu = @("dfu-util.exe", "dfu-util-static.exe") |
    ForEach-Object { Join-Path $PSScriptRoot $_ } | Where-Object { Test-Path $_ } | Select-Object -First 1

function Invoke-DfuUtil {
    if ($LocalDfu) { & $LocalDfu @args 2>&1 | Out-String }
    else { pio pkg exec --package tool-dfuutil -- dfu-util @args 2>&1 | Out-String }
}

if (-not $LocalDfu -and -not (Get-Command pio -ErrorAction SilentlyContinue)) {
    Write-Error "Neither dfu-util.exe next to this script nor PlatformIO (pio) found. Put dfu-util.exe in this folder or install PlatformIO."
    exit 1
}

# Returns serial numbers of all STM32 DFU devices currently attached
function DfuSerials {
    $out = Invoke-DfuUtil -l
    $serials = @()
    foreach ($m in [regex]::Matches($out, '\[0483:df11\].*?alt=0.*?serial="([^"]+)"')) {
        $serials += $m.Groups[1].Value
    }
    return $serials | Select-Object -Unique
}

$done = @{}   # serial -> $true once flashed
$count = 0
Write-Host "Flashing $Bin. Plug boards in DFU mode; Ctrl+C to quit." -ForegroundColor Cyan
Write-Host "Waiting for boards..." -ForegroundColor Yellow

while ($true) {
    $present = DfuSerials
    # forget boards that have been unplugged so a re-plug can be flashed again
    foreach ($s in @($done.Keys)) { if ($present -notcontains $s) { $done.Remove($s) } }

    $todo = $present | Where-Object { -not $done.ContainsKey($_) }
    if (-not $todo) { Start-Sleep -Milliseconds 500; continue }

    foreach ($serial in $todo) {
        Write-Host "`nBoard $serial found, flashing..." -ForegroundColor Cyan
        $out = Invoke-DfuUtil -d 0483:df11 -S $serial -a 0 -s 0x08000000:leave -D "$Bin"
        $done[$serial] = $true
        # STM32 resets on ":leave" before dfu-util can poll status, so it exits 74 with
        # "Error during download get_status" even on success. Trust the download message.
        if ($out -match "File downloaded successfully") {
            $count++
            Write-Host "OK - board #$count ($serial) done. Unplug it." -ForegroundColor Green
        } else {
            Write-Host "FAILED ($serial). Unplug, re-enter DFU mode and try again." -ForegroundColor Red
            $out -split "`n" | Select-String -Pattern "error|Error|fail" | ForEach-Object { Write-Host "  $_" }
        }
    }
}
