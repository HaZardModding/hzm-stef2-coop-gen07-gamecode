$blacklist = "__cleanUp_blacklist.txt"
$dryrun = $false
$logfile = "__cleanup_delete.log"
$filesDeleted = 0
$dirsDeleted = 0
$groupedLog = @{}

if (Test-Path $logfile) {
    Remove-Item $logfile -Force
}

if (!(Test-Path $blacklist)) {
    Write-Host "$blacklist not found!"
    exit 1
}

Get-Content $blacklist | ForEach-Object {
    $line = $_.Trim()

    if (-not [string]::IsNullOrWhiteSpace($line) -and -not $line.StartsWith("#")) {
        $pattern = $line -replace '/', '\'
        if ($pattern.EndsWith("\")) {
            $pattern = $pattern.Substring(0, $pattern.Length - 1)
        }

        $matches = Get-ChildItem -Recurse -Force -Filter "$(Split-Path $pattern -Leaf)" -ErrorAction SilentlyContinue |
            Where-Object { $_.FullName -like "*$($pattern)" }

        foreach ($item in $matches) {
            $target = $item.FullName
            $folder = Split-Path $target -Parent

            if (-not (Test-Path $target)) {
                continue
            }

            if (-not $groupedLog.ContainsKey($folder)) {
                $groupedLog[$folder] = @()
            }

            if ($dryrun) {
                if ($item.PSIsContainer) {
                    Write-Host "[DRY-RUN] Would delete directory: $target"
                    $groupedLog[$folder] += "[DRY-RUN] Would delete directory: $target"
                } else {
                    Write-Host "[DRY-RUN] Would delete file: $target"
                    $groupedLog[$folder] += "[DRY-RUN] Would delete file: $target"
                }
            } else {
                if ($item.PSIsContainer) {
                    Remove-Item -Recurse -Force -LiteralPath $target -ErrorAction SilentlyContinue
                    $groupedLog[$folder] += "Deleted directory: $target"
                    $dirsDeleted++
                } else {
                    Remove-Item -Force -LiteralPath $target -ErrorAction SilentlyContinue
                    $groupedLog[$folder] += "Deleted file: $target"
                    $filesDeleted++
                }
            }
        }
    }
}

foreach ($group in ($groupedLog.Keys | Sort-Object)) {
    Add-Content -Path $logfile -Value "[$group]"
    $groupedLog[$group] | ForEach-Object { Add-Content -Path $logfile -Value $_ }
    Add-Content -Path $logfile -Value ""
}

if (-not $dryrun) {
    Write-Host "Deleted files: $filesDeleted"
    Write-Host "Deleted directories: $dirsDeleted"
    Add-Content -Path $logfile -Value "Deleted files: $filesDeleted"
    Add-Content -Path $logfile -Value "Deleted directories: $dirsDeleted"
}

Write-Host "Cleanup complete. See $logfile for details."
Pause
