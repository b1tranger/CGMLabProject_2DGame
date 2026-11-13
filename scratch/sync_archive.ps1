param(
    [string]$TranscriptSrc = "C:\Users\gsmur\.gemini\antigravity\brain\2d323f1e-f3b4-4713-94a4-8240185698a6\.system_generated\logs\transcript_full.jsonl",
    [string]$ArchiveDir = "d:\GitHub\UITS\CGMLabProject_2DGame\doc\prompts\11. Web Port & Itch.io Deployment Guide",
    [string]$PendingFile = "",
    [string]$ConversationId = "2d323f1e-f3b4-4713-94a4-8240185698a6"
)

$PendingResponse = ""
if (-not [string]::IsNullOrEmpty($PendingFile) -and (Test-Path $PendingFile)) {
    $PendingResponse = Get-Content -Path $PendingFile -Raw -Encoding UTF8
}

if (!(Test-Path $ArchiveDir)) {
    New-Item -ItemType Directory -Path $ArchiveDir -Force | Out-Null
}

$folderName = Split-Path -Leaf $ArchiveDir
$encodedFolderName = [System.Uri]::EscapeDataString($folderName)

$TranscriptDest = Join-Path $ArchiveDir "transcript_full.jsonl"
$ConvDest = Join-Path $ArchiveDir "conversation.md"

Copy-Item -Path $TranscriptSrc -Destination $TranscriptDest -Force

$sb = [System.Text.StringBuilder]::new()
[void]$sb.AppendLine("# Conversation Archive: $folderName")
[void]$sb.AppendLine('')
[void]$sb.AppendLine("- **Conversation ID**: ``$ConversationId``")
[void]$sb.AppendLine(('- **Session Folder**: [`/doc/prompts/' + $folderName + '/`](/doc/prompts/' + $encodedFolderName + '/)'))
[void]$sb.AppendLine(('- **Transcript Source**: [`transcript_full.jsonl`](/doc/prompts/' + $encodedFolderName + '/transcript_full.jsonl)'))
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
