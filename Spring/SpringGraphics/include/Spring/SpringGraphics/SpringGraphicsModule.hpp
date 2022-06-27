#include <Spring/SpringCore/SpringModule.hpp>

#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::graphics
{
    class SpringGraphicsApi;
    class SpringWindow;
    struct WindowDesc;

    class SpringGraphicsModule : public spring::core::SpringModule
    {
    public:
        SpringGraphicsModule(spring::core::SpringApplication* app);
        virtual ~SpringGraphicsModule() override;

        // Windowing
        SpringWindow* createWindow(WindowDesc desc);
        inline int32_t getWindowsCount() { return static_cast<uint32_t>(m_windows.size()); };
        inline bool anyWindow() { return m_windows.size()>0; };

    protected:
        Scope<SpringGraphicsApi> m_api;

        std::vector<Scope<SpringWindow>> m_windows;
    };
}