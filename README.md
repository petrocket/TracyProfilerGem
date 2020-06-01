# TracyProfilerGem
A Lumberyard Gem providing integration with Tracy Profiler https://github.com/wolfpld/tracy


## Installation
1. Clone this repo to your Gems folder using git clone --recurse-submodules <repository url>
2. Build TracyProfiler server application and library
   
   - run **Gems/TracyProfiler/External/tracy/vcpkg/install_vcpkg_dependencies.bat**
   - Open **Gems/TracyProfiler/External/tracy/profiler/build/win32/Tracy.sln** in Visual Studio 2019 and build release mode
   -  Tracy.exe should now be in the Gems/TracyProfiler/External/tracy/profiler/build/win32/x64/Release folder - run Tracy.exe, this will capture and display profiling data from Lumberyard
   - Open **Gems/TracyProfiler/External/tracy/library/win32/Tracy.sln** in Visual Studio 2019 and build release mode
3. Add the TracyProfilerGem to your project using ProjectConfigurator.exe
4. Modify **dev/Code/Frameworks/AzCore/AzCore/Profiler.h** and add an include for the TracyProfiler header like there is for RAD Telemetry near #ifdef AZ_PROFILE_TELEMETRY


        #ifdef TRACY_ENABLE
            #include <TracyProfiler/TracyProfiler.h>
        #endif 

5. Modify dev/Code/Frameworks/AzCore/AzCore/wscript.h and add a use for TRACY_PROFILER like there is for RAD_TELEMETRY similar to

        use = [
           ...
           'RAD_TELEMETRY',
           'TRACY_PROFILER',
           ...
           ]

7. configure and compile your project
8. run the editor or game launcher and connect via the Tracy Profiler app (Tracy.exe) and profiling data should appear.  Congrats! 

## Implementation
The bulk #defines that Lumberyard calls can be found in TracyProfilerGem/Code/Include/TracyProfiler.h
The TracySystemComponent is used for naming threads and calling MarkFrame.  In the future, it should provide the functionality to connect/disconnect and enable/disable profiling

## Known issues

1. Enabling memory profiling (AZ_PROFILE_MEMORY_ALLOC) is currently broken because Lumberyard emits mismatching alloc/free calls which causes Tracy profiler to disconnect.
2. Tracy supports image capture but that isn't hooked up yet.
3. Thread names are not displaying correctly though they are set in the TracySystemComponent.cpp OnThreadEnter()
4. Tracy OnDemand profiling is not implemented and would be useful for starting/stopping profiling.


## Troubleshooting
If you run into errors with numeric_limits::max, try modifying any files that bring in windows.h (LocalFileIO_WinAPI.cpp, LocalFileIO_Windows.cpp, RemoteConsoleCore.cpp, FileIOHandler_wwise.cpp  EMotionFX/GLInclude.h, DeploymentTool_precompiled.h, etc) and put

    #define WINDOWS_LEAN_AND_MEAN 
    #define NOMINMAX 
    
at the top of the file before the #include <windows.h> or #include <WinSoc2.h> (which brings in windows.h)

This fixes an issue with a collision between min/max and std numeric_limits min/max
