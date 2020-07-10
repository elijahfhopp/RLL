// This is inline content for the RLL headeronly file.
// It is public domain:
// Copyright (c) 2020 Elijah Hopp, No Rights Reserved.

std::mutex shared_library::_mutex;

shared_library::shared_library(){
	lib_handle = nullptr;
}

shared_library::~shared_library(){
	unload();
}

void shared_library::load(const std::string& path, int flags){
	std::lock_guard<std::mutex> lock(_mutex);

	if(lib_handle != nullptr){ 
		throw exception::library_already_loaded(lib_path);
	}

	lib_handle = LoadLibraryExA(path.c_str(), 0, flags);

	if(!lib_handle){
		throw exception::library_loading_error(path);
	}

	lib_path = path;
}

void shared_library::load(const std::string& path, loader_flags flags){
	load(path, flags.get_windows_flags());
}

void shared_library::unload(){
	std::lock_guard<std::mutex> lock(_mutex);

	if(lib_handle != nullptr){
		FreeLibrary((HMODULE) lib_handle);
		lib_handle = nullptr;
	}

	lib_path.clear();
}


bool shared_library::is_loaded(){
	return lib_handle != nullptr;
}


void * shared_library::get_symbol(const std::string& name){
	std::lock_guard<std::mutex> lock(_mutex);

	if(lib_handle != nullptr){
		return static_cast<void *>(GetProcAddress((HMODULE) lib_handle, name.c_str()));
	} else {
		throw exception::library_not_loaded();
	}
}

void * shared_library::get_symbol_fast(const std::string& name) noexcept {
	std::lock_guard<std::mutex> lock(_mutex);

	if(lib_handle != nullptr){
		return static_cast<void *>(GetProcAddress((HMODULE) lib_handle, name.c_str()));
	} else {
		return nullptr;
	}
}

const std::string& shared_library::get_path(){
	return lib_path;
}


void * shared_library::get_platform_handle(){
	return lib_handle;
}

std::string shared_library::get_platform_suffix(){
	return ".dll";
}
