// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_HASH_SHA384_HPP
#define NAZARA_CORE_HASH_SHA384_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>

namespace Nz
{
	union SHA_CTX;

	class NAZARA_CORE_API SHA384Hasher final : public AbstractHash
	{
		public:
			SHA384Hasher();
			~SHA384Hasher();

			void Append(const UInt8* data, std::size_t len) override;
			void Begin() override;
			ByteArray End() override;

			std::size_t GetDigestLength() const override;
			const char* GetHashName() const override;

		private:
			SHA_CTX* m_state;
	};
}

#endif // NAZARA_CORE_HASH_SHA384_HPP
