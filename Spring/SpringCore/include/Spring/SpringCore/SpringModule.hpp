#pragma once

namespace spring::core
{
    class SpringApplication;
    enum class SpringModuleTypes
    {
        Core,
        Graphics,
        Input,
        Physics,
        Scripting,
        Audio,
        Network,
        Editor,
        Game,
        Count
    };

    class SpringModule
    {
    public:
        explicit SpringModule(SpringApplication* app);
        virtual ~SpringModule() = default;

        virtual void update(bool closeRequired) = 0;
        virtual bool canClose() = 0;

        [[nodiscard]] SpringModuleTypes getType() const;
    
    protected:
        SpringModuleTypes m_type;
        SpringApplication* m_app;
        bool shouldClose = false;
    };
}