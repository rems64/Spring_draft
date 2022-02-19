#pragma once

#include <Spring/SpringCore/SpringCore.hpp>

class SpringEditorApplication : public spring::core::SpringApplication
{
public:
    SpringEditorApplication(spring::core::SpringApplicationInfos info);
    virtual ~SpringEditorApplication() override;
};