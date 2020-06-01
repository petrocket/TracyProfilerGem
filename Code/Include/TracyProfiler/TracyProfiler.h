/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#ifdef TRACY_ENABLE 
#pragma once

/*!
* ProfileTelemetry.h provides a Tracy Profiler Telemetry specific implementation of the AZ_PROFILE_FUNCTION, AZ_PROFILE_SCOPE, and AZ_PROFILE_SCOPE_DYNAMIC performance instrumentation markers
*/

// disable the "max" macro that windows.h declares because it collides with std::numeric_limits::max which Tracy uses
#pragma push_macro("max")
#undef max
#include <tracy/Tracy.hpp>
#pragma pop_macro("max")

#define  AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category) (static_cast<AZ::Debug::ProfileCategoryPrimitiveType>(1) << static_cast<AZ::Debug::ProfileCategoryPrimitiveType>(category))

// Helpers
#define AZ_INTERNAL_PROF_VERIFY_CAT(category) static_assert(category < AZ::Debug::ProfileCategory::Count, "Invalid profile category")

#define AZ_INTERNAL_PROF_MEMORY_CAT_TO_FLAGS(category) (AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category) | \
        AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(AZ::Debug::ProfileCategory::MemoryReserved))

#define AZ_INTERNAL_PROF_VERIFY_INTERVAL_ID(id) static_assert(sizeof(id) <= sizeof(uint64_t), "Interval id must be a unique value no larger than 64-bits")

#define AZ_INTERNAL_PROF_TM_FUNC_VERIFY_CAT(category, flags) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); \
    ZoneScoped
    //ZoneScoped(flags)
    //tmFunction(AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category), flags)

//namespace TracyProfiler
//{
//    inline void GetName(char* buffer, size_t bufferSize, const char* name, ...)
//    {
//        va_list args;
//        va_start(args, name);
//        const int cNumCharsNeeded = azvsnprintf(buffer, bufferSize, name, args);
//        va_end(args);
//    }
//}
#define TRACY_PROFILER_VA_ARGS_NAME(buffer, size, name, ...) \
        char buffer[size]; \
