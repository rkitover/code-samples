using namespace system.collections.generic;
Push-Location $PSScriptRoot
# $ExportRoot = Join-Path $PSScriptRoot './.output'
$ExportRoot = Join-Path $PSScriptRoot './.output'

function Build-ProxyCommand {
    <#
    generates the proxy command template
    #>
    [cmdletbinding()]
    param (
        # Like 'Get-Item'
        [parameter(
            mandatory, position = 0,
            ValueFromPipelineByPropertyName, ValueFromPipeline
        )]
        [string]$CommandName
    )
    process {
        if ( [string]::IsNullOrWhiteSpace( $CommandName ) ) {
            Write-Error 'Empty -CommandName'
            return
        }

        $Dest = "$ExportRoot/ProxyCommand-Create-$CommandName.ps1"
        $Cmd = Get-Command $CommandName -ea stop
        [System.Management.Automation.ProxyCommand]::Create($Cmd)
        | Set-Content -Path $Dest

        write-color `
            -t ("Wrote: '{0}'" -f @( (Get-Item $Dest).FullName)) `
            -c 'magenta'
    }
}

function Build-FuncInfo {
    <#
    .synopsis
        note: gets func info, not proxy commmands
    #>
    [cmdletbinding()]
    param (
        # Function names like 'help'
        [Alias('FuncInfo', 'Name')]
        [parameter(
            mandatory, position = 0,
            ValueFromPipelineByPropertyName, ValueFromPipeline
        )]
        [string]$FunctionInfo
    )
    process {
        if ( [string]::IsNullOrWhiteSpace( $FunctionInfo ) ) {
            Write-Error 'Empty -FunctionInfo'
            return
        }


        $Dest = "$ExportRoot/FuncInfo-Def-$FunctionInfo.ps1"
        $def = Get-Command $FunctionInfo | ForEach-Object definition
        if ($def) {
            $def | Set-Content -Path $Dest
            write-color `
              -t ("Wrote: '{0}'" -f @( (Get-Item $Dest).FullName)) `
              -c 'darkyellow'
        }

        $Dest = "$ExportRoot/FuncInfo-SB-$FunctionInfo.ps1"
        $def = Get-Command $FunctionInfo | ForEach-Object ScriptBlock
        if ($def) {
            $def
            | Set-Content -Path $Dest
            write-color `
              -t ("Wrote: '{0}'" -f @( (Get-Item $Dest).FullName)) `
              -c 'darkyellow'
        }

    }
}


#'Get-Item', 'Set-Item', 'mkdir', 'Get-Help'
#| Build-ProxyCommand
#
#'help', 'f'
#| Build-FuncInfo


# Get-Command -m (_enumerateMyModule) | ForEach-Object Name | Build-ProxyCommand


