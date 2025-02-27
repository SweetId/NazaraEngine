// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	ApplicationComponent::~ApplicationComponent() = default;

	void ApplicationComponent::Update(Time /*elapsedTime*/)
	{
	}
}
