#include <Spring/SpringGraphics/SpringWindow.hpp>

namespace spring::graphics
{
	const char* SpringWindow::getName()
	{
		return m_name;
	}

	void SpringWindow::setName(const char* name)
	{
		delete m_name;
		m_name = (char*)malloc(sizeof(name) / sizeof(char));
		strcpy(m_name, name);
	}
}