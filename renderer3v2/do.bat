@echo off

rem SOLUTION FOR PLUG_INSPIRER PRODUCTS
rem Put this file (together with the solution)
rem into INTDEV root and call from there.

rem Use this setting to check memory leakage
rem set CL=-DMEMORY_LEAK_DEBUG  
rem set INT_THREAD_CFG=P:4,L:8
rem set INT_THREAD_CFG=P:4,L:8,IMAPS:1,REND:1,PT:1
rem set INT_THREAD_CFG=IMAPS:1,REND:1,PT:1
rem set CL=-DMULT_PROGR
rem set INSP2x64_MASK=AEFGIKMOSUW
rem set MAX_INTERS_NUM=100000
rem set INT_LIMIT_CORES=1

rem Version and path
set INT_VER=1307
set INT_EXT=%~sdp0
set INT_EXT=%INT_EXT:~0,-10%

rem C++ compiler
if -"%VCINSTALLDIR%"- == -""- call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
if -"%VCINSTALLDIR%"- == -""- call "C:\Program Files (x86)\Microsoft Visual Studio\2017\WDExpress\Common7\Tools\VsDevCmd.bat"

rem Set INTEGRA/INTDEV variables
pushd %1
set INTEGRA=%cd%
set INTDEV=%INTEGRA%
set INT_ENVI=%INTEGRA%\envi
set EMBREE=%INTDEV%\embree-3.8.0.x64.vc14.windows
popd

rem Special compiler settings
set CL=-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1 -D_CRT_SECURE_NO_DEPRECATE %CL%

start nit3.sln
