#include <Spring/SpringCore/SpringModule.hpp>

#include "SpringGraphicsDevice.hpp"

namespace spring::graphics
{
    class SpringGraphicsModule : public spring::core::SpringModule
    {
    public:
        SpringGraphicsModule(spring::core::SpringApplication* app);
        virtual ~SpringGraphicsModule() override;

        bool setWindow(HWND handle);

    protected:
        std::unique_ptr<spring::graphics::Device> m_device;
        spring::graphics::SwapChain swapchain;
    };
}