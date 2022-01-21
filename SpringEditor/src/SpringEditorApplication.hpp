#pragma once

#include <Spring/SpringCore/SpringCore.hpp>

class SpringEditorApplication : public spring::core::SpringApplication
{
public:
    SpringEditorApplication(HINSTANCE instance);
    virtual ~SpringEditorApplication() override;
};