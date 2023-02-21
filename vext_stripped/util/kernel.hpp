class kernel_mgr
{
private:
	std::uintptr_t dir_base;
public:
	std::uint32_t process_id;
	std::uintptr_t guard_address;

	kernel_mgr(vdm::vdm_ctx vdm, const char* process_name)
	{
		this->process_id = util::get_pid(process_name);
		if (this->process_id == 0)
		{
			printf("[+] Process not found\n");
			return;
		}

		this->dir_base = vdm.get_dirbase(this->process_id);
		this->guard_address = vdm.retrieve_guarded();
	}

	//bool is_valid()
	//{
	//	return this->process_id != 0 && this->base_address != 0 && this->dir_base != 0;
	//}

	bool is_guarded(uintptr_t pointer) // not my function
	{
		constexpr uintptr_t filter = 0xFFFFFFFF00000000;
		uintptr_t result = pointer & filter;
		return result == 0x8000000000 || result == 0x10000000000;
	}

	template<typename T>
	T validate_pointer(T address) // not my function
	{
		if (is_guarded(*(uintptr_t*)&address)) {
			const auto result = this->guard_address + ((*reinterpret_cast<uintptr_t*>(&address)) & 0xFFFFFF);
			return *(T*)(&result);
		}
		else
			return address;
	}

	template<typename T = std::uintptr_t> auto read(std::uintptr_t address) -> T
	{
		if ( address == 0 || address == 0xFFFFFFFF ) return T{};

		return this->validate_pointer<T>(rpm<T>(this->dir_base, address));
	}

	template<typename T = std::uintptr_t, typename T2 = std::uintptr_t> auto read(T2 address) -> T
	{
		if ((uintptr_t)address == 0 || (uintptr_t)address == 0xFFFFFFFF ) return T{};

		return this->validate_pointer<T>(rpm<T>(this->dir_base, (uintptr_t)address));
	}

	template<typename T = std::uintptr_t, typename T2 = std::uintptr_t> auto read_unguarded(T2 address) -> T
	{
		if ((uintptr_t)address == 0 || (uintptr_t)address == 0xFFFFFFFF) return T{};

		return rpm<T>(this->dir_base, (uintptr_t)address);
	}

	template<typename T = std::uintptr_t, typename T2 = std::uintptr_t> auto write( T2 address, T value ) -> void
	{
		wpm<T>( this->dir_base, (uintptr_t) address, value );
	}
};