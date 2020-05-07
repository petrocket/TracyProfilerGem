
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <TracyProfilerSystemComponent.h>

namespace TracyProfiler
{
    class TracyProfilerModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(TracyProfilerModule, "{7D45AE75-F1D1-4908-A0C9-BFB4BD3D3371}", AZ::Module);
        AZ_CLASS_ALLOCATOR(TracyProfilerModule, AZ::SystemAllocator, 0);

        TracyProfilerModule()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                TracyProfilerSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<TracyProfilerSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(TracyProfiler_7735098263844089a66b8c07ea21d95c, TracyProfiler::TracyProfilerModule)
