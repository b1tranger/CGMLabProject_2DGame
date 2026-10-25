Add-Type -AssemblyName System.Drawing

$castFiles = 1..9 | ForEach-Object { "assets/enemies/bringer_of_death/Bringer-Of-Death/Individual Sprite/Cast/Bringer-of-Death_Cast_$_.png" }
$spellFiles = 1..16 | ForEach-Object { "assets/enemies/bringer_of_death/Bringer-Of-Death/Individual Sprite/Spell/Bringer-of-Death_Spell_$_.png" }

# Inspect Cast
$castBitmaps = $castFiles | ForEach-Object { [System.Drawing.Bitmap]::FromFile((Resolve-Path $_)) }
$frameW = $castBitmaps[0].Width
$frameH = $castBitmaps[0].Height
Write-Host "Cast frames: $($castBitmaps.Count), size: ${frameW}x${frameH}"

# Stitch Cast Sheet: 9 frames horizontal -> (9 * 140) x 93 = 1260 x 93
$castSheet = New-Object System.Drawing.Bitmap ($frameW * $castBitmaps.Count), $frameH
$gCast = [System.Drawing.Graphics]::FromImage($castSheet)
$gCast.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor

for ($i = 0; $i -lt $castBitmaps.Count; $i++) {
    $gCast.DrawImage($castBitmaps[$i], ($i * $frameW), 0, $frameW, $frameH)
}
$gCast.Dispose()
$castBitmaps | ForEach-Object { $_.Dispose() }
$castSheet.Save("assets/enemies/bod_cast.png", [System.Drawing.Imaging.ImageFormat]::Png)
$castSheet.Dispose()
Write-Host "Saved assets/enemies/bod_cast.png"

# Inspect Spell
$spellBitmaps = $spellFiles | ForEach-Object { [System.Drawing.Bitmap]::FromFile((Resolve-Path $_)) }
$sW = $spellBitmaps[0].Width
$sH = $spellBitmaps[0].Height
Write-Host "Spell frames: $($spellBitmaps.Count), size: ${sW}x${sH}"

# Stitch Spell Sheet: 16 frames horizontal -> (16 * 140) x 93 = 2240 x 93
$spellSheet = New-Object System.Drawing.Bitmap ($sW * $spellBitmaps.Count), $sH
$gSpell = [System.Drawing.Graphics]::FromImage($spellSheet)
$gSpell.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor

for ($i = 0; $i -lt $spellBitmaps.Count; $i++) {
    $gSpell.DrawImage($spellBitmaps[$i], ($i * $sW), 0, $sW, $sH)
}
$gSpell.Dispose()
$spellBitmaps | ForEach-Object { $_.Dispose() }
$spellSheet.Save("assets/enemies/bod_spell.png", [System.Drawing.Imaging.ImageFormat]::Png)
$spellSheet.Dispose()
Write-Host "Saved assets/enemies/bod_spell.png"
