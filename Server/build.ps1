param(
    [ValidateSet('All','GW1','GW2','BFN')]
    [string]$Game = 'All',
    [ValidateSet('Debug','Release')]
    [string]$Configuration = 'Release'
)

$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$outDir = Join-Path $root "build"

$msbuild = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe 2>$null | Select-Object -First 1
if (-not $msbuild) { $msbuild = "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe" }

Write-Host "`n  Cypress Server Build" -ForegroundColor Yellow
Write-Host "  ====================`n" -ForegroundColor Yellow

$games = if ($Game -eq 'All') { @('GW1','GW2','BFN') } else { @($Game) }
$vcxproj = "$root\GW2ServerProject.vcxproj"
$total = $games.Count
$i = 0

if (-not (Test-Path $outDir)) { New-Item -ItemType Directory -Path $outDir | Out-Null }

foreach ($g in $games) {
    $i++
    Write-Host "  [$i/$total] $g" -NoNewline
    & $msbuild $vcxproj "/p:Configuration=$Configuration - $g" /p:Platform=x64 /m /nologo /v:minimal 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host " FAILED" -ForegroundColor Red
        & $msbuild $vcxproj "/p:Configuration=$Configuration - $g" /p:Platform=x64 /m /nologo /v:minimal
        exit 1
    }
    $dll = "cypress_$g.dll"
    Copy-Item "$root\x64\$Configuration - $g\$dll" "$outDir\$dll" -Force
    $size = "{0:N0} KB" -f ((Get-Item "$outDir\$dll").Length / 1KB)
    Write-Host " -> $dll ($size)" -ForegroundColor Green
}

Write-Host "`n  Build complete -> $outDir" -ForegroundColor Green
Write-Host ""
Get-ChildItem $outDir -File | Sort-Object Name | ForEach-Object {
    $size = "{0,8:N0} KB" -f ($_.Length / 1KB)
    Write-Host "  $size  $($_.Name)"
}
Write-Host ""
