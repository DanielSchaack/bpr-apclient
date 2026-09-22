find_package(nlohmann_json CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(bprap SHARED
    app/app.cpp
    app/main.cpp

    core/ap/ap_state.cpp
    core/ap/slot_data.cpp
    core/broadcast.cpp

    net/ap_net.cpp
    net/deathlink.cpp

    ui/gui.cpp
    ui/login_window.cpp
    ui/broadcast_banner.cpp

    hooks/game_hooks.cpp

    hooks/function/disable_trigger.cpp
    hooks/function/disable_event_start.cpp
    hooks/function/detect_breakable.cpp
    hooks/function/disable_add_car.cpp
    hooks/function/event_win.cpp
    hooks/function/license_upgrade.cpp
    hooks/function/deathlink.cpp
    hooks/function/enable_event.cpp

    hooks/structure/wndproc.cpp
    hooks/structure/render.cpp
    hooks/structure/game_loop.cpp
    hooks/structure/save_control.cpp
    hooks/structure/connection_wait.cpp
    hooks/structure/game_action_queue.cpp
    core/logger.cpp
    hooks/function/wreck_control.cpp
)

target_include_directories(bprap PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(bprap PUBLIC
    nlohmann_json::nlohmann_json 
    ZLIB::ZLIB 
    Threads::Threads
    asio::asio
    OpenSSL::SSL
    OpenSSL::Crypto
    bprap::imgui
    minhook::minhook
    bprap::apclient
    crypt32
)
target_compile_features(bprap PUBLIC cxx_std_23)
set_target_properties(bprap PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(bprap PUBLIC
    ASIO_STANDALONE
    AP_NO_SCHEMA
    _WEBSOCKETPP_CPP11_STL_
    _WEBSOCKETPP_CPP11_THREAD_
    _WEBSOCKETPP_NO_BOOST_
    WSWRAP_ASYNC_CLEANUP
)


target_compile_definitions(bprap PRIVATE
    BPRAP_RELEASE_VERSION=\"${BPRAP_RELEASE_VERSION}\"
    BPRAP_GAME_NAME=\"${BPRAP_GAME_NAME}\"
)

set(_bprap_mod_name "bpr_apclient.dll")
set_target_properties(bprap PROPERTIES
    OUTPUT_NAME "bpr_apclient"
    PREFIX ""
    SUFFIX ".dll"
    # Without this the PDB is named after the target (mthap.pdb) and mod.dll's debug
    # directory points at the wrong file.
    PDB_NAME "bpr_apclient"
)
