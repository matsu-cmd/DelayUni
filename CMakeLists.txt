cmake_minimum_required(VERSION 3.14.0)

option(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES "Enable VST 3 Plug-in Examples" OFF)
option(SMTG_ENABLE_VST3_HOSTING_EXAMPLES "Enable VST 3 Hosting Examples" OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "")

set(vst3sdk_SOURCE_DIR "F:/Development/vst3sdk")
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()

project(DelayUni
    # This is your plug-in version number. Change it here only.
    # Version number symbols usable in C++ can be found in
    # source/version.h and ${PROJECT_BINARY_DIR}/projectversion.h.
    VERSION 1.0.0.0 
    DESCRIPTION "DelayUni VST 3 Plug-in"
)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(DelayUni
    source/version.h
    source/delaycids.h
    source/delayprocessor.h
    source/delayprocessor.cpp
    source/delaycontroller.h
    source/delaycontroller.cpp
    source/delayentry.cpp
    source/paramids.h
)

#- VSTGUI Wanted ----
if(SMTG_ENABLE_VSTGUI_SUPPORT)
    target_sources(DelayUni
        PRIVATE
            resource/delayeditor.uidesc
    )
    target_link_libraries(DelayUni
        PRIVATE
            vstgui_support
    )
    smtg_target_add_plugin_resources(DelayUni
        RESOURCES
            "resource/delayeditor.uidesc"
    )
endif(SMTG_ENABLE_VSTGUI_SUPPORT)
# -------------------

smtg_target_add_plugin_snapshots (DelayUni
    RESOURCES
        resource/17E9695CE0E75467875D23F293CA69DB_snapshot.png
        resource/17E9695CE0E75467875D23F293CA69DB_snapshot_2.0x.png
)

target_link_libraries(DelayUni
    PRIVATE
        sdk
)

smtg_target_configure_version_file(DelayUni)

if(SMTG_MAC)
    smtg_target_set_bundle(DelayUni
        BUNDLE_IDENTIFIER com.matsucmd.dalayuni
        COMPANY_NAME "matsu-cmd"
    )
    smtg_target_set_debug_executable(DelayUni
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(DelayUni PRIVATE 
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DelayUni)

        smtg_target_set_debug_executable(DelayUni
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)
