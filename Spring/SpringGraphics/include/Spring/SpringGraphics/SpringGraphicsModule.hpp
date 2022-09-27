#pragma once
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
	    explicit SpringGraphicsModule(spring::core::SpringApplication* app);
        ~SpringGraphicsModule() override;

        void update(bool closeRequired) override;
        bool canClose() override;

        // Windowing
        Ref<SpringWindow> createWindow(WindowDesc desc);
        [[nodiscard]] uint32_t getWindowsCount() const;
        [[nodiscard]] bool anyWindow() const;
        [[nodiscard]] SpringGraphicsApi* getApi() const;

    protected:
        Scope<SpringGraphicsApi> m_api;

        std::vector<Ref<SpringWindow>> m_windows;
    };
}