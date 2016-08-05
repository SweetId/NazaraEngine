// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Adds this model to a render queue, using user-specified transform matrix and render order
	*
	* This can be useful when drawing particles
	*
	* \param renderQueue Queue to be added
	* \param transformMatrix Transform matrix to be used for rendering the model
	* \param renderOrder Specify the renderqueue layer to be used
	*/
	inline void Model::AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix, unsigned int renderOrder)
	{
		InstanceData instanceData(transformMatrix);
		instanceData.renderOrder = renderOrder;
		return AddToRenderQueue(renderQueue, instanceData);
	}

	/*!
	* \brief Specify which submeshes should be renderered
	*
	* \param enabledSubmeshes A bitset specifying which submeshes should be drawn
	*/
	inline void Model::ShowSubmeshes(Bitset<> enabledSubmeshes)
	{
		m_enabledSubmeshes = std::move(enabledSubmeshes);
	}

	/*!
	* \brief Creates a new Model from the arguments
	* \return A reference to the newly created model
	*
	* \param args Arguments for the model
	*/
	template<typename... Args>
	ModelRef Model::New(Args&&... args)
	{
		std::unique_ptr<Model> object(new Model(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
