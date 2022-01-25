#pragma once

#define SPRING_WINDOW_MAXTITLE_LEN 40

namespace spring::graphics
{
	class SpringWindow
	{
	public:
		virtual ~SpringWindow() = default;

		virtual bool create() = 0;
		virtual HWND getHandle() = 0;
		
		void setTitle(const char* title);
		const char* getTitle();

		void setName(const char* name);
		const char* getName();

		inline uint32_t getWidth() { return m_width; };
		inline uint32_t getHeight() { return m_height; };

	protected:
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		char* m_name = "Window Name";
		char m_title[SPRING_WINDOW_MAXTITLE_LEN+1] = "Window Title";
		bool m_fullscreen;
	};
}