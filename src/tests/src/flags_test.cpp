// This is an RLL test script.
// It is public domain:
// Copyright (c) 2020 Elijah Hopp, No Rights Reserved.
//----------------------------------INCLUDES----------------------------------//
#include <RLL/RLL.hpp>

#define CATCH_CONFIG_MAIN 1
#include <catch-mini/catch-mini.hpp>
//-----------------------------LOADER_FLAG_TESTS------------------------------//
using namespace rll;
using namespace rll::unix_flags;
using namespace rll::windows_flags;

std::initializer_list<unix_flag> all_unix_flags_il {
    LOAD_LAZY, 
    LOAD_NOW, //Note this is second in the list and when passed to a ctor it should take preference over the first.
    LOAD_LOCAL,
    LOAD_GLOBAL,
    LOAD_DEEPBIND,
    LOAD_NODELETE,
    LOAD_NOLOAD
};

unsigned int all_unix_flags = 
      LOAD_LAZY 
    | LOAD_NOW
	| LOAD_LOCAL
	| LOAD_GLOBAL
	| LOAD_DEEPBIND
	| LOAD_NODELETE
	| LOAD_NOLOAD;

std::initializer_list<windows_flag> all_windows_flags_il {
    DONT_RESOLVE_REFERENCES,
    IGNORE_CODE_AUTHZ_LEVEL,
    REQUIRE_SIGNED_TARGET,
    REQUIRE_CURRENT_DIR_SAFE,
    LOAD_AS_DATAFILE,
    LOAD_AS_EXCLUSIVE_DATAFILE,
    LOAD_AS_IMAGE_RESOURCE,
    SEARCH_APPLICATION_DIR,
    SEARCH_DEFAULT_DIRS,
    SEARCH_DLL_LOAD_DIR,
    SEARCH_SYSTEM32_DIR,
    SEARCH_USER_DIRS,
    SEARCH_WITH_ALTERED_PATH
};

unsigned int all_windows_flags = 
      DONT_RESOLVE_REFERENCES
	| IGNORE_CODE_AUTHZ_LEVEL
	| REQUIRE_SIGNED_TARGET
	| REQUIRE_CURRENT_DIR_SAFE
	| LOAD_AS_DATAFILE
	| LOAD_AS_EXCLUSIVE_DATAFILE
	| LOAD_AS_IMAGE_RESOURCE
	| SEARCH_APPLICATION_DIR
	| SEARCH_DEFAULT_DIRS
	| SEARCH_DLL_LOAD_DIR
	| SEARCH_SYSTEM32_DIR
	| SEARCH_USER_DIRS
	| SEARCH_WITH_ALTERED_PATH;

TEST_CASE("Constructing loader_flags works"){
    loader_flags default_flags;
    REQUIRE(default_flags.get_unix_flags() == LOAD_LAZY);
    REQUIRE(default_flags.get_windows_flags() == 0);

    loader_flags all_loader_flags(all_unix_flags_il, all_windows_flags_il);
    //Note that it is all the Unix flags minus LOAD_LAZY because LOAD_NOW
    //and LOAD_LAZY are mutually exclusive, and the second should overwrite the
    //first in the list.
    REQUIRE(all_loader_flags.get_unix_flags() == (all_unix_flags & ~LOAD_LAZY));
    REQUIRE(all_loader_flags.get_windows_flags() == all_windows_flags);
}

TEST_CASE("Adding flags works"){
    loader_flags flags {
        { LOAD_LAZY, LOAD_LOCAL },
        {}        
    };

    flags.add_flag(LOAD_GLOBAL);
    flags.add_flag(LOAD_AS_DATAFILE);

    REQUIRE(flags.get_unix_flags() == (LOAD_LAZY | LOAD_GLOBAL | LOAD_LOCAL));
    REQUIRE(flags.get_windows_flags() == LOAD_AS_DATAFILE);

    //Exclusive flags.
    flags.add_flag(LOAD_NOW);
    REQUIRE(flags.get_unix_flags() == (LOAD_NOW | LOAD_GLOBAL | LOAD_LOCAL));
}

TEST_CASE("Removing flags works"){
    loader_flags flags(all_unix_flags_il, all_windows_flags_il);

    flags.remove_flag(LOAD_GLOBAL);
    flags.remove_flag(LOAD_AS_DATAFILE);
    REQUIRE(flags.get_unix_flags() == (all_unix_flags & ~(LOAD_GLOBAL | LOAD_LAZY)));
    REQUIRE(flags.get_windows_flags() == (all_windows_flags & ~LOAD_AS_DATAFILE));

    flags.add_flag(LOAD_GLOBAL);
    flags.remove_flag(LOAD_NOW);
    REQUIRE(flags.get_unix_flags() == (all_unix_flags & ~LOAD_NOW));
}

TEST_CASE("Clearing flags works"){
    loader_flags flags(all_unix_flags_il, all_windows_flags_il);

    flags.clear_unix_flags();
    REQUIRE(flags.get_unix_flags() == LOAD_LAZY);

    flags.clear_windows_flags();
    REQUIRE(flags.get_windows_flags() == 0);
}
