// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_DARWIN_TIMEIMPL_HPP
#define NAZARA_CORE_DARWIN_TIMEIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Time.hpp>

namespace Nz
{
	bool InitializeHighPrecisionTimer();
	Time GetElapsedNanosecondsImpl();
	Time GetElapsedMillisecondsImpl();
}

#endif // NAZARA_CORE_DARWIN_TIMEIMPL_HPP
