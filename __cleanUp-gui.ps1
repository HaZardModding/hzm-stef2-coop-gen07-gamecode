Add-Type -AssemblyName PresentationFramework

[xml]$xaml = @'
<Window xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        Title="Cleanup Utility" Height="200" Width="400">
    <StackPanel Margin="10">
        <TextBlock Text="Cleanup based on __cleanUp_blacklist.txt" Margin="0,0,0,10"/>
        <Button Name="DryRunButton" Content="Dry Run" Margin="0,0,0,5" Height="30"/>
        <Button Name="DeleteButton" Content="Delete for Real" Margin="0,0,0,5" Height="30"/>
        <Button Name="CloseButton" Content="Exit" Height="30"/>
    </StackPanel>
</Window>
'@

$reader = New-Object System.Xml.XmlNodeReader $xaml
$window = [Windows.Markup.XamlReader]::Load($reader)

$dryRunButton = $window.FindName("DryRunButton")
$deleteButton = $window.FindName("DeleteButton")
$closeButton = $window.FindName("CloseButton")

function Run-Cleanup {
    param($dryrun)

    $blacklist = "__cleanUp_blacklist.txt"
    $logfile = "__cleanup_delete.log"
    $filesDeleted = 0
    $dirsDeleted = 0
    $groupedLog = @{}

    if (Test-Path $logfile) { Remove-Item $logfile -Force }
    if (!(Test-Path $blacklist)) {
        [System.Windows.MessageBox]::Show("$blacklist not found!", "Error", [System.Windows.MessageBoxButton]::OK, [System.Windows.MessageBoxImage]::Error)
        return
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

                if (-not (Test-Path $target)) { continue }
                if (-not $groupedLog.ContainsKey($folder)) { $groupedLog[$folder] = @() }

                if ($dryrun) {
                    if ($item.PSIsContainer) {
                        $groupedLog[$folder] += "[DRY-RUN] Would delete directory: $target"
                    } else {
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
        Add-Content -Path $logfile -Value "Deleted files: $filesDeleted"
        Add-Content -Path $logfile -Value "Deleted directories: $dirsDeleted"
    }

    [System.Windows.MessageBox]::Show("Cleanup complete. See $logfile", "Done", [System.Windows.MessageBoxButton]::OK, [System.Windows.MessageBoxImage]::Information)
}

$dryRunButton.Add_Click({ Run-Cleanup $true })
$deleteButton.Add_Click({ Run-Cleanup $false })
$closeButton.Add_Click({ $window.Close() })

$window.ShowDialog() | Out-Null
