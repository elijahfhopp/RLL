// This is inline content for the RLL headeronly file.
// It is public domain:
// Copyright (c) 2020 Elijah Hopp, No Rights Reserved.

std::mutex shared_library::_mutex;

shared_library::shared_library() {
	lib_handle = 0;
}
shared_library::~shared_library() {}

void shared_library::load(const std::string& path, int flags) {
	std::lock_guard<std::mutex> lock(_mutex);

	if (lib_handle != nullptr) { 
		throw exception::library_already_loaded(path);
	}

	lib_handle = dlopen(path.c_str(), flags);
	
	if (lib_handle == nullptr) {
		const char* error = dlerror();
		throw exception::library_loading_error(error);
	}
	lib_path = path;
}

void shared_library::load(const std::string& path, loader_flags flags) {
	load(path, flags.get_unix_flags());
}

void shared_library::unload() {
	std::lock_guard<std::mutex> lock(_mutex);

	if (lib_handle) {
		dlclose(lib_handle);
		lib_handle = nullptr;
	}
}


bool shared_library::is_loaded() {
	return lib_handle != nullptr;
}


void * shared_library::get_symbol(const std::string& name) {
	std::lock_guard<std::mutex> lock(_mutex);

	if (lib_handle != nullptr) {
		void* result = dlsym(lib_handle, name.c_str());
		char* error = dlerror();

		if (error != nullptr) {
			if (std::strcmp(error, "") != 0) {
				throw exception::symbol_not_found(name);
			}
		}
		
		return result;
	} else {
		throw exception::library_not_loaded();
	}
}

void * shared_library::get_symbol_fast(const std::string& name) noexcept {
	std::lock_guard<std::mutex> lock(_mutex);

	if (lib_handle != nullptr) {
		return dlsym(lib_handle, name.c_str());
	} else {
		return nullptr;
	}
}


const std::string& shared_library::get_path() {
	return lib_path;
}

void * shared_library::get_platform_handle() {
	return lib_handle;
}

std::string shared_library::get_platform_suffix() {
	#if defined(__APPLE__)
		return ".dylib";
	#elif defined(__CYGWIN__)
		return ".dll";
	#else
		return ".so";
	#endif
}
