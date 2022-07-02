#include <Spring/SpringCore/SpringProfiler.hpp>

namespace spring::core
{
	void Profiler::beginProfiling(const std::string& name, const std::string& path)
	{
		std::lock_guard locker(m_writingMutex);
		if (m_started)
		{
			std::cout << "[ERROR] Profiling has already started!" << std::endl;
			return;
		}
		m_output.open(path);
		if (m_output.is_open())
		{
			m_started = true;
			m_output << "{\"otherData\": {},\"traceEvents\":[{}";
			m_output.flush();
		}
		else
		{
			std::cout << "[ERROR] Failed to open profiling file" << std::endl;
		}
	};

	void Profiler::endProfiling()
	{
		std::lock_guard locker(m_writingMutex);
		if (m_started)
		{
			m_output << "]}";
			m_output.flush();

			m_output.close();
			m_started = false;
		}
	};


}