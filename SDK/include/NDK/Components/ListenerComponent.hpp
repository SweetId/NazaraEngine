// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_COMPONENTS_LISTENERCOMPONENT_HPP
#define NDK_COMPONENTS_LISTENERCOMPONENT_HPP

#include <NDK/Component.hpp>

namespace Ndk
{
	class NDK_API ListenerComponent : public Component<ListenerComponent>
	{
		public:
			ListenerComponent();
			~ListenerComponent() = default;

			bool IsActive() const;

			bool Serialize(Nz::SerializationContext& context) const override;

			void SetActive(bool active = true);

			bool Unserialize(Nz::SerializationContext& context) override;

			static ComponentIndex componentIndex;

		private:
			bool m_isActive;
	};
}

#include <NDK/Components/ListenerComponent.inl>

#endif // NDK_COMPONENTS_LISTENERCOMPONENT_HPP
