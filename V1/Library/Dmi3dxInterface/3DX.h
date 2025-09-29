#pragma once

#ifdef H3DX_EXPORTS
#	define API_3DX __declspec (dllexport)
#else
#	define API_3DX __declspec (dllimport)
#endif

enum class Error {
	None = 0,
	InvalidArgument,
	IoFailure,
	StateError,
	NotInitialized,
	Internal,
	WriteFailed,
	AnalysisFailed
};

struct Result {
	Error code { Error::None };
	std::string message {};

	friend bool operator ==(const Result & lhs, const Result & rhs) noexcept {
		return lhs.code == rhs.code;
	}

	friend bool operator !=(const Result & lhs, const Result & rhs) noexcept {
		return lhs.code != rhs.code;
	}

	static Result Ok() noexcept {
		return {};
	}

	static Result Fail(Error c, std::string_view msg) {
		return Result { c, std::string(msg) };
	}
};