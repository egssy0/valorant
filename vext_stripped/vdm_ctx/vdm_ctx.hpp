#pragma once
#include <windows.h>
#include <string_view>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include "../util/util.hpp"

namespace vdm
{
	// change this to whatever you want :^)
	constexpr std::pair<const char*, const char*> syscall_hook = { "NtDeleteAtom", "ntdll.dll" };
	inline std::atomic<bool> is_page_found = false;
	inline std::atomic<void*> syscall_address = nullptr;
	inline std::uint16_t nt_page_offset;
	inline std::uint32_t nt_rva;
	inline std::uint8_t* ntoskrnl;

	using read_phys_t = std::function<bool(void*, void*, std::size_t)>;
	using write_phys_t = std::function<bool(void*, void*, std::size_t)>;

	typedef struct _SYSTEM_BIGPOOL_ENTRY
	{
		union {
			PVOID VirtualAddress;
			ULONG_PTR NonPaged : 1;
		};
		ULONG_PTR SizeInBytes;
		union {
			UCHAR Tag[ 4 ];
			ULONG TagUlong;
		};
	} SYSTEM_BIGPOOL_ENTRY, * PSYSTEM_BIGPOOL_ENTRY;

	typedef struct _SYSTEM_BIGPOOL_INFORMATION {
		ULONG Count;
		SYSTEM_BIGPOOL_ENTRY AllocatedInfo[ ANYSIZE_ARRAY ];
	} SYSTEM_BIGPOOL_INFORMATION, * PSYSTEM_BIGPOOL_INFORMATION;


	typedef enum _SYS_INFO_CLASS {
		SystemBasicInformation = 0,
		SystemPerformanceInformation = 2,
		SystemTimeOfDayInformation = 3,
		SystemProcessInformation = 5,
		SystemProcessorPerformanceInformation = 8,
		SystemInterruptInformation = 23,
		SystemExceptionInformation = 33,
		SystemRegistryQuotaInformation = 37,
		SystemLookasideInformation = 45,
		SystemCodeIntegrityInformation = 103,
		SystemPolicyInformation = 134,
		SystemBigPoolInformation = 66,
	} SYS_INFO_CLASS;

	using ZwQuerySystemInformation = NTSTATUS( __stdcall* )(
		SYS_INFO_CLASS SystemInformationClass,
		PVOID SystemInformation,
		ULONG SystemInformationLength,
		PULONG ReturnLength
		);

	typedef NTSTATUS(WINAPI* pNtQuerySystemInformation)(
		IN SYS_INFO_CLASS SystemInformationClass,
		OUT PVOID                   SystemInformation,
		IN ULONG                    SystemInformationLength,
		OUT PULONG                  ReturnLength
		);

	class vdm_ctx
	{
	public:
		explicit vdm_ctx();
		explicit vdm_ctx(read_phys_t& read_func, write_phys_t& write_func);
		void set_read(read_phys_t& read_func);
		void set_write(write_phys_t& write_func);
		void rkm(void* dst, void* src, std::size_t size);
		void wkm(void* dst, void* src, std::size_t size);

		template <class T, class ... Ts>
		__forceinline std::invoke_result_t<T, Ts...> syscall(void* addr, Ts ... args) const
		{
			static const auto proc =
				GetProcAddress(
					LoadLibraryA(syscall_hook.second),
					syscall_hook.first
				);

			static std::mutex syscall_mutex;
			syscall_mutex.lock();

			// jmp [rip+0x0]
			std::uint8_t jmp_code[] =
			{
				0xff, 0x25, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
				0x00, 0x00
			};

			std::uint8_t orig_bytes[sizeof jmp_code];
			*reinterpret_cast<void**>(jmp_code + 6) = addr;
			read_phys(vdm::syscall_address.load(), orig_bytes, sizeof orig_bytes);

			// execute hook...
			write_phys(vdm::syscall_address.load(), jmp_code, sizeof jmp_code);
			auto result = reinterpret_cast<T>(proc)(args ...);
			write_phys(vdm::syscall_address.load(), orig_bytes, sizeof orig_bytes);

			syscall_mutex.unlock();
			return result;
		}

