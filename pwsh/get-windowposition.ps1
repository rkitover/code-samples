<#
This will return the rectangle position of a process window. 
Values of the Rectangle in relation to pixel location on window:
Left;        // x position of upper-left corner
Top;         // y position of upper-left corner
Right;       // x position of lower-right corner
Bottom;      // y position of lower-right corner
#>
Function Get-WindowPosition {
    [cmdletbinding()]
    Param (
        [parameter(ValueFromPipelineByPropertyName=$True)]
        $ProcessName
    )
    Begin {
        Add-Type @"
          using System;
          using System.Runtime.InteropServices;
          public class Window {
            [DllImport("user32.dll")]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
          }
          public struct RECT
          {
            public int Left;        // x position of upper-left corner
            public int Top;         // y position of upper-left corner
            public int Right;       // x position of lower-right corner
            public int Bottom;      // y position of lower-right corner
          }
"@
    }
    Process {
        $Rectangle = New-Object RECT
        $Handle = (Get-Process -Name $ProcessName).MainWindowHandle
        $Return = [Window]::GetWindowRect($Handle,[ref]$Rectangle)
        If ($Return) {
            $Height = $Rectangle.Bottom - $Rectangle.Top
            $Width = $Rectangle.Right - $Rectangle.Left
            $Size = New-Object System.Management.Automation.Host.Size -ArgumentList $Width, $Height
            $TopLeft = New-Object System.Management.Automation.Host.Coordinates -ArgumentList $Rectangle.Left, $Rectangle.Top
            $BottomRight = New-Object System.Management.Automation.Host.Coordinates -ArgumentList $Rectangle.Right, $Rectangle.Bottom
            If ($Rectangle.Top -lt 0 -AND $Rectangle.Left -lt 0) {
                Write-Warning "Window is minimized! Coordinates will not be accurate."
            }
            [pscustomobject]@{
                Size = $Size
                TopLeft = $TopLeft
                BottomRight = $BottomRight
            }
        }
    }
}
