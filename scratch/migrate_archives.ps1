$ErrorActionPreference = "Stop"
$PromptsDir = "d:\GitHub\UITS\CGMLabProject_2DGame\doc\prompts"
$MergedFile = Join-Path $PromptsDir "v1.0-v4.17 - Merged Prompts Archive.md"

$SessionMeta = @{
    1 = @{
        Title = "Phase 1 Prototype & Documentation"
        Folder = "1. Phase 1 Prototype & Documentation"
    }
    2 = @{
        Title = "Project Structure & Archiving Guidelines"
        Folder = "2. Project Structure & Archiving Guidelines"
    }
    3 = @{
        Title = "Multi-Directional Engine, Modular Architecture & Guides"
        Folder = "3. Multi-Directional Engine, Modular Architecture & Guides"
    }
    4 = @{
        Title = "Samurai Sprites, Mini-Games, Video Playback & Shields"
        Folder = "4. Samurai Sprites, Mini-Games, Video Playback & Shields"
    }
    5 = @{
        Title = "Combat System, Roaming Enemies, Boss Battle & Audio"
        Folder = "5. Combat System, Roaming Enemies, Boss Battle & Audio"
    }
    6 = @{
        Title = "Build Pipeline, Makefile & CMake Validation"
        Folder = "6. Build Pipeline, Makefile & CMake Validation"
    }
    7 = @{
        Title = "Academic Intro Splash Screen & OpenGL Rendering"
        Folder = "7. Academic Intro Splash Screen & OpenGL Rendering"
    }
    8 = @{
        Title = "Code Snippets, Academic Metadata & Documentation Sync"
        Folder = "8. Code Snippets, Academic Metadata & Documentation Sync"
    }
}

function UrlEncodeFolder([string]$name) {
    return $name.Replace(" ", "%20")
}

function Parse-TurnsFromMerged([string]$path) {
    $raw = Get-Content -Path $path -Raw -Encoding UTF8
    $lines = $raw -split "`r?`n"
    $turns = @()
    $curTitle = $null
    $curBody = [System.Collections.Generic.List[string]]::new()

    foreach ($line in $lines) {
        if ($line -match '^## (.+ - v(\d+)\.(\d+))$') {
            if ($null -ne $curTitle) {
                $turns += [PSCustomObject]@{
                    Major = [int]$curMajor
                    Minor = [int]$curMinor
                    Version = "v$curMajor.$curMinor"
                    Title = $curHeadingTitle
                    RawBody = ($curBody -join "`n").Trim()
                }
            }
            $curHeadingTitle = $Matches[1]
            $curMajor = [int]$Matches[2]
            $curMinor = [int]$Matches[3]
            $curTitle = $curHeadingTitle
            $curBody = [System.Collections.Generic.List[string]]::new()
        } elseif ($null -ne $curTitle) {
            [void]$curBody.Add($line)
        }
    }
    if ($null -ne $curTitle) {
        $turns += [PSCustomObject]@{
            Major = [int]$curMajor
            Minor = [int]$curMinor
            Version = "v$curMajor.$curMinor"
            Title = $curHeadingTitle
            RawBody = ($curBody -join "`n").Trim()
        }
    }
    return $turns
}

function Parse-TurnFromFlatFile([string]$path) {
    $raw = Get-Content -Path $path -Raw -Encoding UTF8
    $fname = [System.IO.Path]::GetFileName($path)
    if ($fname -match '^v(\d+)\.(\d+)\s*-\s*(.+)\.md$') {
        $major = [int]$Matches[1]
        $minor = [int]$Matches[2]
        $titleFromFile = $Matches[3]
    } else {
        throw "Unexpected filename format: $fname"
    }
    # Strip leading `# Session Archive: ...` line so body integrates cleanly under `## Turn vX.Y - ...`
    $lines = $raw -split "`r?`n"
    $bodyLines = [System.Collections.Generic.List[string]]::new()
    $skippedTitle = $false
    foreach ($line in $lines) {
        if (-not $skippedTitle -and $line -match '^# Session Archive:\s*(.+)$') {
            $titleFromFile = $Matches[1].Trim()
            $skippedTitle = $true
            continue
        }
        [void]$bodyLines.Add($line)
    }
    return [PSCustomObject]@{
        Major = $major
        Minor = $minor
        Version = "v$major.$minor"
        Title = $titleFromFile
        RawBody = ($bodyLines -join "`n").Trim()
    }
}

