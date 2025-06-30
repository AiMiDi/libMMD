#include "libMMD/Base/Log.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>

class TestSink : public spdlog::sinks::sink
{
public:
	~TestSink() override = default;

	void log(const spdlog::details::log_msg& msg) override
	{
		m_buffer.emplace_back(msg.payload.data(), msg.payload.size());
	}

	void flush() override
	{
	}

	void set_pattern(const std::string& pattern) override
	{
		// Empty implementation for testing
	}

	void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override
	{
		// Empty implementation for testing
	}

	std::vector<std::string> m_buffer;
};

TEST(BaseTest, LogTest)
{
	auto logger = libmmd::Singleton<libmmd::Logger>::Get();
	auto testSink = logger->AddSink<TestSink>();

	EXPECT_NE(static_cast<TestSink*>(nullptr), testSink.get());

	LIBMMD_INFO("test1");
	LIBMMD_WARN("test2");
	LIBMMD_ERROR("test3");

	EXPECT_EQ(3, testSink->m_buffer.size());

	EXPECT_EQ(2, logger->GetLogger()->sinks().size());
	logger->RemoveSink(testSink.get());
	EXPECT_EQ(1, logger->GetLogger()->sinks().size());

	LIBMMD_INFO("test4");
	EXPECT_EQ(3, testSink->m_buffer.size());
}
