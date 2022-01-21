#pragma once

namespace spring::graphics
{
	class SpringWindow
	{
	public:
		virtual ~SpringWindow() = default;

		virtual bool create() = 0;
		virtual HWND getHandle() = 0;
		
		void setName(const char* name);
		const char* getName();

	protected:
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		char* m_name;
		bool m_fullscreen;
	};
}