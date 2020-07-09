// This is RLL. A Runtime Library Loader.
// It is public domain:
// Copyright (c) 2020 Elijah Hopp, No Rights Reserved.
//--------------------------------HEADER_GUARD--------------------------------//
#ifndef RLL_HPP_
#define RLL_HPP_

//----------------------------------INCLUDES----------------------------------//
#include <exception>
#include <cstring>
#include <string>
#include <mutex>

//------------------------------------RLL-------------------------------------//
#define RLL_VERSION_MAJOR 1
#define RLL_VERSION_MINOR 0
#define RLL_VERSION_PATCH 0 

// Packed via major * 10000 + minor * 100 + patch.
#define RLL_VERSION 10000

#ifdef _WIN32 
	#define RLL_PLATFORM_IS_WINDOWS
#else
	#define RLL_PLATFORM_IS_UNIX
#endif

namespace rll {

//------------------------------RLL_DECLARATIONS------------------------------//
namespace exception {
////////////////////////////////////////////////////////////////////////////////
/// @brief A generic exception that simply denotes that the exception came from RLL.
////////////////////////////////////////////////////////////////////////////////
class rll_exception : public std::exception {};

// All the actual exceptions are declared and defined later...

} // exception

//-----------------SHARED_LIBRARY_DECLARATION---------------------------------//
namespace unix_flags {
////////////////////////////////////////////////////////////////////////////////
/// @brief An enum of this: https://linux.die.net/man/3/dlopen.
////////////////////////////////////////////////////////////////////////////////
enum unix_flag {
    LOAD_LAZY = 0x00001,
    LOAD_NOW = 0x00002,
    //
    LOAD_LOCAL = 0x00000,
    LOAD_GLOBAL = 0x00100,
    LOAD_DEEPBIND = 0x00008,
    LOAD_NODELETE = 0x01000,
    LOAD_NOLOAD = 0x00004
};
} // unix_flag

namespace windows_flags {
////////////////////////////////////////////////////////////////////////////////
/// @brief A enum containing the flags from this:
/// https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryexw
////////////////////////////////////////////////////////////////////////////////
enum windows_flag {
    DONT_RESOLVE_REFERENCES = 0x00000001,
    //
    IGNORE_CODE_AUTHZ_LEVEL = 0x00000010,
    REQUIRE_SIGNED_TARGET = 0x00000080,
    REQUIRE_CURRENT_DIR_SAFE = 0x00002000,
    //
    LOAD_AS_DATAFILE = 0x00000002,
    LOAD_AS_EXCLUSIVE_DATAFILE = 0x00000040,
    LOAD_AS_IMAGE_RESOURCE = 0x00000020,
    //
    SEARCH_APPLICATION_DIR = 0x00000200,
    SEARCH_DEFAULT_DIRS = 0x00001000,
    SEARCH_DLL_LOAD_DIR = 0x00000100,
    SEARCH_SYSTEM32_DIR = 0x00000800,
    SEARCH_USER_DIRS = 0x00000400,
    SEARCH_WITH_ALTERED_PATH = 0x00000008,
};

} // windows_flag

using windows_flag = windows_flags::windows_flag;
using unix_flag = unix_flags::unix_flag;

////////////////////////////////////////////////////////////////////////////////
/// @brief A container for library loader flags. 
///
/// @details This container's  purpose is to make passing flags to the underling
/// (platform backend) library loader easy, and multiplatform. All that you need
/// to do is add the flags that will be used on each platform and the loader you
/// pass the flags to will deal with the rest.
////////////////////////////////////////////////////////////////////////////////
class loader_flags {
    private:
        ////////////////////////////////////////////////////////////////////////////////
        /// @brief The internal Unix loader flags that are modified by methods.
        ////////////////////////////////////////////////////////////////////////////////
        unsigned int uflags;
        ////////////////////////////////////////////////////////////////////////////////
        /// @brief The internal Windows loader flags that are modified by methods.
        ////////////////////////////////////////////////////////////////////////////////
        unsigned int wflags;
    public:
        loader_flags() : uflags(unix_flags::LOAD_LAZY), wflags(0){}
        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct a new loader flags object.
        /// 
        /// @param unix_flags All the Unix loader flags you want enabled.
        /// @param windows_flags All the Windows loader flags you want enabled.
        ////////////////////////////////////////////////////////////////////////////////
        loader_flags(std::initializer_list<unix_flag> unix_flags, std::initializer_list<windows_flag> windows_flags);

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Add a Unix loader flag to the internal flags.
        /// @param flag The flag.
        ////////////////////////////////////////////////////////////////////////////////
        void add_flag(unix_flag flag);
        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Add Window loader flag to the internal flags.
        /// @param flag The flag.
        ////////////////////////////////////////////////////////////////////////////////
        void add_flag(windows_flag flag);

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Remove an Unix loader flag.
        /// @param flag The flag.
        ////////////////////////////////////////////////////////////////////////////////
        void remove_flag(unix_flag flag);
        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Remove a Windows loader flag.
        /// @param flag The flag.
        ////////////////////////////////////////////////////////////////////////////////
        void remove_flag(windows_flag flag);

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Looks for a Unix loader flag in the internal flags.
        /// @param flag The flag that is searched for.
        /// @return bool Wether the flag is present.
        ////////////////////////////////////////////////////////////////////////////////
        bool has_flag(unix_flag flag);
        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Looks for a Windows loader flag in the internal flags.
        /// @param flag The flag that is searched for.
        /// @return bool Wether the flag is present.
        ////////////////////////////////////////////////////////////////////////////////
        bool has_flag(windows_flag flag);

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Get the Unix loader flags.
        /// @return unsigned int The stored Unix loader flags.
        ////////////////////////////////////////////////////////////////////////////////
        unsigned int get_unix_flags();
        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Get the Windows loader flags.
        /// @return unsigned int The stored Windows loader flags.
        ////////////////////////////////////////////////////////////////////////////////
        unsigned int get_windows_flags();

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Clear all the Unix loader flags.
        ////////////////////////////////////////////////////////////////////////////////
        void clear_unix_flags();
        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Clear all the Windows loader flags.
        ////////////////////////////////////////////////////////////////////////////////
        void clear_windows_flags();
};


////////////////////////////////////////////////////////////////////////////////
/// @brief An interface for loading shared libraries at run-time.
///
/// @details Hey! This is a simple class that allows for simple yet powerful
/// interface for loading and processing shared libraries (dynamic libraries) at
/// run-time. It is completely multi-platform (for the operating systems you
/// have actually heard of atleast, :P) and the API is too! 
///
/// It has an exception-based way of processing errors (no more error codes,
/// yay!). This means if you haven't properly loaded a library before you try to
/// get a symbol or something it will throw an error.
///
/// Better than anything is a code example:
/// ```cpp
/// // The function signature using for briefness.
/// using func_type = int(int, int);
/// // Contains a function add two integers:
/// rll::shared_library test_lib;
/// try {
///     test_lib.load("test_lib.so");
/// } catch (rll::exception::library_loading_error& e) {
///     std::cout << "Oh noes! We had an issue loading the shared library:\n" << e.what() << "\n"; 
/// }
///
/// std::function<func_type> add_function;
///
/// if (test_lib.has_symbol("add")) {
///     add_function = reinterpret_cast<func_type*>(test_lib.get_symbol("add"));
/// }
///
/// std::cout << add_function(2, 4) << "\n"; // returns "6";
///
/// test_lib.unload(); // Clears object and closes the shared library.
///
/// //Repeat...
/// ```
////////////////////////////////////////////////////////////////////////////////
class shared_library {
	private:
		shared_library(const shared_library&);
		shared_library& operator=(const shared_library&);
		//
		std::string lib_path;
		void* lib_handle;
		static std::mutex _mutex;
		//
		void load(const std::string& path, int flags);
	public:
		////////////////////////////////////////////////////////////////////////////////
		/// @brief Construct a new shared library object.
		////////////////////////////////////////////////////////////////////////////////
		shared_library();
		////////////////////////////////////////////////////////////////////////////////
		/// @brief Destroy the shared library object.
		////////////////////////////////////////////////////////////////////////////////
		virtual ~shared_library();

