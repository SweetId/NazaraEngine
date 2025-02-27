// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	template<typename T>
	T* JoltConstraint3D::GetConstraint()
	{
		return SafeCast<T*>(m_constraint.get());
	}

	template<typename T>
	const T* JoltConstraint3D::GetConstraint() const
	{
		return SafeCast<const T*>(m_constraint.get());
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
