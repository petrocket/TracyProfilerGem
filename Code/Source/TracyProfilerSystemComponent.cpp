
#include <AzCore/Debug/ProfilerBus.h>
#include <AzCore/PlatformIncl.h>
#include <TracyProfilerSystemComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>

#include <tracy/Tracy.hpp>

namespace TracyProfiler
{
    static const char * ProfileChannel = "TracyProfiler";
    static const AZ::u32 MaxProfileThreadCount = 128;

    //static void MessageFrameTickType(AZ::Debug::ProfileFrameAdvanceType type)
    //{
    //    if (type == AZ::Debug::ProfileFrameAdvanceType::Game)
    //    {
    //        AZ_Printf(ProfileChannel, "Profile tick set to Game");
    //        TracyMessageL("Profile tick set to Game");
    //    }
    //    else
    //    {
    //        AZ_Printf(ProfileChannel, "Profile tick set to Render Frame");
    //        TracyMessageL("Profile tick set to Render Frame");
    //    }
    //}

    TracyProfilerSystemComponent::TracyProfilerSystemComponent()
    {
        AZStd::ThreadEventBus::Handler::BusConnect();
    }

    TracyProfilerSystemComponent::~TracyProfilerSystemComponent()
    {
        AZStd::ThreadEventBus::Handler::BusDisconnect();
    }

    void TracyProfilerSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<TracyProfilerSystemComponent, AZ::Component>()
                ->Version(0);

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<TracyProfilerSystemComponent>("TracyProfiler", "Tracy Profiler System Component that detects frame tick events and thread names")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void TracyProfilerSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("TracyProfilerService"));
    }

    void TracyProfilerSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("TracyProfilerService"));
    }

    void TracyProfilerSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        AZ_UNUSED(required);
    }

    void TracyProfilerSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        AZ_UNUSED(dependent);
    }

    void TracyProfilerSystemComponent::Init()
    {
    }

    void TracyProfilerSystemComponent::Activate()
    {
        AZ::Debug::ProfilerRequestBus::Handler::BusConnect();
        AZ::SystemTickBus::Handler::BusConnect();
    }

    void TracyProfilerSystemComponent::Deactivate()
    {
        AZ::SystemTickBus::Handler::BusDisconnect();
        AZ::Debug::ProfilerRequestBus::Handler::BusDisconnect();
    }

    void TracyProfilerSystemComponent::OnThreadEnter(const AZStd::thread_id& id, const AZStd::thread_desc* desc)
    {
        (void)id;
        (void)desc;
#if AZ_TRAIT_OS_USE_WINDOWS_THREADS
        if (!desc)
        {
            // Skip unnamed threads
            return;
        }

        //if (IsInitialized())
        {
            // We can send the thread name to Telemetry now
            const AZ::u32 newProfiledThreadCount = ++m_profiledThreadCount;
            AZ_Assert(newProfiledThreadCount <= MaxProfileThreadCount, "Tracy Profiled threadcount exceeded MaxProfileThreadCount!");
            tracy::SetThreadName(desc->m_name);
            return;
        }

        // Save off to send on the next connection
        ScopedLock lock(m_threadNameLock);

        auto end = m_threadNames.end();
        auto itr = AZStd::find_if(m_threadNames.begin(), end, [id](const ThreadNameEntry& entry)
        {
            return entry.id == id;
        });

        if (itr != end)
        {
            itr->name = desc->m_name;
        }
        else
        {
            m_threadNames.push_back({ id, desc->m_name });
        }
#else
        const AZ::u32 newProfiledThreadCount = ++m_profiledThreadCount;
        AZ_Assert(newProfiledThreadCount <= MaxProfileThreadCount, "RAD Telemetry profiled threadcount exceeded MaxProfileThreadCount!");
#endif
    }

    void TracyProfilerSystemComponent::OnThreadExit(const AZStd::thread_id& id)
    {
        (void)id;
#if AZ_TRAIT_OS_USE_WINDOWS_THREADS
        {
            ScopedLock lock(m_threadNameLock);

            auto end = m_threadNames.end();
            auto itr = AZStd::find_if(m_threadNames.begin(), end, [id](const ThreadNameEntry& entry)
            {
                return entry.id == id;
            });
            if (itr != end)
            {
                m_threadNames.erase(itr);
            }
            else
            {
                --m_profiledThreadCount;
            }
        }
#else
        --m_profiledThreadCount;
#endif
    }

    void TracyProfilerSystemComponent::OnSystemTick()
    {
        FrameAdvance(AZ::Debug::ProfileFrameAdvanceType::Game);
    }

    void TracyProfilerSystemComponent::FrameAdvance(AZ::Debug::ProfileFrameAdvanceType type)
    {
        if (type == m_frameAdvanceType)
        {
            FrameMark
        }
    }

    bool TracyProfilerSystemComponent::IsActive()
    {
        return m_running;
    }
}
