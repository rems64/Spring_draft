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
        SpringModule(SpringApplication* app);
        virtual ~SpringModule() = default;

        SpringModuleTypes getType() const;
    
    protected:
        SpringModuleTypes m_type;
        SpringApplication* m_app;
    };
}