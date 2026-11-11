$ErrorActionPreference = "Stop"
$ChangelogPath = "d:\GitHub\UITS\CGMLabProject_2DGame\doc\changelog.md"
$PromptsDir = "d:\GitHub\UITS\CGMLabProject_2DGame\doc\prompts"

$FolderMap = @{
    1 = "1.%20Phase%201%20Prototype%20&%20Documentation"
    2 = "2.%20Project%20Structure%20&%20Archiving%20Guidelines"
    3 = "3.%20Multi-Directional%20Engine,%20Modular%20Architecture%20&%20Guides"
    4 = "4.%20Samurai%20Sprites,%20Mini-Games,%20Video%20Playback%20&%20Shields"
    5 = "5.%20Combat%20System,%20Roaming%20Enemies,%20Boss%20Battle%20&%20Audio"
    6 = "6.%20Build%20Pipeline,%20Makefile%20&%20CMake%20Validation"
    7 = "7.%20Academic%20Intro%20Splash%20Screen%20&%20OpenGL%20Rendering"
    8 = "8.%20Code%20Snippets,%20Academic%20Metadata%20&%20Documentation%20Sync"
}

$RawFolderMap = @{
    1 = "1. Phase 1 Prototype & Documentation"
    2 = "2. Project Structure & Archiving Guidelines"
    3 = "3. Multi-Directional Engine, Modular Architecture & Guides"
    4 = "4. Samurai Sprites, Mini-Games, Video Playback & Shields"
    5 = "5. Combat System, Roaming Enemies, Boss Battle & Audio"
    6 = "6. Build Pipeline, Makefile & CMake Validation"
    7 = "7. Academic Intro Splash Screen & OpenGL Rendering"
    8 = "8. Code Snippets, Academic Metadata & Documentation Sync"
}

$lines = Get-Content -Path $ChangelogPath -Encoding UTF8
$newLines = [System.Collections.Generic.List[string]]::new()

foreach ($line in $lines) {
    if ($line -match '^#### Session Archive: \[`([^`]+)`\]\(/doc/prompts/(v(\d+)\.(\d+)[^)#]*)(#[^)]+)?\)$') {
        $label = $Matches[1]
        $major = [int]$Matches[3]
        $minor = [int]$Matches[4]
        if ($FolderMap.ContainsKey($major)) {
            $encFolder = $FolderMap[$major]
            $rawFolder = $RawFolderMap[$major]
            $newLabel = "$rawFolder/conversation.md (v$major.$minor)"
            $newLine = "#### Session Archive: [``$newLabel``](/doc/prompts/$encFolder/conversation.md)"
            [void]$newLines.Add($newLine)
            continue
        }
    }
    if ($line -match '^#### Session Archive: \[`([^`]+)`\]\(/doc/prompts/v1\.0-v4\.17[^#]*#[^)]+\)$') {
        # Handled below if vX.Y is in label
    }
    if ($line -match '^#### Session Archive: \[`([^`]+ - v(\d+)\.(\d+))`\]\(/doc/prompts/v1\.0-v4\.17[^)]+\)$') {
        $major = [int]$Matches[2]
        $minor = [int]$Matches[3]
        if ($FolderMap.ContainsKey($major)) {
            $encFolder = $FolderMap[$major]
            $rawFolder = $RawFolderMap[$major]
            $newLabel = "$rawFolder/conversation.md (v$major.$minor)"
            $newLine = "#### Session Archive: [``$newLabel``](/doc/prompts/$encFolder/conversation.md)"
            [void]$newLines.Add($newLine)
            continue
        }
    }
    [void]$newLines.Add($line)
}

[System.IO.File]::WriteAllText($ChangelogPath, ($newLines -join "`r`n") + "`r`n", [System.Text.UTF8Encoding]::new($false))

# Remove legacy flat files now that all content is migrated into 1..8 serial folders
$legacyFiles = Get-ChildItem -Path $PromptsDir -File
foreach ($lf in $legacyFiles) {
    Remove-Item -Path $lf.FullName -Force
    Write-Host "Removed legacy flat file: $($lf.Name)"
}

# Verify remaining structure in doc/prompts
Get-ChildItem -Path $PromptsDir | Sort-Object { if ($_.Name -match '^(\d+)\.') { [int]$Matches[1] } else { 999 } } | ForEach-Object {
    $children = (Get-ChildItem $_.FullName -Name) -join ", "
    Write-Host "$($_.Name) -> [$children]"
}

