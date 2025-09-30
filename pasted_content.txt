# Veyon Chat Plugin - PowerShell Installation Script
# This script automates the installation of the Veyon Chat Plugin on Windows

param(
    [switch]$Force,
    [string]$VeyonPath = ""
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Veyon Chat Plugin Installation Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if running as administrator
if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Host "ERROR: This script must be run as Administrator." -ForegroundColor Red
    Write-Host "Please right-click PowerShell and select 'Run as administrator'" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "Checking for Veyon installation..." -ForegroundColor Yellow

# Find Veyon installation
$veyonLocations = @(
    "C:\Program Files\Veyon",
    "C:\Program Files (x86)\Veyon"
)

$veyonFound = $false
$veyonInstallPath = ""

if ($VeyonPath -ne "") {
    if (Test-Path "$VeyonPath\veyon-master.exe") {
        $veyonInstallPath = $VeyonPath
        $veyonFound = $true
        Write-Host "Using specified Veyon path: $VeyonPath" -ForegroundColor Green
    } else {
        Write-Host "ERROR: Veyon not found at specified path: $VeyonPath" -ForegroundColor Red
        exit 1
    }
} else {
    foreach ($location in $veyonLocations) {
        if (Test-Path "$location\veyon-master.exe") {
            $veyonInstallPath = $location
            $veyonFound = $true
            Write-Host "Found Veyon at: $location" -ForegroundColor Green
            break
        }
    }
}

if (-not $veyonFound) {
    Write-Host "ERROR: Veyon installation not found." -ForegroundColor Red
    Write-Host "Please ensure Veyon 4.9.7 is installed before running this script." -ForegroundColor Red
    Write-Host "You can also specify a custom path using: -VeyonPath 'C:\Your\Veyon\Path'" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

# Create plugins directory
$pluginDir = Join-Path $veyonInstallPath "plugins"
if (-not (Test-Path $pluginDir)) {
    Write-Host "Creating plugins directory..." -ForegroundColor Yellow
    try {
        New-Item -ItemType Directory -Path $pluginDir -Force | Out-Null
        Write-Host "Plugins directory created successfully." -ForegroundColor Green
    } catch {
        Write-Host "ERROR: Failed to create plugins directory: $_" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
}

# Check for plugin DLL
$pluginDll = "veyon-chat-plugin.dll"
if (-not (Test-Path $pluginDll)) {
    Write-Host "ERROR: $pluginDll not found in current directory." -ForegroundColor Red
    Write-Host "Please ensure the plugin DLL is in the same folder as this script." -ForegroundColor Red
    Write-Host "Current directory: $(Get-Location)" -ForegroundColor Yellow
    Read-Host "Press Enter to exit"
    exit 1
}

# Check if plugin already exists
$targetPath = Join-Path $pluginDir $pluginDll
if ((Test-Path $targetPath) -and (-not $Force)) {
    Write-Host "WARNING: Plugin already exists at target location." -ForegroundColor Yellow
    $response = Read-Host "Do you want to overwrite it? (y/N)"
    if ($response -ne "y" -and $response -ne "Y") {
        Write-Host "Installation cancelled by user." -ForegroundColor Yellow
        Read-Host "Press Enter to exit"
        exit 0
    }
}

# Install plugin
Write-Host "Installing plugin..." -ForegroundColor Yellow
try {
    Copy-Item $pluginDll $targetPath -Force
    Write-Host "Plugin installed successfully!" -ForegroundColor Green
} catch {
    Write-Host "ERROR: Failed to copy plugin file: $_" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Restart Veyon Service
Write-Host "Restarting Veyon Service..." -ForegroundColor Yellow
try {
    $service = Get-Service -Name "VeyonService" -ErrorAction SilentlyContinue
    if ($service) {
        if ($service.Status -eq "Running") {
            Stop-Service -Name "VeyonService" -Force
            Write-Host "Veyon Service stopped." -ForegroundColor Green
            Start-Sleep -Seconds 2
        }
        Start-Service -Name "VeyonService"
        Write-Host "Veyon Service started." -ForegroundColor Green
    } else {
        Write-Host "WARNING: Veyon Service not found." -ForegroundColor Yellow
        Write-Host "Please restart the service manually if it's running." -ForegroundColor Yellow
    }
} catch {
    Write-Host "WARNING: Failed to restart Veyon Service: $_" -ForegroundColor Yellow
    Write-Host "Please restart the service manually." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Installation Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "The Veyon Chat Plugin has been installed successfully." -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Restart Veyon Master application" -ForegroundColor White
Write-Host "2. Look for the Chat button in the toolbar" -ForegroundColor White
Write-Host "3. Press F10 to open the chat window" -ForegroundColor White
Write-Host "4. Install the plugin on all client computers" -ForegroundColor White
Write-Host ""
Write-Host "For detailed usage instructions, see the README.md file." -ForegroundColor Cyan
Write-Host ""

# Optional: Open plugin directory
$response = Read-Host "Would you like to open the plugins directory? (y/N)"
if ($response -eq "y" -or $response -eq "Y") {
    Start-Process explorer.exe $pluginDir
}

Read-Host "Press Enter to exit"