function Extract-Metadata([string]$rawBody) {
    $dateVal = ""
    $ctxVal = ""
    if ($rawBody -match '\*\*Date\*\*:\s*([^\r\n]+)') { $dateVal = $Matches[1].Trim() }
    if ($rawBody -match '\*\*Context\*\*:\s*([^\r\n]+)') { $ctxVal = $Matches[1].Trim() }
    return @{ Date = $dateVal; Context = $ctxVal }
}

function Extract-Section([string]$rawBody, [string]$headerRegex) {
    $lines = $rawBody -split "`r?`n"
    $inSec = $false
    $secLines = [System.Collections.Generic.List[string]]::new()
    foreach ($l in $lines) {
        if ($l -match '^###\s+(.+)$') {
            if ($inSec) { break }
            if ($Matches[1] -match $headerRegex) {
                $inSec = $true
                continue
            }
        } elseif ($inSec) {
            if ($l -eq '---' -and $secLines.Count -gt 0) {
                # keep going unless next heading follows
            }
            [void]$secLines.Add($l)
        }
    }
    return ($secLines -join "`n").Trim()
}

# Collect all turns grouped by Major 1..8
$AllTurns = @()
$AllTurns += Parse-TurnsFromMerged $MergedFile

$FlatFiles = Get-ChildItem -Path $PromptsDir -Filter "v*.md" | Where-Object { $_.Name -ne "v1.0-v4.17 - Merged Prompts Archive.md" }
foreach ($f in $FlatFiles) {
    $AllTurns += Parse-TurnFromFlatFile $f.FullName
}

$Grouped = $AllTurns | Group-Object Major | Sort-Object { [int]$_.Name }

