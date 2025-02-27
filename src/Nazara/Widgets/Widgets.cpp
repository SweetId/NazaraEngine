// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup widgets
	* \class Nz::Widgets
	* \brief Widgets class that represents the module initializer of Widgets
	*/
	Widgets::Widgets(Config /*config*/) :
	ModuleBase("Widgets", this)
	{
		CreateDefaultMaterials();
	}

	void Widgets::CreateDefaultMaterials()
	{
		m_opaqueMaterial = MaterialInstance::Instantiate(MaterialType::Basic);
		m_opaqueMaterial->UpdatePassesStates([](RenderStates& renderStates)
		{
			renderStates.scissorTest = true;
			return true;
		});

		m_transparentMaterial = MaterialInstance::Instantiate(MaterialType::Basic, MaterialInstancePreset::Transparent);
		m_transparentMaterial->UpdatePassesStates([](RenderStates& renderStates)
		{
			renderStates.scissorTest = true;
			return true;
		});
	}

	Widgets* Widgets::s_instance = nullptr;
}
