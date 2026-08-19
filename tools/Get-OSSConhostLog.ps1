# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

################################
# This script takes a range of commits and generates
# a commit log with the git2git-excluded file changes
# filtered out.
#
# It also replaces GitHub issue numbers with GH-XXX so
# as to not confuse Git2Git or Azure DevOps.
# Community contributions are tagged with CC- so they
# can be detected later.

[CmdletBinding()]
Param(
    [string]$RevisionRange
)

Function Test-MicrosoftPerson($email) {
	Write-Host "[DEBUG] >>> Entering Test-MicrosoftPerson($email=$email)"
	Write-Host "[DEBUG] <<< Exiting Test-MicrosoftPerson($email=$email)"
    Return $email -like "*@microsoft.com"
}

# Replaces github PR numbers with GH-XXX or CC-XXX (community contribution)
# and issue numbers with GH-XXX
Function Mangle-CommitMessage($object) {
	Write-Host "[DEBUG] >>> Entering Mangle-CommitMessage($object=$object)"
    $Prefix = "GH-"
    If (-Not (Test-MicrosoftPerson $object.Email)) {
        $Prefix = "CC-"
    }

    $s = $object.Subject -Replace "\(#(\d+)\)", "(${Prefix}`$1)"
    $s = $s -Replace "#(\d+)","GH-`$1"
	Write-Host "[DEBUG] <<< Exiting Mangle-CommitMessage($object=$object)"
    $s
}

Function Get-Git2GitIgnoresAsExcludes() {
	Write-Host "[DEBUG] >>> Entering Get-Git2GitIgnoresAsExcludes()"
    $filters = (Get-Content (Join-Path (& git rev-parse --show-toplevel) consolegit2gitfilters.json) | ConvertFrom-Json)
    $excludes = $filters.ContainsFilters | ? { $_ -Ne "/." } | % { $_ -Replace "^/","" }
    $excludes += $filters.SuffixFilters | % { "**/*$_"; "*$_" }
    $excludes += $filters.PrefixFilters | % { "**/$_*"; "$_*" }
	Write-Host "[DEBUG] <<< Exiting Get-Git2GitIgnoresAsExcludes()"
    $excludes | % { ":(top,exclude)$_" }
}

$Excludes = Get-Git2GitIgnoresAsExcludes
Write-Verbose "IGNORING: $Excludes"
$Entries = & git log $RevisionRange --first-parent "--pretty=format:%an%x1C%ae%x1C%s" -- $Excludes |
    ConvertFrom-CSV -Delimiter "`u{001C}" -Header Author,Email,Subject

Write-Verbose ("{0} unfiltered log entries" -f $Entries.Count)

$Grouped = $Entries | Group Email
$Grouped | % {
    $e = $_.Group[0].Email
    $p = $_.Group[0].Author
    "$p ($($_.Group.Count))"
    $_.Group | % {
        If ($_.Subject -Imatch "^Merge") {
            # Skip merge commits
            Return
        }
        $cm = Mangle-CommitMessage $_
        "* $cm"
    }
    ""
}
