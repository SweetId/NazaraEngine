// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Clipboard.hpp>

#ifdef NAZARA_PLATFORM_ANDROID
#include <Nazara/Platform/Android/InputImpl.hpp>
#else
#include <Nazara/Platform/SDL2/InputImpl.hpp>
#endif

#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	ClipboardContentType Clipboard::GetContentType()
	{
		return InputImpl::GetClipboardContentType();
	}

	std::string Clipboard::GetString()
	{
		return InputImpl::GetClipboardString();
	}

	void Clipboard::SetString(const std::string& str)
	{
		return InputImpl::SetClipboardString(str);
	}
}