		////////////////////////////////////////////////////////////////////////////////
		/// @brief Loads a shared library from a given path. 
		///
		/// @details This ctor takes the path to a shared library and then loads
		/// it with no flags on Windows or the default flag
		/// (RTLD_LAZY/LOAD_LAZY) on Unix. If it fails to load the shared
		/// library it will throw a `library_not_loaded` exception with the
		/// relevent information. If a library is already loaded into the object
		/// then it throws a `library_already_loaded` exception.
		///
		/// @param path The path to the shared library.
		///
		/// @throw rll::exception::library_loading_error 
		/// @throw rll::exception::library_already_loaded
		////////////////////////////////////////////////////////////////////////////////
		void load(const std::string& path) { load(path, loader_flags()); }

		////////////////////////////////////////////////////////////////////////////////
		/// @brief Loads a shared library from a path with flags.
		///
		/// @details This ctor takes the path to shared library and also
		/// loader_flags. These flags tell the platform backend how to load the
		/// library. It also throws the same exception as the other overload
		/// (`load(std::string)`).
		///
		/// @param path The path to the shared library. 
		/// @param flags The flags that are used by the platform backend.
		///
		/// @see load
		///
		/// @throw rll::exception::library_loading_error 
		/// @throw rll::exception::library_already_loaded
		////////////////////////////////////////////////////////////////////////////////
		void load(const std::string& path, loader_flags flags);

