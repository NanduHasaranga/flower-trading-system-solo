param(
    [string]$ExecutablePath = ".\exchange_app.exe",
    [string]$OrdersPath = ".\data\orders.csv",
    [string]$OutputPath = ".\data\execution.csv",
    [string]$CasesRoot = ".\tests\cases",
    [switch]$BuildBeforeRun,
    [string]$BuildCommand = "g++ -O3 -std=c++20 .\\src\\Engine\\*.cpp .\\src\\IO\\*.cpp .\\src\\Utils\\*.cpp .\\src\\main.cpp -I .\\include\\ -o exchange_app"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-NormalizedRows {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [Parameter(Mandatory = $true)]
        [string]$ScenarioName
    )

    if (-not (Test-Path -LiteralPath $Path)) {
        throw "[$ScenarioName] Missing CSV file: $Path"
    }

    $rawLines = Get-Content -LiteralPath $Path
    $lines = @()

    foreach ($line in $rawLines) {
        if ([string]::IsNullOrWhiteSpace($line)) {
            continue
        }

        $trimmed = $line.Trim()
        if ($trimmed.Length -gt 0) {
            $lines += $trimmed
        }
    }

    if ($lines.Count -le 1) {
        return @()
    }

    $rows = @()
    $dataLines = $lines[1..($lines.Count - 1)]

    foreach ($line in $dataLines) {
        $parts = $line.Split(',', 8)
        if ($parts.Count -lt 8) {
            $missing = 8 - $parts.Count
            for ($i = 0; $i -lt $missing; $i++) {
                $parts += ""
            }
        }

        [int]$quantity = 0
        if (-not [int]::TryParse($parts[5].Trim(), [ref]$quantity)) {
            throw "[$ScenarioName] Invalid quantity value in row: '$line'"
        }

        [double]$price = 0.0
        if (-not [double]::TryParse(
                $parts[6].Trim(),
                [System.Globalization.NumberStyles]::Float,
                [System.Globalization.CultureInfo]::InvariantCulture,
                [ref]$price
            )) {
            throw "[$ScenarioName] Invalid price value in row: '$line'"
        }

        $rows += [PSCustomObject]@{
            OrderId       = $parts[0].Trim()
            ClientOrderId = $parts[1].Trim()
            Instrument    = $parts[2].Trim()
            Side          = $parts[3].Trim()
            ExecStatus    = $parts[4].Trim().ToLowerInvariant()
            Quantity      = $quantity
            Price         = [Math]::Round($price, 2)
            Reason        = $parts[7].Trim().ToLowerInvariant()
        }
    }

    return $rows
}

function Compare-Rows {
    param(
        [Parameter(Mandatory = $true)]
        [array]$Expected,

        [Parameter(Mandatory = $true)]
        [array]$Actual,

        [Parameter(Mandatory = $true)]
        [string]$ScenarioName
    )

    $differences = @()

    if ($Expected.Count -ne $Actual.Count) {
        $differences += "Row count mismatch. Expected $($Expected.Count), actual $($Actual.Count)."
    }

    $max = [Math]::Min($Expected.Count, $Actual.Count)
    for ($i = 0; $i -lt $max; $i++) {
        $expectedRow = $Expected[$i]
        $actualRow = $Actual[$i]

        if ($expectedRow.OrderId -ne $actualRow.OrderId) {
            $differences += "Row $($i + 1): OrderId expected '$($expectedRow.OrderId)' but got '$($actualRow.OrderId)'"
        }
        if ($expectedRow.ClientOrderId -ne $actualRow.ClientOrderId) {
            $differences += "Row $($i + 1): ClientOrderId expected '$($expectedRow.ClientOrderId)' but got '$($actualRow.ClientOrderId)'"
        }
        if ($expectedRow.Instrument -ne $actualRow.Instrument) {
            $differences += "Row $($i + 1): Instrument expected '$($expectedRow.Instrument)' but got '$($actualRow.Instrument)'"
        }
        if ($expectedRow.Side -ne $actualRow.Side) {
            $differences += "Row $($i + 1): Side expected '$($expectedRow.Side)' but got '$($actualRow.Side)'"
        }
        if ($expectedRow.ExecStatus -ne $actualRow.ExecStatus) {
            $differences += "Row $($i + 1): ExecStatus expected '$($expectedRow.ExecStatus)' but got '$($actualRow.ExecStatus)'"
        }
        if ($expectedRow.Quantity -ne $actualRow.Quantity) {
            $differences += "Row $($i + 1): Quantity expected '$($expectedRow.Quantity)' but got '$($actualRow.Quantity)'"
        }
        if ($expectedRow.Price -ne $actualRow.Price) {
            $differences += "Row $($i + 1): Price expected '$($expectedRow.Price)' but got '$($actualRow.Price)'"
        }
        if ($expectedRow.Reason -ne $actualRow.Reason) {
            $differences += "Row $($i + 1): Reason expected '$($expectedRow.Reason)' but got '$($actualRow.Reason)'"
        }
    }

    if ($differences.Count -eq 0) {
        Write-Host "[PASS] $ScenarioName"
        return $true
    }

    Write-Host "[FAIL] $ScenarioName"
    foreach ($difference in $differences) {
        Write-Host "  - $difference"
    }

    return $false
}

