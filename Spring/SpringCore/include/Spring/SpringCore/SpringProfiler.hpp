#pragma once

namespace spring::core
{
	struct ProfileResult
	{
		std::string name;

		std::chrono::duration<double, std::micro> start;
		std::chrono::microseconds elapsedTime;
		std::thread::id threadID;
	};

	class Profiler
	{
	public:
		Profiler(const Profiler&) = delete;
		Profiler(Profiler&&) = delete;

		void beginProfiling(const std::string& name, const std::string& path = "session.json");

		void endProfiling();

		void writeProfile(const ProfileResult& result)
		{
			std::lock_guard lock(m_writingMutex);
			std::basic_stringstream<char> json;

			json << std::setprecision(3) << std::fixed;
			json << ",{";
			json << R"("cat":"function",)";
			json << "\"dur\":" << (result.elapsedTime.count()) << ',';
			json << R"("name":")" << result.name << "\",";
			json << R"("ph":"X",)";
			json << "\"pid\":0,";
			json << "\"tid\":" << result.threadID << ",";
			json << "\"ts\":" << result.start.count();
			json << "}";

			if (m_started)
			{
				m_output << json.str();
				m_output.flush();
			}
		}

		static Profiler& get()
		{
			static Profiler instance;
			return instance;
		}

	private:
		Profiler()
			: m_sessionsName("Default session name"), m_started(false)
		{
		}

		~Profiler()
		{
            if(m_started)
			    endProfiling();
		}

		std::mutex m_writingMutex;
		std::string m_sessionsName;
		bool m_started;
		std::ofstream m_output;
	};

	class InstrumentationTimer
	{
	public:
		explicit InstrumentationTimer(const char* name)
			: m_name(name), m_stopped(false)
		{
			m_startTimepoint = std::chrono::steady_clock::now();
		}

		~InstrumentationTimer()
		{
			if (!m_stopped)
				stop();
		}

		void stop()
		{
			const auto endTimepoint = std::chrono::steady_clock::now();
			const auto highResStart = std::chrono::duration<double, std::micro>{ m_startTimepoint.time_since_epoch() };
			const auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch();

			Profiler::get().writeProfile({ m_name, highResStart, elapsedTime, std::this_thread::get_id() });

			m_stopped = true;
		}
	private:
		const char* m_name;
		std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
		bool m_stopped;
	};

	namespace InstrumentorUtils {

		template <size_t N>
		struct ChangeResult
		{
			char Data[N];
		};

		template <size_t N, size_t K>
		constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}
	}
}