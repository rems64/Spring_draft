#include <Spring/SpringCore/SpringModule.hpp>

namespace spring::core
{
	SpringModule::SpringModule(SpringApplication* app) : m_type(SpringModuleTypes::Core), m_app(app)
	{
	}

	SpringModuleTypes SpringModule::getType() const
	{
		return m_type;
	}
}