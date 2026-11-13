$dir = "d:\GitHub\UITS\CGMLabProject_2DGame\TheWenderer_Web"
$zip = "$dir\TheWenderer_Web_itch.zip"
if (Test-Path $zip) {
    Remove-Item $zip -Force
}
$items = Get-ChildItem -Path $dir -Exclude "*.zip"
Compress-Archive -Path $items.FullName -DestinationPath $zip -Force
Write-Host "Created $zip with $( (Get-Item $zip).Length ) bytes."

