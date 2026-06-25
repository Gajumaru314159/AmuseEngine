param(
    [string]$HtmlDirectory = "docs/html"
)

$resolvedHtmlDirectory = Resolve-Path -LiteralPath $HtmlDirectory -ErrorAction Stop
$encoding = [System.Text.UTF8Encoding]::new($false)

Get-ChildItem -LiteralPath $resolvedHtmlDirectory -Recurse -Filter "group_*.js" | ForEach-Object {
    $path = $_.FullName
    $text = [System.IO.File]::ReadAllText($path)
    $updated = [regex]::Replace($text, '"Amuse::([A-Za-z_][A-Za-z0-9_:]*)"', '"$1"')

    if ($updated -ne $text) {
        [System.IO.File]::WriteAllText($path, $updated, $encoding)
    }
}
