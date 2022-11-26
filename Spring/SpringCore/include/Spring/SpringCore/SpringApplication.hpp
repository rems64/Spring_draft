#pragma once

#include <Spring/SpringCore/SpringCore.hpp>
#include <Spring/SpringCore/SpringPlatform.hpp>
#include <Spring/SpringCore/SpringModule.hpp>

namespace spring::core
{
    struct SpringApplicationInfos
    {
        bool showConsole;
        spNativeInstance instance;
    };

    /**
     * \brief Main class, responsible of managing the lifetime of the app
     */
    class SpringApplication
    {
    public:
        explicit SpringApplication(SpringApplicationInfos infos);
        virtual ~SpringApplication() = default;

        template<typename T>
        Ref<T> registerModule();
        [[nodiscard]] SpringModule* getModule(SpringModuleTypes type) const;
        [[nodiscard]] spNativeInstance getNativeInstance() const;
        void shutdown();
        static SpringApplication* get();

        /**
         * \brief Application main loop, handles update for each module
         * \return 0 if no error occured
         */
        virtual int mainLoop();
        virtual int mainLoopCall() {return 0x0;};

    protected:
        static SpringApplication* m_app;
        spNativeInstance m_instance;
        std::vector<Ref<SpringModule>> m_modules;
        bool shutdowning = false;
    };

    template <typename T>
    Ref<T> SpringApplication::registerModule()
    {
	    Ref<T> module = makeRef<T>(this);
	    m_modules.push_back(module);
	    return module;
    }
}
