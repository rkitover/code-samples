Add-Type '
using System.ComponentModel;
using System.Management.Automation;
using System.Runtime.InteropServices;

[Cmdlet(VerbsCommon.Get, "ShortPathName")]
[OutputType(typeof(string))]
public sealed class GetShortPathNameCommand : PSCmdlet
{
    [Parameter(Mandatory = true, Position = 0)]
    public string[] Path { get; set; } = [];

    [Parameter(Position = 1)]
    public int MaxLength { get; set; } = 500;


    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
    private extern static int GetShortPathNameW(string lpszLongPath, char[] lpszShortPath, int cchBuffer);

    protected override void EndProcessing()
    {
        char[] buffer = new char[MaxLength];
        int len;
        foreach (string path in Path)
        {
            if ((len = GetShortPathNameW(path, buffer, buffer.Length)) == 0)
            {
                WriteError(new ErrorRecord(
                    new Win32Exception(Marshal.GetLastWin32Error()),
                    "WellSh...",
                    ErrorCategory.InvalidResult,
                    path));
            }

            WriteObject(new string(buffer, 0, len));
        }
    }
}' -PassThru -WA 0 -IgnoreWarnings | Import-Module -Assembly { $_.Assembly }
