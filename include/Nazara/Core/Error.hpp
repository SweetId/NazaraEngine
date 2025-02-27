// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ERROR_HPP
#define NAZARA_CORE_ERROR_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Format.hpp>
#include <string>

#if NAZARA_CORE_ENABLE_ASSERTS || defined(NAZARA_DEBUG)
	#define NazaraAssert(a, err) if NAZARA_UNLIKELY(!(a)) Nz::Error::Trigger(Nz::ErrorType::AssertFailed, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, err)
	#define NazaraAssertFmt(a, fmt, ...) if NAZARA_UNLIKELY(!(a)) Nz::Error::Trigger(Nz::ErrorType::AssertFailed, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, Nz::Format(NAZARA_FORMAT(fmt), __VA_ARGS__))
#else
	#define NazaraAssert(a, err) for (;;) break
	#define NazaraAssertFmt(a, fmt, ...) for (;;) break
#endif

#define NazaraError(err)                 Nz::Error::Trigger(Nz::ErrorType::Normal,   __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, err)
#define NazaraErrorFmt(fmt, ...)         Nz::Error::Trigger(Nz::ErrorType::Normal,   __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, Nz::Format(NAZARA_FORMAT(fmt), __VA_ARGS__))
#define NazaraInternalError(err)         Nz::Error::Trigger(Nz::ErrorType::Internal, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, err)
#define NazaraInternalErrorFmt(fmt, ...) Nz::Error::Trigger(Nz::ErrorType::Internal, __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, Nz::Format(NAZARA_FORMAT(fmt), __VA_ARGS__))
#define NazaraWarning(err)               Nz::Error::Trigger(Nz::ErrorType::Warning,  __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, err)
#define NazaraWarningFmt(fmt, ...)       Nz::Error::Trigger(Nz::ErrorType::Warning,  __LINE__, __FILE__, NAZARA_PRETTY_FUNCTION, Nz::Format(NAZARA_FORMAT(fmt), __VA_ARGS__))

namespace Nz
{
	class NAZARA_CORE_API Error
	{
		public:
			Error() = delete;
			~Error() = delete;

			static ErrorModeFlags ApplyFlags(ErrorModeFlags orFlags, ErrorModeFlags andFlags);

			static constexpr std::string_view GetCurrentFileRelativeToEngine(std::string_view file);
			static ErrorModeFlags GetFlags();
			static std::string GetLastError(std::string_view* file = nullptr, unsigned int* line = nullptr, std::string_view* function = nullptr);
			static unsigned int GetLastSystemErrorCode();
			static std::string GetLastSystemError(unsigned int code = GetLastSystemErrorCode());

			static ErrorModeFlags SetFlags(ErrorModeFlags flags);

			static inline void Trigger(ErrorType type, std::string error);
			static inline void Trigger(ErrorType type, unsigned int line, std::string_view file, std::string_view function, std::string error);

		private:
			static void TriggerInternal(ErrorType type, std::string error, unsigned int line, std::string_view file, std::string_view function);
	};
}

#include <Nazara/Core/Error.inl>

#endif // NAZARA_CORE_ERROR_HPP
