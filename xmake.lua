set_xmakever("2.9.5")
set_project("MarkerSwap")
set_version("0.1.0")
set_languages("c++23")
set_warnings("allextra")
add_rules("mode.release")

-- CommonLibF4 as a submodule (extern/commonlibf4), Address Library based => works across game versions.
includes("extern/commonlibf4")

target("MarkerSwap")
    add_deps("commonlibf4")
    add_rules("commonlibf4.plugin", {
        name = "MarkerSwap",
        author = "you",
        description = "Replace Pip-Boy map markers with PNGs from Data/Interface/MapMarkers",
        email = "",
        options = { address_library = true }
    })
    add_files("src/plugin.cpp")
    add_includedirs("src")
