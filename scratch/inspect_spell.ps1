Add-Type -AssemblyName System.Drawing
$img = [System.Drawing.Bitmap]::FromFile((Resolve-Path 'assets/enemies/bod_spell.png'))
for ($f = 0; $f -lt 16; $f++) {
    $fMinX = 9999; $fMaxX = -1; $fMinY = 9999; $fMaxY = -1
    for ($x = 0; $x -lt 140; $x++) {
        for ($y = 0; $y -lt 93; $y++) {
            $px = $img.GetPixel($f * 140 + $x, $y)
            if ($px.A -gt 15) {
                if ($x -lt $fMinX) { $fMinX = $x }
                if ($x -gt $fMaxX) { $fMaxX = $x }
                if ($y -lt $fMinY) { $fMinY = $y }
                if ($y -gt $fMaxY) { $fMaxY = $y }
            }
        }
    }
    Write-Host "Frame $f : X in [$fMinX, $fMaxX] (center=$([Math]::Round(($fMinX+$fMaxX)/2.0, 1))), Y in [$fMinY, $fMaxY]"
}
$img.Dispose()
