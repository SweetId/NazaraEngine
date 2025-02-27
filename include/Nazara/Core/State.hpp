// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_STATE_HPP
#define NAZARA_CORE_STATE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Time.hpp>

namespace Nz
{
	class StateMachine;

	class NAZARA_CORE_API State
	{
		public:
			State() = default;
			virtual ~State();

			virtual void Enter(StateMachine& fsm) = 0;
			virtual void Leave(StateMachine& fsm) = 0;

			virtual bool Update(StateMachine& fsm, Time elapsedTime) = 0;
	};
}

#endif // NAZARA_CORE_STATE_HPP
