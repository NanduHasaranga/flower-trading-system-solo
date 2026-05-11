param(
    [string]$BuildDir = "build",
    [string]$OutputName = "FlowerTradingSystem_AllInOne.exe",
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$Config = "Release",
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptRoot "..")
$buildPath = Join-Path $repoRoot $BuildDir
$releaseRoot = Join-Path $repoRoot "release"
$stagePath = Join-Path $releaseRoot "stage"
$launcherTemp = Join-Path $releaseRoot "launcher_temp"
$bundlePath = Join-Path $releaseRoot "single_exe"
$payloadZip = Join-Path $launcherTemp "payload.zip"

if (!(Test-Path $buildPath)) {
    throw "Build directory not found: $buildPath"
}

if (-not (Get-Command dotnet -ErrorAction SilentlyContinue)) {
    throw "dotnet CLI is required but was not found in PATH."
}

$traderProcess = Get-Process -Name TraderApp -ErrorAction SilentlyContinue
if ($null -ne $traderProcess) {
    $traderProcess | Stop-Process -Force
}

if (Test-Path $stagePath) {
    Remove-Item -Recurse -Force $stagePath
}
New-Item -ItemType Directory -Path $stagePath | Out-Null

if (Test-Path $launcherTemp) {
    Remove-Item -Recurse -Force $launcherTemp
}
New-Item -ItemType Directory -Path $launcherTemp | Out-Null

if (Test-Path $bundlePath) {
    Remove-Item -Recurse -Force $bundlePath
}
New-Item -ItemType Directory -Path $bundlePath | Out-Null

if (-not $SkipBuild) {
    & cmake --build $buildPath --config $Config
}

# Use CMake install + Qt deploy script to stage a runnable app folder.
& cmake --install $buildPath --config $Config --prefix $stagePath

Compress-Archive -Path (Join-Path $stagePath "*") -DestinationPath $payloadZip -Force

$csprojPath = Join-Path $launcherTemp "Launcher.csproj"
$programPath = Join-Path $launcherTemp "Program.cs"

$csprojContent = @'
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>WinExe</OutputType>
    <TargetFramework>net8.0-windows</TargetFramework>
    <UseWindowsForms>true</UseWindowsForms>
    <Nullable>enable</Nullable>
    <ImplicitUsings>enable</ImplicitUsings>
    <AssemblyName>FlowerTradingSystem_AllInOne</AssemblyName>
    <RootNamespace>FlowerTradingSystemAllInOne</RootNamespace>
    <PublishSingleFile>true</PublishSingleFile>
    <SelfContained>true</SelfContained>
    <RuntimeIdentifier>win-x64</RuntimeIdentifier>
    <IncludeNativeLibrariesForSelfExtract>true</IncludeNativeLibrariesForSelfExtract>
    <EnableCompressionInSingleFile>true</EnableCompressionInSingleFile>
    <PublishTrimmed>false</PublishTrimmed>
    <DebugType>none</DebugType>
  </PropertyGroup>

  <ItemGroup>
    <EmbeddedResource Include="payload.zip" />
  </ItemGroup>
</Project>
'@

$programContent = @'
using System;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Reflection;
using System.Security.Cryptography;
using System.Windows.Forms;

internal static class Program
{
    [STAThread]
    private static int Main()
    {
        try
        {
            string resourceName = Assembly.GetExecutingAssembly()
                .GetManifestResourceNames()
                .FirstOrDefault(n => n.EndsWith("payload.zip", StringComparison.OrdinalIgnoreCase))
                ?? throw new InvalidOperationException("Embedded payload.zip was not found.");

            byte[] zipBytes;
            using (Stream stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(resourceName)
                                  ?? throw new InvalidOperationException("Unable to read embedded payload.zip."))
            using (var ms = new MemoryStream())
            {
                stream.CopyTo(ms);
                zipBytes = ms.ToArray();
            }

            string hash;
            using (var sha = SHA256.Create())
            {
                hash = Convert.ToHexString(sha.ComputeHash(zipBytes)).Substring(0, 12);
            }

            string extractRoot = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                "FlowerTradingSystem",
                "TraderAppRuntime",
                hash);

            string markerPath = Path.Combine(extractRoot, ".extracted");
            if (!File.Exists(markerPath))
            {
                if (Directory.Exists(extractRoot))
                {
                    Directory.Delete(extractRoot, true);
                }

                Directory.CreateDirectory(extractRoot);

                using var payloadStream = new MemoryStream(zipBytes);
                using var archive = new ZipArchive(payloadStream, ZipArchiveMode.Read);

                foreach (ZipArchiveEntry entry in archive.Entries)
                {
                    string destinationPath = Path.Combine(extractRoot, entry.FullName);

                    if (string.IsNullOrEmpty(entry.Name))
                    {
                        Directory.CreateDirectory(destinationPath);
                        continue;
                    }

                    string? destinationDir = Path.GetDirectoryName(destinationPath);
                    if (!string.IsNullOrEmpty(destinationDir))
                    {
                        Directory.CreateDirectory(destinationDir);
                    }

                    entry.ExtractToFile(destinationPath, true);
                }

                File.WriteAllText(markerPath, DateTime.UtcNow.ToString("O"));
            }

            string appPath = Path.Combine(extractRoot, "bin", "TraderApp.exe");
            if (!File.Exists(appPath))
            {
                throw new FileNotFoundException("TraderApp.exe was not found after extraction.", appPath);
            }

            var psi = new ProcessStartInfo(appPath)
            {
                WorkingDirectory = Path.GetDirectoryName(appPath) ?? extractRoot,
                UseShellExecute = true
            };

            Process.Start(psi);
            return 0;
        }
        catch (Exception ex)
        {
            MessageBox.Show(
                ex.Message,
                "Flower Trading System Launcher Error",
                MessageBoxButtons.OK,
                MessageBoxIcon.Error);
            return 1;
        }
    }
}
'@

Set-Content -Path $csprojPath -Value $csprojContent -Encoding UTF8
Set-Content -Path $programPath -Value $programContent -Encoding UTF8

& dotnet publish $csprojPath -c Release -r win-x64 --self-contained true -o $bundlePath

$publishedExe = Join-Path $bundlePath "FlowerTradingSystem_AllInOne.exe"
if (!(Test-Path $publishedExe)) {
    throw "Single executable was not generated as expected."
}

if ($OutputName -ne "FlowerTradingSystem_AllInOne.exe") {
    $finalPath = Join-Path $bundlePath $OutputName
    if (Test-Path $finalPath) {
        Remove-Item -Force $finalPath
    }
    Move-Item -Path $publishedExe -Destination $finalPath
    $publishedExe = $finalPath
}

Write-Output "Single executable created at: $publishedExe"
