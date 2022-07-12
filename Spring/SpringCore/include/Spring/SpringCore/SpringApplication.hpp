#pragma once

#include <Spring/SpringCore/SpringModule.hpp>
#include <Spring/SpringCore/SpringCommon.hpp>

namespace spring::core
{
    struct SpringApplicationInfos
    {
        bool showConsole;
        HINSTANCE instance;
    };

    /**
     * \brief Main class, responsible of managing the lifetime of the app
     */
    class SpringApplication
    {
    public:
        SpringApplication(SpringApplicationInfos infos);
        virtual ~SpringApplication() = default;

        template<typename T>
        Ref<T> registerModule();
        [[nodiscard]] SpringModule* getModule(SpringModuleTypes type) const;
        [[nodiscard]] HINSTANCE getNativeInstance() const;
        static SpringApplication* get();

        /**
         * \brief Application main loop, handles update for each module
         * \return 0 if no error occured
         */
        virtual int mainLoop();

    protected:
        static SpringApplication* m_app;
        HINSTANCE m_instance;
        HWND m_console;
        std::vector<Ref<SpringModule>> m_modules;
    };

    template <typename T>
    Ref<T> SpringApplication::registerModule()
    {
	    Ref<T> module = makeRef<T>(this);
	    m_modules.push_back(module);
	    return module;
    }
}
