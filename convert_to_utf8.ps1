$files = Get-ChildItem -Recurse -Include *.cpp,*.h,*.ui
foreach ($file in $files) {
    $content = Get-Content -Path $file.FullName -Raw
    $utf8NoBom = New-Object System.Text.UTF8Encoding $false
    [System.IO.File]::WriteAllLines($file.FullName, $content, $utf8NoBom)
    Write-Host "Converted $($file.Name) to UTF-8"
}