#pragma once

#include "SpringCore.hpp"

namespace spring::core
{
    struct Resource {
        Ref<void> data = nullptr;
    };

    class ResourceManager
    {
        ResourceManager();
    };
}