foreach ($grp in $Grouped) {
    $major = [int]$grp.Name
    $meta = $SessionMeta[$major]
    $folderName = $meta.Folder
    $title = $meta.Title
    $destDir = Join-Path $PromptsDir $folderName
    if (!(Test-Path $destDir)) {
        New-Item -ItemType Directory -Path $destDir -Force | Out-Null
    }

    $turnsSorted = $grp.Group | Sort-Object Minor
    $firstVer = $turnsSorted[0].Version
    $lastVer = $turnsSorted[-1].Version
    $verRange = if ($firstVer -eq $lastVer) { $firstVer } else { "$firstVer - $lastVer" }
    $encodedFolder = UrlEncodeFolder $folderName

    $dates = @()
    foreach ($t in $turnsSorted) {
        $m = Extract-Metadata $t.RawBody
        if ($m.Date -and ($dates -notcontains $m.Date)) { $dates += $m.Date }
    }
    $dateRange = if ($dates.Count -eq 1) { $dates[0] } elseif ($dates.Count -gt 1) { "$($dates[0]) to $($dates[-1])" } else { "Legacy Archive" }

    # 1. Build conversation.md (verbatim migration of every turn)
    $convSb = [System.Text.StringBuilder]::new()
    [void]$convSb.AppendLine("# Conversation Archive: Session $major - $title")
    [void]$convSb.AppendLine("")
    [void]$convSb.AppendLine("- **Session Number**: $major")
    [void]$convSb.AppendLine("- **Legacy Versions**: ``$verRange``")
    [void]$convSb.AppendLine("- **Date(s)**: $dateRange")
    [void]$convSb.AppendLine("- **Session Folder**: [``/doc/prompts/$folderName/``](/doc/prompts/$encodedFolder/)")
    [void]$convSb.AppendLine("- **Transcript Status**: Migrated verbatim from legacy Markdown session logs (raw ``transcript_full.jsonl`` not available for pre-Session-9 conversations).")
    [void]$convSb.AppendLine("")
    [void]$convSb.AppendLine("---")
    [void]$convSb.AppendLine("")

    if ($major -eq 1) {
        $b1 = (Get-Content (Join-Path $PromptsDir "b1tranger.txt") -Raw -Encoding UTF8).Trim()
        $lm = (Get-Content (Join-Path $PromptsDir "lima.txt") -Raw -Encoding UTF8).Trim()
        [void]$convSb.AppendLine("## Original Raw Prompt Notes (Pre-Archive)")
        [void]$convSb.AppendLine("")
        [void]$convSb.AppendLine("### ``b1tranger.txt``")
        [void]$convSb.AppendLine('```text')
        [void]$convSb.AppendLine($b1)
        [void]$convSb.AppendLine('```')
        [void]$convSb.AppendLine("")
        [void]$convSb.AppendLine("### ``lima.txt``")
        [void]$convSb.AppendLine('```text')
        [void]$convSb.AppendLine($lm)
        [void]$convSb.AppendLine('```')
        [void]$convSb.AppendLine("")
        [void]$convSb.AppendLine("---")
        [void]$convSb.AppendLine("")
    }

    foreach ($t in $turnsSorted) {
        [void]$convSb.AppendLine("## Turn $($t.Version) - $($t.Title)")
        [void]$convSb.AppendLine("")
        [void]$convSb.AppendLine($t.RawBody)
        [void]$convSb.AppendLine("")
        [void]$convSb.AppendLine("---")
        [void]$convSb.AppendLine("")
    }

    [System.IO.File]::WriteAllText((Join-Path $destDir "conversation.md"), $convSb.ToString(), [System.Text.UTF8Encoding]::new($false))

    # 2. Build summary.md matching Session 9's structure
    $sumSb = [System.Text.StringBuilder]::new()
    [void]$sumSb.AppendLine("# Session $major Summary: $title")
    [void]$sumSb.AppendLine("")
    [void]$sumSb.AppendLine("- **Session Number**: $major")
    [void]$sumSb.AppendLine("- **Legacy Versions**: ``$verRange``")
    [void]$sumSb.AppendLine("- **Session Folder**: [``/doc/prompts/$folderName/``](/doc/prompts/$encodedFolder/)")
    [void]$sumSb.AppendLine("- **Date(s)**: $dateRange")
    [void]$sumSb.AppendLine("- **Conversation Log**: [``conversation.md``](/doc/prompts/$encodedFolder/conversation.md)")
    [void]$sumSb.AppendLine("- **Transcript Status**: Migrated from legacy Markdown archives (raw ``transcript_full.jsonl`` unavailable).")
    [void]$sumSb.AppendLine("")
    [void]$sumSb.AppendLine("---")
    [void]$sumSb.AppendLine("")
    [void]$sumSb.AppendLine("## 1. User Goals")
    foreach ($t in $turnsSorted) {
        $m = Extract-Metadata $t.RawBody
        $ctx = if ($m.Context) { $m.Context } else { $t.Title }
        [void]$sumSb.AppendLine("- **$($t.Version)** ($($t.Title)): $ctx")
    }
    [void]$sumSb.AppendLine("")
    [void]$sumSb.AppendLine("---")
    [void]$sumSb.AppendLine("")
    [void]$sumSb.AppendLine("## 2. Engineering & Implementation Details")
    foreach ($t in $turnsSorted) {
        [void]$sumSb.AppendLine("### $($t.Version) - $($t.Title)")
        $analysis = Extract-Section $t.RawBody "AI Technical Analysis|High-Level Architectural"
        if (-not [string]::IsNullOrWhiteSpace($analysis)) {
            [void]$sumSb.AppendLine($analysis)
        } else {
            $m = Extract-Metadata $t.RawBody
            [void]$sumSb.AppendLine($m.Context)
        }
        [void]$sumSb.AppendLine("")
    }
    [void]$sumSb.AppendLine("---")
    [void]$sumSb.AppendLine("")
    [void]$sumSb.AppendLine("## 3. Actions, Files & Verification")
    foreach ($t in $turnsSorted) {
        $actions = Extract-Section $t.RawBody "Actions"
        if (-not [string]::IsNullOrWhiteSpace($actions)) {
            [void]$sumSb.AppendLine("### $($t.Version)")
            [void]$sumSb.AppendLine($actions)
            [void]$sumSb.AppendLine("")
        }
    }
    [void]$sumSb.AppendLine("---")
    [void]$sumSb.AppendLine("")
    [void]$sumSb.AppendLine("## 4. Outcomes")
    foreach ($t in $turnsSorted) {
        $resp = Extract-Section $t.RawBody "Final Response"
        if (-not [string]::IsNullOrWhiteSpace($resp)) {
            [void]$sumSb.AppendLine("- **$($t.Version)**: $resp")
        }
    }

    [System.IO.File]::WriteAllText((Join-Path $destDir "summary.md"), $sumSb.ToString(), [System.Text.UTF8Encoding]::new($false))
    Write-Host ("Created Session " + $major + ": " + $turnsSorted.Count + " turns -> " + $folderName)
}

