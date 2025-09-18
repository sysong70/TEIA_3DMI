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
};

struct Result {
	Error code { Error::None };
	std::string message {};

	constexpr explicit operator bool() const noexcept {
		return code == Error::None;
	}

	static Result Ok() noexcept {
		return {};
	}

	static Result Fail(Error c, std::string_view msg) {
		return Result { c, std::string(msg) };
	}
};