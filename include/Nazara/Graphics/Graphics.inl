// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<RenderPipeline>& Graphics::GetBlitPipeline(bool transparent) const
	{
		return (transparent) ? m_blitPipelineTransparent : m_blitPipeline;
	}

	inline const std::shared_ptr<RenderPipelineLayout>& Graphics::GetBlitPipelineLayout() const
	{
		return m_blitPipelineLayout;
	}

	inline auto Graphics::GetDefaultMaterials() const -> const DefaultMaterials&
	{
		return m_defaultMaterials;
	}

	inline const std::shared_ptr<PipelinePassList>& Graphics::GetDefaultPipelinePasses() const
	{
		return m_defaultPipelinePasses;
	}

	inline auto Graphics::GetDefaultTextures() const -> const DefaultTextures&
	{
		return m_defaultTextures;
	}

	inline FramePipelinePassRegistry& Graphics::GetFramePipelinePassRegistry()
	{
		return m_pipelinePassRegistry;
	}

	inline const FramePipelinePassRegistry& Graphics::GetFramePipelinePassRegistry() const
	{
		return m_pipelinePassRegistry;
	}

	inline MaterialPassRegistry& Graphics::GetMaterialPassRegistry()
	{
		return m_materialPassRegistry;
	}

	inline const MaterialPassRegistry& Graphics::GetMaterialPassRegistry() const
	{
		return m_materialPassRegistry;
	}

	inline MaterialInstanceLoader& Graphics::GetMaterialInstanceLoader()
	{
		return m_materialInstanceLoader;
	}

	inline const MaterialInstanceLoader& Graphics::GetMaterialInstanceLoader() const
	{
		return m_materialInstanceLoader;
	}

	inline MaterialLoader& Graphics::GetMaterialLoader()
	{
		return m_materialLoader;
	}

	inline const MaterialLoader& Graphics::GetMaterialLoader() const
	{
		return m_materialLoader;
	}

	inline PipelinePassListLoader& Graphics::GetPipelinePassListLoader()
	{
		return m_pipelinePassListLoader;
	}

	inline const PipelinePassListLoader& Graphics::GetPipelinePassListLoader() const
	{
		return m_pipelinePassListLoader;
	}

	inline PixelFormat Graphics::GetPreferredDepthFormat() const
	{
		return m_preferredDepthFormat;
	}

	inline PixelFormat Graphics::GetPreferredDepthStencilFormat() const
	{
		return m_preferredDepthStencilFormat;
	}

	inline const std::shared_ptr<RenderDevice>& Graphics::GetRenderDevice() const
	{
		return m_renderDevice;
	}

	inline const RenderPassCache& Graphics::GetRenderPassCache() const
	{
		assert(m_renderPassCache);
		return *m_renderPassCache;
	}

	inline TextureSamplerCache& Graphics::GetSamplerCache()
	{
		return *m_samplerCache;
	}

	inline std::shared_ptr<nzsl::FilesystemModuleResolver>& Graphics::GetShaderModuleResolver()
	{
		return m_shaderModuleResolver;
	}

	inline const std::shared_ptr<nzsl::FilesystemModuleResolver>& Graphics::GetShaderModuleResolver() const
	{
		return m_shaderModuleResolver;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
