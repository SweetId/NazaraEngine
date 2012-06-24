// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ICON_HPP
#define NAZARA_ICON_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>

class NzImage;
class NzIconImpl;
class NzWindowImpl;

class NAZARA_API NzIcon
{
	friend class NzWindowImpl;

	public:
		NzIcon();
		~NzIcon();

		bool Create(const NzImage& icon);
		void Destroy();

		bool IsValid() const;

	private:
		NzIconImpl* m_impl;
};

#endif // NAZARA_ICON_HPP
