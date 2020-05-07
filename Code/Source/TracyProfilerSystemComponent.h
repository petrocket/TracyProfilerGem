#pragma once

#include <AzCore/Component/Component.h>

#include <TracyProfiler/TracyProfilerBus.h>

namespace TracyProfiler
{
    class TracyProfilerSystemComponent
        : public AZ::Component
        , protected TracyProfilerRequestBus::Handler
    {
    public:
        AZ_COMPONENT(TracyProfilerSystemComponent, "{C1B37CCE-EB09-488F-8390-99BCF0053134}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        ////////////////////////////////////////////////////////////////////////
        // TracyProfilerRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
