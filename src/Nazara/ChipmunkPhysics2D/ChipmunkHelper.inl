// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	inline Vector2f FromChipmunk(const cpVect& vect)
	{
		return { float(vect.x), float(vect.y) };
	}

	inline cpVect ToChipmunk(const Vector2f& vec)
	{
		return cpv(cpFloat(vec.x), cpFloat(vec.y));
	}
}

#include <Nazara/ChipmunkPhysics2D/DebugOff.hpp>
