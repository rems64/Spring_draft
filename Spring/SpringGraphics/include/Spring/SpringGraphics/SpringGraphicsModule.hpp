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

        virtual void update() override;
        virtual bool canClose() override;

        // Windowing
        Ref<SpringWindow> createWindow(WindowDesc desc);
        inline int32_t getWindowsCount() { return static_cast<uint32_t>(m_windows.size()); };
        inline bool anyWindow() { return m_windows.size()>0; };
        inline SpringGraphicsApi* getApi() { return m_api.get(); };

    protected:
        Scope<SpringGraphicsApi> m_api;

        std::vector<Ref<SpringWindow>> m_windows;
    };
}