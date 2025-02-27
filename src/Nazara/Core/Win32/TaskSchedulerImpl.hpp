// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_WIN32_TASKSCHEDULERIMPL_HPP
#define NAZARA_CORE_WIN32_TASKSCHEDULERIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Functor.hpp>
#include <atomic>
#include <memory>
#include <queue>
#include <Windows.h>

namespace Nz
{
	class TaskSchedulerImpl
	{
		public:
			TaskSchedulerImpl() = delete;
			~TaskSchedulerImpl() = delete;

			static bool Initialize(std::size_t workerCount);
			static bool IsInitialized();
			static void Run(AbstractFunctor** tasks, std::size_t count);
			static void Uninitialize();
			static void WaitForTasks();

		private:
			static AbstractFunctor* StealTask(std::size_t workerID);
			static unsigned int __stdcall WorkerProc(void* userdata);

			struct Worker
			{
				std::atomic_size_t workCount;
				std::queue<AbstractFunctor*> queue;
				CRITICAL_SECTION queueMutex;
				HANDLE wakeEvent;
				volatile bool running;
			};

			static std::unique_ptr<HANDLE[]> s_doneEvents; // Doivent être contigus
			static std::unique_ptr<Worker[]> s_workers;
			static std::unique_ptr<HANDLE[]> s_workerThreads; // Doivent être contigus
			static DWORD s_workerCount;
};
}

#endif // NAZARA_CORE_WIN32_TASKSCHEDULERIMPL_HPP
