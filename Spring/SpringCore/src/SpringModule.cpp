#include <Spring/SpringCore/SpringModule.hpp>

namespace spring::core
{
	SpringModule::SpringModule(SpringApplication* app) : m_app(app), m_type(SpringModuleTypes::Core)
	{
	}

	SpringModuleTypes SpringModule::getType() const
	{
		return m_type;
	}
}