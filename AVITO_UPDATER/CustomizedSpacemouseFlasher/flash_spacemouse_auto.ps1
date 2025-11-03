<#
SpaceMouse/ATmega32U4 flashing helper
- Triggers 1200-baud reset on the app port (default COM5)
- Detects the new bootloader COM
- Immediately flashes via avrdude (avr109) at 57600, then 115200 if needed
- Retries the whole cycle several times to survive Windows driver delays
Place this script in the same folder as avrdude.exe, avrdude.conf, and your HEX.

Usage examples:
  powershell -NoProfile -ExecutionPolicy Bypass -File .\flash_spacemouse_auto.ps1
  powershell -NoProfile -ExecutionPolicy Bypass -File .\flash_spacemouse_auto.ps1 -AppPort COM7 -Hex .\fw.hex
#>

param(
  [string]$AppPort = "COM5",
  [string]$Hex = ".\Customized_Spacemouse_v2.1.hex",
  [string]$Avrdude = ".\avrdude.exe",
  [string]$Conf = ".\avrdude.conf",
  [int]$Attempts = 10,
  [int]$BootScanSec = 10,
  [int]$BetweenTriesSec = 2,
  [switch]$VerboseLog
)

$ErrorActionPreference = 'Stop'

function Kill-PortClaimers {
  $names = @('3DxService','3DxSmartUi','3DxWare','3Dconnexion')
  Get-Process 2>$null | Where-Object { $names -contains $_.ProcessName } | ForEach-Object {
    try { Stop-Process -Id $_.Id -Force } catch {}
  }
}

function Reset-IntoBoot([string]$port) {
  try {
    $sp = New-Object IO.Ports.SerialPort $port,1200,'None',8,'One'
    $sp.ReadTimeout = 200; $sp.WriteTimeout = 200
    $sp.Open()
    Start-Sleep -Milliseconds 80
    $sp.Close()
    $sp.Dispose()
    return $true
  } catch {
    return $false
  }
}

function Get-Ports {
  # .NET is fast and returns just COM names
  return [System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object -Unique
}

function Wait-NewPort([string[]]$before, [int]$timeoutSec) {
  $deadline = (Get-Date).AddSeconds($timeoutSec)
  $picked = $null
  do {
    Start-Sleep -Milliseconds 120
    $now = Get-Ports
    $added = @($now | Where-Object { $before -notcontains $_ })
    if ($added.Count -gt 0) {
      # pick the highest-numbered COM (usually the newest)
      $picked = $added | Sort-Object { [int]($_ -replace '\D','') } | Select-Object -Last 1
      break
    }
  } while ((Get-Date) -lt $deadline)
  return $picked
}

function Probe-Port([string]$port, [int]$msTimeout = 3000) {
  $deadline = (Get-Date).AddMilliseconds($msTimeout)
  do {
    try {
      $t = New-Object IO.Ports.SerialPort $port,1200,'None',8,'One'
      $t.Open(); $t.Close(); $t.Dispose()
      return $true
    } catch {
      Start-Sleep -Milliseconds 120
    }
  } while ((Get-Date) -lt $deadline)
  return $false
}

# Validate files
$Avrdude = (Resolve-Path $Avrdude).Path
$Conf    = (Resolve-Path $Conf).Path
$Hex     = (Resolve-Path $Hex).Path

Write-Host "=== 32U4 Flash Auto (reset -> detect -> flash) ==="
Write-Host ("Working dir : {0}" -f (Get-Location).Path)
Write-Host ("AppPort     : {0}" -f $AppPort)
Write-Host ("HEX         : {0}" -f $Hex)
Write-Host ("avrdude     : {0}" -f $Avrdude)
Write-Host ("conf        : {0}" -f $Conf)

Kill-PortClaimers

$uArg = 'flash:w:"{0}":i' -f $Hex
$ok = $false

for ($try = 1; $try -le $Attempts; $try++) {
  Write-Host ("--- TRY {0}/{1} ---" -f $try, $Attempts)

  $before = Get-Ports
  if ($VerboseLog) { Write-Host ("Ports before: {0}" -f ($before -join ', ')) }

  # Trigger bootloader
  if (-not (Reset-IntoBoot -port $AppPort)) {
    Write-Warning "1200-baud reset failed (continuing anyway)"
  }

  # Wait for new COM
  $boot = Wait-NewPort -before $before -timeoutSec $BootScanSec
  if (-not $boot) {
    Write-Warning "No new COM detected; retrying..."
    Start-Sleep -Seconds $BetweenTriesSec
    continue
  }

  Write-Host ("Bootloader port: {0}" -f $boot)

  # Make sure it's openable (driver finished binding)
  if (-not (Probe-Port -port $boot -msTimeout 2500)) {
    Write-Warning ("Port {0} not openable yet; trying anyway..." -f $boot)
  }

  # Flash @57600, then @115200
  $args = @('-C',$Conf,'-v','-p','atmega32u4','-c','avr109','-P',"\\.\$boot",'-b','57600','-D','-U',$uArg)
  & $Avrdude @args
  if ($LASTEXITCODE -eq 0) { $ok = $true; break }

  Write-Warning ("avrdude @57600 failed (rc={0}), trying @115200" -f $LASTEXITCODE)
  $args = @('-C',$Conf,'-v','-p','atmega32u4','-c','avr109','-P',"\\.\$boot",'-b','115200','-D','-U',$uArg)
  & $Avrdude @args
  if ($LASTEXITCODE -eq 0) { $ok = $true; break }

  Write-Warning ("Failed on {0}; will retry after {1}s..." -f $boot, $BetweenTriesSec)
  Start-Sleep -Seconds $BetweenTriesSec
}

if ($ok) {
  Write-Host "Flashing OK."
  exit 0
} else {
  Write-Error "Flashing failed after $Attempts attempts."
  exit 1
}
