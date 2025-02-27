// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Core/AppFilesystemComponent.hpp>
#include <Nazara/Core/CommandLineParameters.hpp>
#include <Nazara/Graphics/DebugDrawPipelinePass.hpp>
#include <Nazara/Graphics/DepthPipelinePass.hpp>
#include <Nazara/Graphics/ForwardPipelinePass.hpp>
#include <Nazara/Graphics/GuillotineTextureAtlas.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Graphics/PipelinePassList.hpp>
#include <Nazara/Graphics/PostProcessPipelinePass.hpp>
#include <Nazara/Graphics/PredefinedMaterials.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/Formats/PipelinePassListLoader.hpp>
#include <Nazara/Graphics/Formats/TextureLoader.hpp>
#include <Nazara/Utility/Font.hpp>
#include <NZSL/Ast/AstSerializer.hpp>
#include <NZSL/Ast/Module.hpp>
#include <array>
#include <stdexcept>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_textureBlitShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/TextureBlit.nzslb.h>
		};

		const UInt8 r_basicMaterialShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/BasicMaterial.nzslb.h>
		};

		const UInt8 r_fullscreenVertexShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/FullscreenVertex.nzslb.h>
		};

		const UInt8 r_phongMaterialShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/PhongMaterial.nzslb.h>
		};

		const UInt8 r_physicallyBasedMaterialShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/PhysicallyBasedMaterial.nzslb.h>
		};

		// Modules
		const UInt8 r_instanceDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/InstanceData.nzslb.h>
		};

		const UInt8 r_lightDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/LightData.nzslb.h>
		};

		const UInt8 r_lightShadowModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/LightShadow.nzslb.h>
		};

		const UInt8 r_skeletalDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/SkeletalData.nzslb.h>
		};

		const UInt8 r_skinningDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/SkinningData.nzslb.h>
		};

		const UInt8 r_skinningLinearModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/SkinningLinear.nzslb.h>
		};

		const UInt8 r_viewerDataModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Engine/ViewerData.nzslb.h>
		};

		const UInt8 r_mathColorModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Math/Color.nzslb.h>
		};

		const UInt8 r_mathConstantsModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Math/Constants.nzslb.h>
		};

		const UInt8 r_mathCookTorrancePBRModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Math/CookTorrancePBR.nzslb.h>
		};

		const UInt8 r_mathDepthModule[] = {
			#include <Nazara/Graphics/Resources/Shaders/Modules/Math/Depth.nzslb.h>
		};

		// Passes
		const UInt8 r_gammaCorrectionPass[] = {
			#include <Nazara/Graphics/Resources/Shaders/Passes/GammaCorrection.nzslb.h>
		};
	}

	/*!
	* \ingroup graphics
	* \class Graphics
	* \brief Graphics class that represents the module initializer of Graphics
	*/
	Graphics::Graphics(Config config) :
	ModuleBase("Graphics", this),
	m_preferredDepthFormat(PixelFormat::Undefined),
	m_preferredDepthStencilFormat(PixelFormat::Undefined)
	{
		Renderer* renderer = Renderer::Instance();

		const std::vector<RenderDeviceInfo>& renderDeviceInfo = renderer->QueryRenderDevices();
		if (renderDeviceInfo.empty())
			throw std::runtime_error("no render device available");

		std::size_t bestRenderDeviceIndex = 0;
		for (std::size_t i = 0; i < renderDeviceInfo.size(); ++i)
		{
			const auto& deviceInfo = renderDeviceInfo[i];
			if (config.useDedicatedRenderDevice && deviceInfo.type == RenderDeviceType::Dedicated)
			{
				bestRenderDeviceIndex = i;
				break;
			}
			else if (!config.useDedicatedRenderDevice && deviceInfo.type == RenderDeviceType::Integrated)
			{
				bestRenderDeviceIndex = i;
				break;
			}
		}

		RenderDeviceFeatures enabledFeatures;
		enabledFeatures.anisotropicFiltering = !config.forceDisableFeatures.anisotropicFiltering && renderDeviceInfo[bestRenderDeviceIndex].features.anisotropicFiltering;
		enabledFeatures.computeShaders = !config.forceDisableFeatures.computeShaders && renderDeviceInfo[bestRenderDeviceIndex].features.computeShaders;
		enabledFeatures.depthClamping = !config.forceDisableFeatures.depthClamping && renderDeviceInfo[bestRenderDeviceIndex].features.depthClamping;
		enabledFeatures.nonSolidFaceFilling = !config.forceDisableFeatures.nonSolidFaceFilling && renderDeviceInfo[bestRenderDeviceIndex].features.nonSolidFaceFilling;
		enabledFeatures.storageBuffers = !config.forceDisableFeatures.storageBuffers && renderDeviceInfo[bestRenderDeviceIndex].features.storageBuffers;
		enabledFeatures.textureReadWithoutFormat = !config.forceDisableFeatures.textureReadWithoutFormat && renderDeviceInfo[bestRenderDeviceIndex].features.textureReadWithoutFormat;
		enabledFeatures.textureReadWrite = !config.forceDisableFeatures.textureReadWrite && renderDeviceInfo[bestRenderDeviceIndex].features.textureReadWrite;
		enabledFeatures.textureWriteWithoutFormat = !config.forceDisableFeatures.textureWriteWithoutFormat && renderDeviceInfo[bestRenderDeviceIndex].features.textureWriteWithoutFormat;
		enabledFeatures.unrestrictedTextureViews = !config.forceDisableFeatures.unrestrictedTextureViews && renderDeviceInfo[bestRenderDeviceIndex].features.unrestrictedTextureViews;

		m_renderDevice = renderer->InstanciateRenderDevice(bestRenderDeviceIndex, enabledFeatures);
		if (!m_renderDevice)
			throw std::runtime_error("failed to instantiate render device");

		m_renderPassCache.emplace(*m_renderDevice);
		m_samplerCache.emplace(m_renderDevice);

		SelectDepthStencilFormats();

		BuildDefaultTextures();
		RegisterShaderModules();
		BuildBlitPipeline();
		RegisterMaterialPasses();

		MaterialPipeline::Initialize();
		BuildDefaultMaterials();
		RegisterPipelinePasses();
		BuildDefaultPipelinePasses();

		Font::SetDefaultAtlas(std::make_shared<GuillotineTextureAtlas>(*m_renderDevice));

		m_materialInstanceLoader.RegisterLoader(Loaders::GetMaterialInstanceLoader_Texture()); // texture to material loader
		m_pipelinePassListLoader.RegisterLoader(Loaders::GetPipelinePassListLoader()); // texture to material loader
	}

	Graphics::~Graphics()
	{
		// Free of atlas if it is ours
		std::shared_ptr<AbstractAtlas> defaultAtlas = Font::GetDefaultAtlas();
		if (defaultAtlas && defaultAtlas->GetStorage() == DataStorage::Hardware)
		{
			Font::SetDefaultAtlas(nullptr);

			// The default police can make live one hardware atlas after the free of a module (which could be problematic)
			// So, if the default police use a hardware atlas, we stole it.
			// I don't like this solution, but I don't have any better
			if (defaultAtlas.use_count() > 1)
			{
				// At least one police use the atlas
				const std::shared_ptr<Font>& defaultFont = Font::GetDefault();
				defaultFont->SetAtlas(nullptr);

				if (defaultAtlas.use_count() > 1)
				{
					// Still not the only one to own it ? Then crap.
					NazaraWarning("Default font atlas uses hardware storage and is still used");
				}
			}
		}

		defaultAtlas.reset();

		MaterialPipeline::Uninitialize();
		m_renderPassCache.reset();
		m_samplerCache.reset();
		m_blitPipeline.reset();
		m_blitPipelineLayout.reset();
		m_defaultMaterials = DefaultMaterials{};
		m_defaultTextures = DefaultTextures{};
	}

	void Graphics::RegisterComponent(AppFilesystemComponent& component)
	{
		TextureParams defaultTexParams;
		defaultTexParams.renderDevice = m_renderDevice;

		component.SetDefaultResourceParameters<Texture>(defaultTexParams);
	}

	void Graphics::BuildBlitPipeline()
	{
		RenderPipelineLayoutInfo layoutInfo;
		layoutInfo.bindings.assign({
			{
				0, 0, 1,
				ShaderBindingType::Sampler,
				nzsl::ShaderStageType::Fragment
			}
		});

		m_blitPipelineLayout = m_renderDevice->InstantiateRenderPipelineLayout(std::move(layoutInfo));
		if (!m_blitPipelineLayout)
			throw std::runtime_error("failed to instantiate fullscreen renderpipeline layout");

		nzsl::Ast::ModulePtr blitShaderModule = m_shaderModuleResolver->Resolve("TextureBlit");

		nzsl::ShaderWriter::States states;
		states.shaderModuleResolver = m_shaderModuleResolver;

		auto blitShader = m_renderDevice->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, *blitShaderModule, states);
		if (!blitShader)
			throw std::runtime_error("failed to instantiate blit shader");

		RenderPipelineInfo pipelineInfo;

		pipelineInfo.pipelineLayout = m_blitPipelineLayout;
		pipelineInfo.shaderModules.push_back(std::move(blitShader));

		m_blitPipeline = m_renderDevice->InstantiateRenderPipeline(pipelineInfo);

		// Blending
		pipelineInfo.blending = true;
		pipelineInfo.blend.modeColor = BlendEquation::Add;
		pipelineInfo.blend.modeAlpha = BlendEquation::Add;
		pipelineInfo.blend.srcColor = BlendFunc::SrcAlpha;
		pipelineInfo.blend.dstColor = BlendFunc::InvSrcAlpha;
		pipelineInfo.blend.srcAlpha = BlendFunc::SrcAlpha;
		pipelineInfo.blend.dstAlpha = BlendFunc::InvSrcAlpha;

		m_blitPipelineTransparent = m_renderDevice->InstantiateRenderPipeline(std::move(pipelineInfo));
	}

	void Graphics::BuildDefaultMaterials()
	{
		std::size_t depthPassIndex = m_materialPassRegistry.GetPassIndex("DepthPass");
		std::size_t shadowPassIndex = m_materialPassRegistry.GetPassIndex("ShadowPass");
		std::size_t distanceShadowPassIndex = m_materialPassRegistry.GetPassIndex("DistanceShadowPass");
		std::size_t forwardPassIndex = m_materialPassRegistry.GetPassIndex("ForwardPass");

		const auto& enabledFeatures = m_renderDevice->GetEnabledFeatures();

		// BasicMaterial
		{
			MaterialSettings settings;
			PredefinedMaterials::AddBasicSettings(settings);

			MaterialPass forwardPass;
			forwardPass.states.depthBuffer = true;
			forwardPass.shaders.push_back(std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "BasicMaterial"));
			settings.AddPass(forwardPassIndex, forwardPass);

			MaterialPass depthPass = forwardPass;
			depthPass.options[CRC32("DepthPass")] = true;
			settings.AddPass(depthPassIndex, depthPass);

			MaterialPass shadowPass = depthPass;
			shadowPass.options[CRC32("ShadowPass")] = true;
			shadowPass.states.frontFace = FrontFace::Clockwise;
			shadowPass.states.depthClamp = enabledFeatures.depthClamping;
			settings.AddPass(shadowPassIndex, shadowPass);

			MaterialPass distanceShadowPass = shadowPass;
			distanceShadowPass.options[CRC32("DistanceDepth")] = true;
			settings.AddPass(distanceShadowPassIndex, distanceShadowPass);

			m_defaultMaterials.materials[MaterialType::Basic].material = std::make_shared<Material>(std::move(settings), "BasicMaterial");
		}

		// PbrMaterial
		{
			MaterialSettings settings;
			PredefinedMaterials::AddBasicSettings(settings);
			PredefinedMaterials::AddPbrSettings(settings);

			MaterialPass forwardPass;
			forwardPass.states.depthBuffer = true;
			forwardPass.shaders.push_back(std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "PhysicallyBasedMaterial"));
			settings.AddPass(forwardPassIndex, forwardPass);

			MaterialPass depthPass = forwardPass;
			depthPass.options[CRC32("DepthPass")] = true;
			settings.AddPass(depthPassIndex, depthPass);

			MaterialPass shadowPass = depthPass;
			shadowPass.options[CRC32("ShadowPass")] = true;
			shadowPass.states.frontFace = FrontFace::Clockwise;
			shadowPass.states.depthClamp = enabledFeatures.depthClamping;
			settings.AddPass(shadowPassIndex, shadowPass);

			MaterialPass distanceShadowPass = shadowPass;
			distanceShadowPass.options[CRC32("DistanceDepth")] = true;
			settings.AddPass(distanceShadowPassIndex, distanceShadowPass);

			m_defaultMaterials.materials[MaterialType::PhysicallyBased].material = std::make_shared<Material>(std::move(settings), "PhysicallyBasedMaterial");
		}

		// PhongMaterial
		{
			MaterialSettings settings;
			PredefinedMaterials::AddBasicSettings(settings);
			PredefinedMaterials::AddPhongSettings(settings);

			MaterialPass forwardPass;
			forwardPass.states.depthBuffer = true;
			forwardPass.shaders.push_back(std::make_shared<UberShader>(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, "PhongMaterial"));
			settings.AddPass(forwardPassIndex, forwardPass);

			MaterialPass depthPass = forwardPass;
			depthPass.options[CRC32("DepthPass")] = true;
			settings.AddPass(depthPassIndex, depthPass);

			MaterialPass shadowPass = depthPass;
			shadowPass.options[CRC32("ShadowPass")] = true;
			shadowPass.states.frontFace = FrontFace::Clockwise;
			shadowPass.states.depthClamp = enabledFeatures.depthClamping;
			settings.AddPass(shadowPassIndex, shadowPass);

			MaterialPass distanceShadowPass = shadowPass;
			distanceShadowPass.options[CRC32("DistanceDepth")] = true;
			settings.AddPass(distanceShadowPassIndex, distanceShadowPass);

			m_defaultMaterials.materials[MaterialType::Phong].material = std::make_shared<Material>(std::move(settings), "PhongMaterial");
		}

		for (auto&& [materialType, materialData] : m_defaultMaterials.materials.iter_kv())
		{
			materialData.presets[MaterialInstancePreset::Default] = materialData.material->GetDefaultInstance();

			materialData.presets[MaterialInstancePreset::NoDepth] = materialData.material->Instantiate();
			materialData.presets[MaterialInstancePreset::NoDepth]->DisablePass(depthPassIndex);
			materialData.presets[MaterialInstancePreset::NoDepth]->DisablePass(shadowPassIndex);
			materialData.presets[MaterialInstancePreset::NoDepth]->UpdatePassStates(forwardPassIndex, [](RenderStates& states)
			{
				states.depthBuffer = false;
			});

			materialData.presets[MaterialInstancePreset::Transparent] = materialData.material->Instantiate();
			materialData.presets[MaterialInstancePreset::Transparent]->DisablePass(depthPassIndex);
			materialData.presets[MaterialInstancePreset::Transparent]->DisablePass(shadowPassIndex);
			materialData.presets[MaterialInstancePreset::Transparent]->UpdatePassFlags(forwardPassIndex, MaterialPassFlag::SortByDistance);
			materialData.presets[MaterialInstancePreset::Transparent]->UpdatePassStates(forwardPassIndex, [](RenderStates& renderStates)
			{
				renderStates.depthBuffer = true;
				renderStates.depthWrite = false;
				renderStates.blending = true;
				renderStates.blend.modeColor = BlendEquation::Add;
				renderStates.blend.modeAlpha = BlendEquation::Add;
				renderStates.blend.srcColor = BlendFunc::SrcAlpha;
				renderStates.blend.dstColor = BlendFunc::InvSrcAlpha;
				renderStates.blend.srcAlpha = BlendFunc::One;
				renderStates.blend.dstAlpha = BlendFunc::One;
			});
		}
	}

	void Graphics::BuildDefaultPipelinePasses()
	{
		m_defaultPipelinePasses = std::make_shared<PipelinePassList>();

		// Forward pass
		std::size_t forwardColorOutput = m_defaultPipelinePasses->AddAttachment({
			"Forward output",
			PixelFormat::RGBA16F
		});

		std::size_t forwardDepthOutput = m_defaultPipelinePasses->AddAttachment({
			"Depth-stencil buffer",
			m_preferredDepthStencilFormat
		});

		std::size_t forwardPass = m_defaultPipelinePasses->AddPass("ForwardPass", m_pipelinePassRegistry.GetPassIndex("Forward"));

		m_defaultPipelinePasses->SetPassOutput(forwardPass, 0, forwardColorOutput);
		m_defaultPipelinePasses->SetPassDepthStencilOutput(forwardPass, forwardDepthOutput);

		m_defaultPipelinePasses->EnablePassFlags(forwardPass, FramePipelinePassFlag::LightShadowing);

		// Gamma correction
		std::size_t gammaCorrectionOutput = m_defaultPipelinePasses->AddAttachment({
			"Gamma-corrected output",
			PixelFormat::RGBA8
		});

		ParameterList gammaCorrectionParameters;
		gammaCorrectionParameters.SetParameter("Shader", "PostProcess.GammaCorrection");

		std::size_t gammaCorrectionPass = m_defaultPipelinePasses->AddPass("Gamma correction", m_pipelinePassRegistry.GetPassIndex("PostProcess"), gammaCorrectionParameters);

		m_defaultPipelinePasses->SetPassInput(gammaCorrectionPass, 0, forwardColorOutput);
		m_defaultPipelinePasses->SetPassOutput(gammaCorrectionPass, 0, gammaCorrectionOutput);

		m_defaultPipelinePasses->SetFinalOutput(gammaCorrectionOutput);
	}

	void Graphics::BuildDefaultTextures()
	{
		// Depth textures (white but with a depth format)
		{
			TextureInfo texInfo;
			texInfo.width = texInfo.height = texInfo.depth = texInfo.levelCount = 1;
			texInfo.pixelFormat = m_preferredDepthFormat;

			std::array<UInt8, 6> whitePixels = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

			for (auto&& [imageType, texture] : m_defaultTextures.depthTextures.iter_kv())
			{
				if (imageType == ImageType::E3D)
					continue;

				texInfo.type = imageType;
				texInfo.layerCount = (imageType == ImageType::Cubemap) ? 6 : 1;

				texture = m_renderDevice->InstantiateTexture(texInfo, whitePixels.data(), false);
			}
		}

		// White texture 2D
		{
			TextureInfo texInfo;
			texInfo.width = texInfo.height = texInfo.depth = texInfo.levelCount = 1;
			texInfo.pixelFormat = PixelFormat::L8;

			std::array<UInt8, 6> whitePixels = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

			for (auto&& [imageType, texture] : m_defaultTextures.whiteTextures.iter_kv())
			{
				texInfo.type = imageType;
				texInfo.layerCount = (imageType == ImageType::Cubemap) ? 6 : 1;

				texture = m_renderDevice->InstantiateTexture(texInfo, whitePixels.data(), false);
			}
		}
	}

	template<std::size_t N>
	void Graphics::RegisterEmbedShaderModule(const UInt8(&content)[N])
	{
		nzsl::Unserializer unserializer(content, N);
		m_shaderModuleResolver->RegisterModule(nzsl::Ast::UnserializeShader(unserializer));
	}

	void Graphics::RegisterMaterialPasses()
	{
		m_materialPassRegistry.RegisterPass("ForwardPass");
		m_materialPassRegistry.RegisterPass("DepthPass");
		m_materialPassRegistry.RegisterPass("ShadowPass");
		m_materialPassRegistry.RegisterPass("DistanceShadowPass");
	}

	void Graphics::RegisterPipelinePasses()
	{
		m_pipelinePassRegistry.RegisterPass<DepthPipelinePass>("Depth", {}, {});
		m_pipelinePassRegistry.RegisterPass<DebugDrawPipelinePass>("DebugDraw", { "Input" }, { "Output" });
		m_pipelinePassRegistry.RegisterPass<ForwardPipelinePass>("Forward", {}, { "Output" });
		m_pipelinePassRegistry.RegisterPass<PostProcessPipelinePass>("PostProcess", { "Input" }, { "Output" });
	}

	void Graphics::RegisterShaderModules()
	{
		m_shaderModuleResolver = std::make_shared<nzsl::FilesystemModuleResolver>();
		RegisterEmbedShaderModule(r_basicMaterialShader);
		RegisterEmbedShaderModule(r_fullscreenVertexShader);
		RegisterEmbedShaderModule(r_gammaCorrectionPass);
		RegisterEmbedShaderModule(r_instanceDataModule);
		RegisterEmbedShaderModule(r_lightDataModule);
		RegisterEmbedShaderModule(r_lightShadowModule);
		RegisterEmbedShaderModule(r_mathColorModule);
		RegisterEmbedShaderModule(r_mathConstantsModule);
		RegisterEmbedShaderModule(r_mathCookTorrancePBRModule);
		RegisterEmbedShaderModule(r_mathDepthModule);
		RegisterEmbedShaderModule(r_phongMaterialShader);
		RegisterEmbedShaderModule(r_physicallyBasedMaterialShader);
		RegisterEmbedShaderModule(r_skinningDataModule);
		RegisterEmbedShaderModule(r_skinningLinearModule);
		RegisterEmbedShaderModule(r_skeletalDataModule);
		RegisterEmbedShaderModule(r_textureBlitShader);
		RegisterEmbedShaderModule(r_viewerDataModule);

#ifdef NAZARA_DEBUG
		// Override embed files with dev files in debug
		if (std::filesystem::path modulePath = "../../src/Nazara/Graphics/Resources/Shaders"; std::filesystem::is_directory(modulePath))
			m_shaderModuleResolver->RegisterModuleDirectory(modulePath, true);
#endif

		// Let application register their own shaders
		if (std::filesystem::path shaderPath = "Shaders"; std::filesystem::is_directory(shaderPath))
			m_shaderModuleResolver->RegisterModuleDirectory(shaderPath);
	}

	void Graphics::SelectDepthStencilFormats()
	{
		for (PixelFormat depthStencilCandidate : { PixelFormat::Depth24, PixelFormat::Depth32F, PixelFormat::Depth16 })
		{
			if (m_renderDevice->IsTextureFormatSupported(depthStencilCandidate, TextureUsage::DepthStencilAttachment))
			{
				m_preferredDepthFormat = depthStencilCandidate;
				break;
			}
		}

		if (m_preferredDepthFormat == PixelFormat::Undefined)
			throw std::runtime_error("no supported depth format found");

		for (PixelFormat depthStencilCandidate : { PixelFormat::Depth24Stencil8, PixelFormat::Depth32FStencil8, PixelFormat::Depth16Stencil8 })
		{
			if (m_renderDevice->IsTextureFormatSupported(depthStencilCandidate, TextureUsage::DepthStencilAttachment))
			{
				m_preferredDepthStencilFormat = depthStencilCandidate;
				break;
			}
		}

		if (m_preferredDepthStencilFormat == PixelFormat::Undefined)
			throw std::runtime_error("no supported depth-stencil format found");
	}

	Graphics* Graphics::s_instance = nullptr;

	void Graphics::Config::Override(const CommandLineParameters& parameters)
	{
		if (parameters.HasFlag("use-dedicated-gpu"))
			useDedicatedRenderDevice = true;

		if (parameters.HasFlag("use-integrated-gpu"))
			useDedicatedRenderDevice = false;
	}
}
