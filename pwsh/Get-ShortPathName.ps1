function Get-ShortPathName{
    Param([string] $path)

    $MethodDefinition = @'
[DllImport("kernel32.dll", CharSet = CharSet.Unicode, EntryPoint = "GetShortPathNameW", SetLastError = true)]
public static extern int GetShortPathName(string pathName, System.Text.StringBuilder shortName, int cbShortName);
'@

    $Kernel32 = Add-Type -MemberDefinition $MethodDefinition -Name 'Kernel32' -Namespace 'Win32' -PassThru
    $shortPath = New-Object System.Text.StringBuilder(500)
    $retVal = $Kernel32::GetShortPathName($path, $shortPath, $shortPath.Capacity)
    return $shortPath.ToString()
}
