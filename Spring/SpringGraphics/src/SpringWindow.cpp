#include <Spring/SpringGraphics/SpringWindow.hpp>

namespace spring::graphics
{
	const char* SpringWindow::getName()
	{
		return m_name;
	}

	void SpringWindow::setName(const char* name)
	{
		free(m_name);
		m_name = (char*)malloc(sizeof(name) / sizeof(char));
		strcpy(m_name, name);
	}
	
	const char* SpringWindow::getTitle()
	{
		return m_title;
	}

	void SpringWindow::setTitle(const char* title)
	{
		size_t size = strlen(title);
		if (size <= SPRING_WINDOW_MAXTITLE_LEN)
		{
			memcpy(m_title, title, SPRING_WINDOW_MAXTITLE_LEN);
		}
		else
		{
			memcpy(m_title, title, SPRING_WINDOW_MAXTITLE_LEN-3);
			m_title[SPRING_WINDOW_MAXTITLE_LEN - 3] = '.';
			m_title[SPRING_WINDOW_MAXTITLE_LEN - 2] = '.';
			m_title[SPRING_WINDOW_MAXTITLE_LEN - 1] = '.';
		}
	}
}