		////////////////////////////////////////////////////////////////////////////////
		/// @brief Unloads the loaded if there is one loaded.
		////////////////////////////////////////////////////////////////////////////////
		void unload();

		////////////////////////////////////////////////////////////////////////////////
		/// @brief Returns wether a shared library has been loaded into the object.
		/// @return true A shared library is loaded.
		/// @return false A shared library isn't loaded.
		////////////////////////////////////////////////////////////////////////////////
		bool is_loaded();

		////////////////////////////////////////////////////////////////////////////////
		/// @brief Returns wether the loaded shared library has a symbol in it.
		///
		/// @details It looks for a symbol. Simple as that. But if a library
		/// hasn't been loaded it throws an `library_not_loaded` exception.
		///
		/// @param name The name of the symbol.
		/// @return true The symbol was found.
		/// @return false The symbol wasn't found.
		///
		/// @throw rll::exception::library_not_loaded
		////////////////////////////////////////////////////////////////////////////////
		bool has_symbol(const std::string& name) { return get_symbol_fast(name) != nullptr; }

		////////////////////////////////////////////////////////////////////////////////
		/// @brief Attempts to retreive a symbol.
		///
		/// @details It attempts to find a symbol but if it can't find a symbol
		/// it throws a `symbol_not_found` error. If a shared library has not
		/// been loaded into the object then it throws a `library_not_loaded`
		/// exception. 
		///
		/// @warning C++ library symbols are NOT demangled! If you dislike this
		/// enough open a Github issue and I will re-visit it. 
		///
		/// @param name The name of the symbol. 
		/// @return void * The pointer to the retrieved symbol.
		///
		/// @throw rll::exception::library_not_loaded 
		/// @throw rll::exception::symbol_not_found
		////////////////////////////////////////////////////////////////////////////////
		void * get_symbol(const std::string& name);

        ////////////////////////////////////////////////////////////////////////////////
        /// @brief Get a symbol without exception handling.
        ///
        /// @details A fast symbol accesser that doesn't handle exception states
        /// for faster runtimes. This is only to be used if you know what you
        /// are doing. You can get any errors with the corresponding platform
        /// backends.
        ///
        /// @param name The name of the symbol attempting to be accessed.
        /// @return void* A pointer to the acessed symbol (typically a nullptr
        /// if it failed.)
        ////////////////////////////////////////////////////////////////////////////////
        void * get_symbol_fast(const std::string& name) noexcept;

		////////////////////////////////////////////////////////////////////////////////
		/// @brief Get the path to the loaded shared library.
		///
		/// @details If no library has been loaded it returns an empty string. 
		///
		/// @return const std::string& The path to the loaded library.
		////////////////////////////////////////////////////////////////////////////////
		const std::string& get_path();

		////////////////////////////////////////////////////////////////////////////////
		/// @brief Gets the underlying platform library handle
		///
		/// @details It does what the label says. It also *doesn't* throw a
		/// `library_not_loaded` exception if a library has not been loaded. In
		/// such a case it will (probably) return a null pointer.
		///
		/// @return void * The pointer to the platform library handle.
		////////////////////////////////////////////////////////////////////////////////
		void * get_platform_handle();

