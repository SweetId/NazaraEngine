// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	inline VkBuffer VulkanBuffer::GetBuffer() const
	{
		return m_buffer;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
