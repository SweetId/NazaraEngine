// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERRESOURCES_HPP
#define NAZARA_RENDERER_RENDERRESOURCES_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <functional>
#include <type_traits>
#include <vector>

namespace Nz
{
	class CommandBuffer;
	class CommandBufferBuilder;
	class RenderDevice;
	class UploadPool;

	class NAZARA_RENDERER_API RenderResources
	{
		public:
			class Releasable;
			class ReleasableCallback;
			template<typename T> class ReleasableData;
			template<typename T> class ReleasableLambda;

			virtual ~RenderResources();

			virtual void Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags) = 0;

			inline void FlushReleaseQueue();

			virtual UInt32 GetImageIndex() const = 0;
			inline RenderDevice& GetRenderDevice();
			virtual UploadPool& GetUploadPool() = 0;

			template<typename T> void PushForRelease(const T& value) = delete;
			template<typename T> void PushForRelease(T&& value);
			template<typename F> void PushReleaseCallback(F&& callback);

			virtual void SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags) = 0;

		protected:
			inline RenderResources(RenderDevice& renderDvice);
			RenderResources(const RenderResources&) = delete;
			RenderResources(RenderResources&&) = delete;

		private:
			template<typename T> T* Allocate();
			inline void* Allocate(std::size_t size, std::size_t alignment);

			static constexpr std::size_t BlockSize = 4 * 1024 * 1024;

			using Block = std::vector<UInt8>;

			std::vector<ReleasableCallback*> m_callbackQueue;
			std::vector<Releasable*> m_releaseQueue;
			std::vector<Block> m_releaseMemoryPool;
			RenderDevice& m_renderDevice;
	};

	class NAZARA_RENDERER_API RenderResources::Releasable
	{
		public:
			virtual ~Releasable();
	};

	class RenderResources::ReleasableCallback : public Releasable
	{
		public:
			virtual void Release() = 0;
	};

	template<typename T>
	class RenderResources::ReleasableData : public Releasable
	{
		public:
			ReleasableData(T&& data);
			ReleasableData(const ReleasableData&) = delete;
			ReleasableData(ReleasableData&&) = delete;
			~ReleasableData() = default;

			ReleasableData& operator=(const ReleasableData&) = delete;
			ReleasableData& operator=(ReleasableData&&) = delete;

		private:
			T m_data;
	};

	template<typename T>
	class RenderResources::ReleasableLambda final : public ReleasableCallback
	{
		public:
			template<typename U> ReleasableLambda(U&& lambda);
			ReleasableLambda(const ReleasableLambda&) = delete;
			ReleasableLambda(ReleasableLambda&&) = delete;
			~ReleasableLambda() = default;

			void Release() override;

			ReleasableLambda& operator=(const ReleasableLambda&) = delete;
			ReleasableLambda& operator=(ReleasableLambda&&) = delete;

		private:
			T m_lambda;
	};
}

#include <Nazara/Renderer/RenderResources.inl>

#endif // NAZARA_RENDERER_RENDERRESOURCES_HPP