$scenarioNames = @(
    "example1",
    "example2",
    "example3",
    "example4",
    "example5",
    "example6",
    "example7"
)

if ($BuildBeforeRun -or -not (Test-Path -LiteralPath $ExecutablePath)) {
    Write-Host "Building executable..."
    Invoke-Expression $BuildCommand
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE"
    }
}

if (-not (Test-Path -LiteralPath $ExecutablePath)) {
    throw "Executable not found: $ExecutablePath"
}

$ordersBackupPath = $null
$outputBackupPath = $null

if (Test-Path -LiteralPath $OrdersPath) {
    $ordersBackupPath = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName())
    Copy-Item -LiteralPath $OrdersPath -Destination $ordersBackupPath -Force
}

if (Test-Path -LiteralPath $OutputPath) {
    $outputBackupPath = Join-Path ([System.IO.Path]::GetTempPath()) ([System.IO.Path]::GetRandomFileName())
    Copy-Item -LiteralPath $OutputPath -Destination $outputBackupPath -Force
}

$passed = 0
$failed = 0

try {
    foreach ($scenario in $scenarioNames) {
        $inputPath = Join-Path $CasesRoot "input/$scenario.csv"
        $expectedPath = Join-Path $CasesRoot "expected/$scenario.csv"

        if (-not (Test-Path -LiteralPath $inputPath)) {
            throw "Missing input fixture: $inputPath"
        }

        if (-not (Test-Path -LiteralPath $expectedPath)) {
            throw "Missing expected fixture: $expectedPath"
        }

        Copy-Item -LiteralPath $inputPath -Destination $OrdersPath -Force

        if (Test-Path -LiteralPath $OutputPath) {
            Remove-Item -LiteralPath $OutputPath -Force
        }

        & $ExecutablePath
        if ($LASTEXITCODE -ne 0) {
            throw "[$scenario] Program exited with code $LASTEXITCODE"
        }

        $expectedRows = Get-NormalizedRows -Path $expectedPath -ScenarioName $scenario
        $actualRows = Get-NormalizedRows -Path $OutputPath -ScenarioName $scenario

        if (Compare-Rows -Expected $expectedRows -Actual $actualRows -ScenarioName $scenario) {
            $passed++
        }
        else {
            $failed++
        }
    }
}
finally {
    if ($ordersBackupPath -and (Test-Path -LiteralPath $ordersBackupPath)) {
        Copy-Item -LiteralPath $ordersBackupPath -Destination $OrdersPath -Force
        Remove-Item -LiteralPath $ordersBackupPath -Force
    }

    if ($outputBackupPath -and (Test-Path -LiteralPath $outputBackupPath)) {
        Copy-Item -LiteralPath $outputBackupPath -Destination $OutputPath -Force
        Remove-Item -LiteralPath $outputBackupPath -Force
    }
}

Write-Host ""
Write-Host "Summary: $passed passed, $failed failed"

if ($failed -gt 0) {
    exit 1
}
