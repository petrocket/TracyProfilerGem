# TracyProfilerGem
A Lumberyard Gem providing integration with Tracy Profiler https://github.com/wolfpld/tracy


**To install**
1. Clone this repo to your Gems folder using git clone --recurse-submodules <repository url>
2. Build TracyProfiler server application and library
   
   a. Run Gems/TracyProfiler/External/tracy/vcpkg/install_vcpkg_dependencies.bat
   b. Open Gems/TracyProfiler/External/tracy/profiler/build/win32/Tracy.sln in Visual Studio 2019 and build release mode
   c.  Tracy.exe should now be in the Gems/TracyProfiler/External/tracy/profiler/build/win32/x64/Release folder - run Tracy.exe, this will capture and display profiling data from Lumberyard
   d. Open Gems/TracyProfiler/External/tracy/library/win32/Tracy.sln in Visual Studio 2019 and build release mode
3. Add the gem to your project using ProjectConfigurator.exe
4. Modify dev/Code/Frameworks/AzCore/AzCore/Profiler.h and add an include for the TracyProfiler header like there is for RAD Telemetry near #ifdef AZ_PROFILE_TELEMETRY
#ifdef TRACY_ENABLE
#   include <TracyProfiler/TracyProfiler.h>
#endif 

5. Modify dev/Code/Frameworks/AzCore/AzCore/wscript.h and add a use for TRACY_PROFILER like there is for RAD_TELEMETRY like so...
use = [
   ...
   'RAD_TELEMETRY',
   'TRACY_PROFILER',
   ...
   ]

6. configure and compile your project
7. run the editor or game launcher and watch the data appear in the Tracy Profiler app (Tracy.exe).  Congrats you're gathering profiling data! 


**Troubleshooting**
If you run into errors with numeric_limits::max, try modifying any files that bring in windows.h (LocalFileIO_WinAPI.cpp, LocalFileIO_Windows.cpp, RemoteConsoleCore.cpp, FileIOHandler_wwise.cpp  EMotionFX/GLInclude.h and DeploymentTool_precompiled.h) and put
#define WINDOWS_LEAN_AND_MEAN 
#define NOMINMAX 
at the top of the file (before the #include <windows.h> or #include <WinSoc2.h> which brings in windows.h)
This fixes an issue with a collision between min/max and std numeric_limits min/max