		template <class T>
		__forceinline auto rkm(std::uintptr_t addr) -> T
		{
			T buffer;
			rkm((void*)&buffer, (void*)addr, sizeof T);
			return buffer;
		}

		template <class T>
		__forceinline void wkm(std::uintptr_t addr, const T& value)
		{
			wkm((void*)addr, (void*)&value, sizeof T);
		}

		__forceinline auto get_peprocess(std::uint32_t pid) -> PEPROCESS
		{
			static const auto ps_lookup_peproc =
				util::get_kmodule_export(
					"ntoskrnl.exe",
					"PsLookupProcessByProcessId");

			PEPROCESS peproc = nullptr;
			this->syscall<PsLookupProcessByProcessId>(
				ps_lookup_peproc,
				(HANDLE)pid,
				&peproc
			);
			return peproc;
		}

		__forceinline auto get_dirbase(std::uint32_t pid) -> std::uintptr_t
		{
			const auto peproc = get_peprocess(pid);

			if (!peproc) 
				return {};

			return rkm<cr3>(
				reinterpret_cast<std::uintptr_t>(peproc) + 0x28).pml4_pfn << 12;
		}

		__forceinline auto get_base(std::uint32_t pid) -> std::uintptr_t
		{
			static const auto get_base =
				util::get_kmodule_export(
					"ntoskrnl.exe", "PsGetProcessSectionBaseAddress");

			return this->syscall<uintptr_t(*)(PEPROCESS)>(
				get_base, get_peprocess(pid));
		}

		__forceinline auto free_pool( PVOID pool ) -> void
		{
			static const auto ex_free_pool =
				util::get_kmodule_export(
					"ntoskrnl.exe", "ExFreePool" );

			this->syscall<ExFreePool>(
				ex_free_pool,
				pool);
		}

		__forceinline auto allocate_pool( POOL_TYPE type, SIZE_T size ) -> PVOID
		{
			static const auto ex_allocate_pool =
				util::get_kmodule_export(
					"ntoskrnl.exe", "ExAllocatePool" );

			return this->syscall<ExAllocatePool>(
				ex_allocate_pool,
				type,
				size);
		}

		__forceinline auto query_bigpools() -> PSYSTEM_BIGPOOL_INFORMATION
		{
			static const pNtQuerySystemInformation NtQuerySystemInformation =
				(pNtQuerySystemInformation)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");

			DWORD length = 0;
			DWORD size = 0;
			LPVOID heap = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0);
			heap = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, heap, 0xFF);
			NTSTATUS ntLastStatus = NtQuerySystemInformation(SystemBigPoolInformation, heap, 0x30, &length);
			heap = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, heap, length + 0x1F);
			size = length;
			ntLastStatus = NtQuerySystemInformation(SystemBigPoolInformation, heap, size, &length);

			return reinterpret_cast<PSYSTEM_BIGPOOL_INFORMATION>(heap);
		}
		__forceinline auto retrieve_guarded() -> uintptr_t
		{
			auto pool_information = this->query_bigpools();
			uintptr_t guarded = 0;

			if (pool_information)
			{
				auto count = pool_information->Count;
				for (auto i = 0ul; i < count; i++)
				{
					SYSTEM_BIGPOOL_ENTRY* allocation_entry = &pool_information->AllocatedInfo[i];
					const auto virtual_address = (PVOID)((uintptr_t)allocation_entry->VirtualAddress & ~1ull);
					if (allocation_entry->NonPaged && allocation_entry->SizeInBytes == 0x200000)
						if (guarded == 0 && allocation_entry->TagUlong == 'TnoC')
							guarded = reinterpret_cast<uintptr_t>(virtual_address);
				}
			}

			return guarded;
		}

		__forceinline auto get_peb(std::uint32_t pid) -> PPEB
		{
			static const auto get_peb = 
				util::get_kmodule_export(
					"ntoskrnl.exe", "PsGetProcessPeb");

			return this->syscall<PPEB(*)(PEPROCESS)>(
				get_peb, get_peprocess(pid));
		}

	private:
		void locate_syscall(std::uintptr_t begin, std::uintptr_t end) const;
		bool valid_syscall(void* syscall_addr) const;

		read_phys_t read_phys;
		write_phys_t write_phys;
	};
}