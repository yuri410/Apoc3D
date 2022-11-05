@echo off
Set Path=%Path%;C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin

msbuild assimp\workspaces\vc9\assimp.vcxproj -nologo -m -t:Rebuild -p:Configuration=debug-noboost-st -p:Platform=Win32
msbuild assimp\workspaces\vc9\assimp.vcxproj -nologo -m -t:Rebuild -p:Configuration=debug-noboost-st -p:Platform=x64
msbuild assimp\workspaces\vc9\assimp.vcxproj -nologo -m -t:Rebuild -p:Configuration=release-noboost-st -p:Platform=Win32
msbuild assimp\workspaces\vc9\assimp.vcxproj -nologo -m -t:Rebuild -p:Configuration=release-noboost-st -p:Platform=x64
msbuild assimp\workspaces\vc9\assimp.vcxproj -nologo -m -t:Rebuild -p:Configuration=release-noboost-st-mt -p:Platform=Win32
msbuild assimp\workspaces\vc9\assimp.vcxproj -nologo -m -t:Rebuild -p:Configuration=release-noboost-st-mt -p:Platform=x64

msbuild freetype-2.4.11\builds\win32\vc2010\freetype.vcxproj -nologo -m -t:Rebuild -p:Configuration=Debug -p:Platform=Win32
msbuild freetype-2.4.11\builds\win32\vc2010\freetype.vcxproj -nologo -m -t:Rebuild -p:Configuration=Debug -p:Platform=x64
msbuild freetype-2.4.11\builds\win32\vc2010\freetype.vcxproj -nologo -m -t:Rebuild -p:Configuration=Release -p:Platform=Win32
msbuild freetype-2.4.11\builds\win32\vc2010\freetype.vcxproj -nologo -m -t:Rebuild -p:Configuration=Release -p:Platform=x64
msbuild freetype-2.4.11\builds\win32\vc2010\freetype.vcxproj -nologo -m -t:Rebuild -p:"Configuration=Release Multithreaded" -p:Platform=Win32
msbuild freetype-2.4.11\builds\win32\vc2010\freetype.vcxproj -nologo -m -t:Rebuild -p:"Configuration=Release Multithreaded" -p:Platform=x64

msbuild ois-v1-3\Win32\OIS_vc9.vcxproj -nologo -m -t:Rebuild -p:Configuration=Debug -p:Platform=Win32
msbuild ois-v1-3\Win32\OIS_vc9.vcxproj -nologo -m -t:Rebuild -p:Configuration=Debug -p:Platform=x64
msbuild ois-v1-3\Win32\OIS_vc9.vcxproj -nologo -m -t:Rebuild -p:Configuration=Release -p:Platform=Win32
msbuild ois-v1-3\Win32\OIS_vc9.vcxproj -nologo -m -t:Rebuild -p:Configuration=Release -p:Platform=x64
msbuild ois-v1-3\Win32\OIS_vc9.vcxproj -nologo -m -t:Rebuild -p:Configuration=Release_MT -p:Platform=Win32
msbuild ois-v1-3\Win32\OIS_vc9.vcxproj -nologo -m -t:Rebuild -p:Configuration=Release_MT -p:Platform=x64

call deploy.bat