AZ_PUSH_DISABLE_WARNING(4477, "-Wunknown-warning-option") \
     	azsnprintf(buffer, size, name, ##__VA_ARGS__); \
AZ_POP_DISABLE_WARNING

#define TRACY_PROFILER_ZONE_SCOPED_VA_ARGS(name, ...) \
        TRACY_PROFILER_VA_ARGS_NAME(_markerName, 256, name, __VA_ARGS__) \
        ZoneScopedN(_markerName);
        //ZoneNamedN(markerName,markerName, true);
        //if (cNumCharsNeeded > cBufferSize - 1 || cNumCharsNeeded < 0)
        //{
        //    markerName[cBufferSize - 1] = '\0'; 
        //}

#define AZ_INTERNAL_PROF_TM_ZONE_VERIFY_CAT(category, flags, name, ...) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); \
    TRACY_PROFILER_ZONE_SCOPED_VA_ARGS(name, __VA_ARGS__)
    //tmZone(AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category), flags, __VA_ARGS__)    

#define AZ_INTERNAL_PROF_TM_ZONE_VERIFY_CAT_NAME(category, flags, name) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); \
    ZoneScopedN(name)

#define TMZF_NONE 0
#define TMZF_STALL 1
#define TMZF_IDLE 2

// AZ_PROFILE_FUNCTION
#define AZ_PROFILE_FUNCTION(category) \
    AZ_INTERNAL_PROF_TM_FUNC_VERIFY_CAT(category, TMZF_NONE)

#define AZ_PROFILE_FUNCTION_STALL(category) \
    AZ_INTERNAL_PROF_TM_FUNC_VERIFY_CAT(category, TMZF_STALL)

#define AZ_PROFILE_FUNCTION_IDLE(category) \
    AZ_INTERNAL_PROF_TM_FUNC_VERIFY_CAT(category, TMZF_IDLE)


// AZ_PROFILE_SCOPE
#define AZ_PROFILE_SCOPE(category, name) \
    AZ_INTERNAL_PROF_TM_ZONE_VERIFY_CAT_NAME(category, TMZF_NONE, name)

#define AZ_PROFILE_SCOPE_STALL(category, name) \
    AZ_INTERNAL_PROF_TM_ZONE_VERIFY_CAT_NAME(category, TMZF_STALL, name)

#define AZ_PROFILE_SCOPE_IDLE(category, name) \
    AZ_INTERNAL_PROF_TM_ZONE_VERIFY_CAT_NAME(category, TMZF_IDLE, name)

// AZ_PROFILE_SCOPE_DYNAMIC
// For profiling events with dynamic scope names
// Note: the first variable argument must be a const format string
// Usage: AZ_PROFILE_SCOPE_DYNAMIC(AZ::Debug::ProfileCategory, <printf style const format string>, format args...)
#define AZ_PROFILE_SCOPE_DYNAMIC(category, name, ...)  \
        AZ_INTERNAL_PROF_TM_ZONE_VERIFY_CAT(category, TMZF_NONE, name, __VA_ARGS__)

#define AZ_PROFILE_SCOPE_STALL_DYNAMIC(category, name, ...) \
        AZ_INTERNAL_PROF_TM_ZONE_VERIFY_CAT(category, TMZF_STALL, name, __VA_ARGS__)

#define AZ_PROFILE_SCOPE_IDLE_DYNAMIC(category, name, ...) \
        AZ_INTERNAL_PROF_TM_ZONE_VERIFY_CAT(category, TMZF_IDLE, name, __VA_ARGS__)


// AZ_PROFILE_EVENT_BEGIN/END
// For profiling events that do not start and stop in the same scope (they MUST start/stop on the same thread)
// ALWAYS favor using scoped events (AZ_PROFILE_FUNCTION, AZ_PROFILE_SCOPE) as debugging an unmatched begin/end can be challenging
#define AZ_PROFILE_EVENT_BEGIN(category, name) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); 
    //tmEnter(AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category), TMZF_NONE, name)

#define AZ_PROFILE_EVENT_END(category) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); 
    //tmLeave(AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category))


// AZ_PROFILE_INTERVAL (mapped to Telemetry Timespan APIs)
// Note: using C-style casting as we allow either pointers or integral types as IDs
#define AZ_PROFILE_INTERVAL_START(category, id, ...) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); \
    AZ_INTERNAL_PROF_VERIFY_INTERVAL_ID(id);
    //tmBeginTimeSpan(AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category), (uint64_t)(id), TMZF_NONE, __VA_ARGS__)

#define AZ_PROFILE_INTERVAL_END(category, id) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); \
    AZ_INTERNAL_PROF_VERIFY_INTERVAL_ID(id);
    //tmEndTimeSpan(AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category), (uint64_t)(id))

// AZ_PROFILE_INTERVAL_SCOPED
// Scoped interval event that implicitly starts and ends in the same scope
// Note: using C-style casting as we allow either pointers or integral types as IDs
// Note: the first variable argument must be a const format string
// Usage: AZ_PROFILE_INTERVAL_SCOPED(AZ::Debug::ProfileCategory, <unique interval id>, <printf style const format string>, format args...)
#define AZ_PROFILE_INTERVAL_SCOPED(category, id,  ...) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); \
    AZ_INTERNAL_PROF_VERIFY_INTERVAL_ID(id);
    //tmTimeSpan(AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category), (uint64_t)(id), TM_MIN_TIME_SPAN_TRACK_ID + static_cast<AZ::Debug::ProfileCategoryPrimitiveType>(category), 0, TMZF_NONE, __VA_ARGS__)


// AZ_PROFILE_DATAPOINT (mapped to tmPlot APIs)
// Note: data points can have static or dynamic names, if using a dynamic name the first variable argument must be a const format string
// Usage: AZ_PROFILE_DATAPOINT(AZ::Debug::ProfileCategory, <printf style const format string>, format args...)
#define AZ_PROFILE_DATAPOINT(category, value, name, ...) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); \
    { \
        TRACY_PROFILER_VA_ARGS_NAME(_plotName, 256, name, __VA_ARGS__) \
        TracyPlot(_plotName, static_cast<double>(value)) \
        TracyPlotConfig(_plotName, tracy::PlotFormatType::Number) \
    }
    //TracyPlot(AZ_PROFILE_CAT_TO_RAD_CAPFLAGS(category), TM_PLOT_UNITS_REAL, TM_PLOT_DRAW_LINE, static_cast<double>(value), __VA_ARGS__)

#define AZ_PROFILE_DATAPOINT_PERCENT(category, value, name, ...) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); \
    { \
        TRACY_PROFILER_VA_ARGS_NAME(_plotNamePercent, 256, name, __VA_ARGS__) \
        TracyPlot(_plotNamePercent, static_cast<double>(value)) \
        TracyPlotConfig(_plotNamePercent, tracy::PlotFormatType::Percentage) \
    }
    //TracyPlotConfig(_plotName, tracy::PlotFormatType::Percentage) \


// AZ_PROFILE_MEMORY_ALLOC
#define AZ_PROFILE_MEMORY_ALLOC(category, address, size, context) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); 
    //TracyAlloc(address, size)

#define AZ_PROFILE_MEMORY_ALLOC_EX(category, filename, lineNumber, address, size, context) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); 
    //TracyAlloc(address, size)
    //tmAllocEx(AZ_INTERNAL_PROF_MEMORY_CAT_TO_FLAGS(category), filename, lineNumber, address, size, context)

#define AZ_PROFILE_MEMORY_FREE(category, address) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); 
    //TracyFree(address)

#define AZ_PROFILE_MEMORY_FREE_EX(category, filename, lineNumber, address) \
    AZ_INTERNAL_PROF_VERIFY_CAT(category); 
    //TracyFree(address);
    //tmFreeEx(AZ_INTERNAL_PROF_MEMORY_CAT_TO_FLAGS(category), filename, lineNumber, address)

#endif