		////////////////////////////////////////////////////////////////////////////////
		/// @brief Get the platform suffix for shared/dynamic libraries.
		///
		/// @details It returns the _commonly used_ platform-specific filename
		/// suffix for libraries. I.e. Windows: ".dll", MacOS: ".dylib",
		/// Unix/other: ".so"
		///
		/// @return std::string The commonly used platform file extention for
		/// shared libraries. 
		////////////////////////////////////////////////////////////////////////////////
		static std::string get_platform_suffix();
};

//------------------------------RLL_DEFINITIONS-------------------------------//
#define RLL_DEFINE_EXCEPTION_W_METADATA(EXCP_NAME, METADATA_TYPE, METADATA_NAME, WHAT_RETURN) \
    class EXCP_NAME : public rll_exception { \
        public: \
            METADATA_TYPE METADATA_NAME; \
            EXCP_NAME(METADATA_TYPE METADATA_NAME) : METADATA_NAME(METADATA_NAME){} \
            const char* what() const noexcept { \
                WHAT_RETURN \
            } \
    }; \

#define RLL_DEFINE_EXCEPTION(EXCP_NAME, WHAT_RETURN)\
    class EXCP_NAME : public rll_exception { \
        public: \
            const char* what() const noexcept { \
                WHAT_RETURN \
            } \
    }; \

namespace exception {
////////////////////////////////////////////////////////////////////////////////
/// @brief If a symbol was not found in a shared library this exception is thrown.
////////////////////////////////////////////////////////////////////////////////
RLL_DEFINE_EXCEPTION_W_METADATA(symbol_not_found, std::string, symbol_name, return symbol_name.c_str();)
////////////////////////////////////////////////////////////////////////////////
/// @brief If a loading method is invoked but a shared library already has
/// something loaded this exception is thrown.
////////////////////////////////////////////////////////////////////////////////
RLL_DEFINE_EXCEPTION_W_METADATA(library_already_loaded, std::string, library_path, return library_path.c_str();)
////////////////////////////////////////////////////////////////////////////////
/// @brief If a accessor method is invoked but a shared library has no content
/// loaded into it this exception is thrown.
////////////////////////////////////////////////////////////////////////////////
RLL_DEFINE_EXCEPTION(library_not_loaded, return "A shared_library has not been loaded with content before use.";)
////////////////////////////////////////////////////////////////////////////////
/// @brief If an error occurred while loading content into a shared library this
/// exception is thrown.
////////////////////////////////////////////////////////////////////////////////
RLL_DEFINE_EXCEPTION_W_METADATA(library_loading_error, std::string, loading_error, return (loading_error != "" ? loading_error.c_str() : "Unknown Error.");)
} // exception

// Shared library platform implementations:
#ifdef RLL_PLATFORM_IS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef ERROR
#include "platform/sl_windows_impl.inl"
#else
#include <dlfcn.h>
#include "platform/sl_unix_impl.inl"
#endif

loader_flags::loader_flags(std::initializer_list<unix_flag> unix_flags, std::initializer_list<windows_flag> windows_flags) {
    uflags = 0;
    wflags = 0;
    for (auto& it : unix_flags) {
        add_flag(it);
    }
    for (auto& it : windows_flags) {
        add_flag(it);
    }
}

void loader_flags::add_flag(unix_flag flag) { 
    //LOAD_LAZY and LOAD_NOW are mutually exclusive:
    if (flag == unix_flags::LOAD_LAZY) {
        if (this->has_flag(unix_flags::LOAD_NOW)) {
            remove_flag(unix_flags::LOAD_NOW);
        }
    } else if (flag == unix_flags::LOAD_NOW) {
        if (this->has_flag(unix_flags::LOAD_LAZY)) {
            remove_flag(unix_flags::LOAD_LAZY);
        }
    }

    uflags |= flag; 
}

void loader_flags::add_flag(windows_flag flag) { wflags |= flag; }

void loader_flags::remove_flag(unix_flag flag) { 
    if (flag == unix_flags::LOAD_LAZY) {
        uflags &= ~flag;
        add_flag(unix_flags::LOAD_NOW);
    } else if (flag == unix_flags::LOAD_NOW) {
        uflags &= ~flag;
        add_flag(unix_flags::LOAD_LAZY);
    }

    uflags &= ~flag; 
}
void loader_flags::remove_flag(windows_flag flag) { wflags &= ~flag; }

bool loader_flags::has_flag(unix_flag flag) {
    return true ? ((uflags & flag) == flag) : false;
}
bool loader_flags::has_flag(windows_flag flag) {
    return true ? ((wflags & flag) == flag) : false;
}

void loader_flags::clear_unix_flags() { uflags = unix_flags::LOAD_LAZY; }
void loader_flags::clear_windows_flags() { wflags = 0; }

unsigned int loader_flags::get_unix_flags() { return uflags; }
unsigned int loader_flags::get_windows_flags() { return wflags; }

} // rll
//-----------------------------------END_IF-----------------------------------//
#endif //RLL_HPP_