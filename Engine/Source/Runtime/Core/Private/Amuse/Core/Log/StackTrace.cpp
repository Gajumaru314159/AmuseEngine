//***********************************************************
//! @file
//***********************************************************
#include <Amuse/Core/Log/StackTrace.h>
#include <Amuse/Core/CoreTypes.h>
#include <Amuse/Core/String/StringEncoder.h>

namespace Amuse::Core {

	StackTrace StackTrace::Capture(s32 frameToSkip) {
		StackTrace result;
#ifdef AMUSE_DEBUG
		auto stacktraces = std::stacktrace::current();
		for (s32 i=frameToSkip;i<stacktraces.size();++i)
		{
			result.entries.push_back(stacktraces[i]);
			if (result.entries.size() == result.entries.capacity()) break;
		}
#endif
		return result;
	}

	auto StackTrace::elements()const->FixedVector<StackTraceElement,MAX_ENTRY_COUNT> {

		FixedVector<StackTraceElement,MAX_ENTRY_COUNT> result;

#ifdef AMUSE_DEBUG
		for (auto& entry: entries)
		{
			auto& element = result.emplace_back();
			element.filename = entry.source_file();
			element.line = entry.source_line();
			element.name = entry.description();
		}
#endif

		return result;
	}

}
