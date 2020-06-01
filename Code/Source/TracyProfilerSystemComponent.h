#pragma once

#include <AzCore/std/parallel/threadbus.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/string/string.h>

namespace TracyProfiler
{
    class TracyProfilerSystemComponent
        : public AZ::Component
        , private AZStd::ThreadEventBus::Handler
        , private AZ::SystemTickBus::Handler
        , private AZ::Debug::ProfilerRequestBus::Handler
    {
    public:
        AZ_COMPONENT(TracyProfilerSystemComponent, "{C1B37CCE-EB09-488F-8390-99BCF0053134}");

        TracyProfilerSystemComponent();
        ~TracyProfilerSystemComponent() override;

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;

    private:
        //////////////////////////////////////////////////////////////////////////
        // Thread event bus
        void OnThreadEnter(const AZStd::thread_id& id, const AZStd::thread_desc* desc) override;
        void OnThreadExit(const AZStd::thread_id& id) override;

        //////////////////////////////////////////////////////////////////////////
        // SystemTickBus
        void OnSystemTick() override;

        //////////////////////////////////////////////////////////////////////////
        // ProfilerRequstBus
        bool IsActive() override;
        void FrameAdvance(AZ::Debug::ProfileFrameAdvanceType type) override;

        //////////////////////////////////////////////////////////////////////////
        // Data members
        struct ThreadNameEntry
        {
            AZStd::thread_id id;
            AZStd::string name;
        };
        AZStd::vector<ThreadNameEntry> m_threadNames;
        using LockType = AZStd::mutex;
        using ScopedLock = AZStd::lock_guard<LockType>;
        LockType m_threadNameLock;
        AZStd::atomic_uint m_profiledThreadCount = { 0 };

        AZ::Debug::ProfileFrameAdvanceType m_frameAdvanceType = AZ::Debug::ProfileFrameAdvanceType::Game;
        bool m_running = true; // currently not using on-demand mode 
    };
}
