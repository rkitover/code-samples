New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShell -Value "C:\tools\msys64\usr\bin\bash.exe" -PropertyType String -Force

New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShellCommandOption -Value "-c" -PropertyType String -Force

New-ItemProperty -Path "HKLM:\SOFTWARE\OpenSSH" -Name DefaultShellEscapeArguments -Value 1 -PropertyType DWORD -Force
