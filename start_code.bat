set PATH=%~dp0\tools\bin;%~dp0\mruby-2.1.1\bin;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%

@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
call "C:\Program Files (x86)\Microsoft Azure Sphere SDK\InitializeCommandPrompt.cmd"
@echo on

start azure_sphere_mruby.code-workspace

