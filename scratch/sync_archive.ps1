param(
    [string]$TranscriptSrc = "C:\Users\gsmur\.gemini\antigravity\brain\89776864-ea2f-4258-93e9-47803a82641c\.system_generated\logs\transcript_full.jsonl",
    [string]$ArchiveDir = "d:\GitHub\UITS\CGMLabProject_2DGame\doc\prompts\9. Relics, Dynamic Enemy Spawning, and Music Volume Slider",
    [string]$PendingFile = ""
)

$PendingResponse = ""
if (-not [string]::IsNullOrEmpty($PendingFile) -and (Test-Path $PendingFile)) {
    $PendingResponse = Get-Content -Path $PendingFile -Raw -Encoding UTF8
}

if (!(Test-Path $ArchiveDir)) {
    New-Item -ItemType Directory -Path $ArchiveDir -Force | Out-Null
}

$TranscriptDest = Join-Path $ArchiveDir "transcript_full.jsonl"
$ConvDest = Join-Path $ArchiveDir "conversation.md"

Copy-Item -Path $TranscriptSrc -Destination $TranscriptDest -Force

$sb = [System.Text.StringBuilder]::new()
[void]$sb.AppendLine('# Conversation Archive: Session 9 - Relics, Dynamic Enemy Spawning, and Music Volume Slider')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('- **Conversation ID**: `89776864-ea2f-4258-93e9-47803a82641c`')
[void]$sb.AppendLine('- **Session Folder**: [`/doc/prompts/9. Relics, Dynamic Enemy Spawning, and Music Volume Slider/`](/doc/prompts/9.%20Relics,%20Dynamic%20Enemy%20Spawning,%20and%20Music%20Volume%20Slider/)')
[void]$sb.AppendLine('- **Transcript Source**: [`transcript_full.jsonl`](/doc/prompts/9.%20Relics,%20Dynamic%20Enemy%20Spawning,%20and%20Music%20Volume%20Slider/transcript_full.jsonl)')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('---')
[void]$sb.AppendLine('')

$lines = Get-Content -Path $TranscriptDest -Encoding UTF8
foreach ($line in $lines) {
    if ([string]::IsNullOrWhiteSpace($line)) { continue }
    $entry = $line | ConvertFrom-Json
    $stepIdx = $entry.step_index
    $source = $entry.source
    $type = $entry.type
    $status = $entry.status
    $ts = $entry.created_at

    [void]$sb.AppendLine("## Step $stepIdx - $type ($source / $status)")
    if ($ts) {
        [void]$sb.AppendLine(('- **Timestamp**: `' + $ts + '`'))
    }
    [void]$sb.AppendLine('')

    if ($entry.PSObject.Properties["thinking"] -and -not [string]::IsNullOrEmpty($entry.thinking)) {
        [void]$sb.AppendLine('### Thinking')
        [void]$sb.AppendLine('````markdown')
        [void]$sb.AppendLine($entry.thinking)
        [void]$sb.AppendLine('````')
        [void]$sb.AppendLine('')
    }

    if ($entry.PSObject.Properties["tool_calls"] -and $entry.tool_calls -and $entry.tool_calls.Count -gt 0) {
        [void]$sb.AppendLine('### Tool Calls')
        foreach ($tc in $entry.tool_calls) {
            $tcJson = $tc | ConvertTo-Json -Depth 10 -Compress
            [void]$sb.AppendLine(('- ``' + $tcJson + '``'))
        }
        [void]$sb.AppendLine('')
    }

    if ($entry.PSObject.Properties["content"] -and -not [string]::IsNullOrEmpty($entry.content)) {
        [void]$sb.AppendLine('### Content')
        [void]$sb.AppendLine('````text')
        [void]$sb.AppendLine($entry.content)
        [void]$sb.AppendLine('````')
        [void]$sb.AppendLine('')
    }

    [void]$sb.AppendLine('---')
    [void]$sb.AppendLine('')
}

if (-not [string]::IsNullOrEmpty($PendingResponse)) {
    [void]$sb.AppendLine('## Pending Final Response (Pre-Commit)')
    [void]$sb.AppendLine('> Note: This pre-commit snapshot will be replaced verbatim by the runtime-committed `thinking` + `content` from `transcript_full.jsonl` on the next sync.')
    [void]$sb.AppendLine('')
    [void]$sb.AppendLine($PendingResponse)
    [void]$sb.AppendLine('')
}

[System.IO.File]::WriteAllText($ConvDest, $sb.ToString(), [System.Text.UTF8Encoding]::new($false))
Write-Host ("Synced transcript_full.jsonl and conversation.md (" + $lines.Count + " steps).")
