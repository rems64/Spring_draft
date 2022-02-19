#include <Spring/SpringCore/SpringModule.hpp>

#include "SpringGraphicsDevice.hpp"

namespace spring::graphics
{
    class SpringWindow;

    enum class SpringWindowTypes
    {
        None,
        Native
    };

    class SpringGraphicsModule : public spring::core::SpringModule
    {
    public:
        SpringGraphicsModule(spring::core::SpringApplication* app);
        virtual ~SpringGraphicsModule() override;

        SpringWindow* createWindow(const char* title, SpringWindowTypes type);
        bool createSwapChain(SpringWindow* window);
        Device* getDevice() { return m_device.get(); };

        void tmpSetupRender();
        void tmpFrame();

    protected:
        std::unique_ptr<Device> m_device;

        std::vector<std::shared_ptr<SpringWindow>> m_windows;

        // TMP
        CommandList cmd;
        Shader vs;
        Shader fs;
        PipelineState pipelineState;
        RenderPass renderPass;
    };
}