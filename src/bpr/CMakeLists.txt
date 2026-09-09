find_package(nlohmann_json CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)

add_library(bprap_core STATIC
    core/ap/slot_data.cpp
)

target_include_directories(bprap_core PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(bprap_core PUBLIC nlohmann_json::nlohmann_json ZLIB::ZLIB Threads::Threads)
target_compile_features(bprap_core PUBLIC cxx_std_23)
set_target_properties(bprap_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

find_package(asio CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(bprap_net STATIC
    net/ap_connection_handler.cpp
    net/deathlink.cpp
)
target_include_directories(bprap_net PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_compile_definitions(bprap_net PUBLIC
    ASIO_STANDALONE
    AP_NO_SCHEMA
    _WEBSOCKETPP_CPP11_STL_
    _WEBSOCKETPP_CPP11_THREAD_
    _WEBSOCKETPP_NO_BOOST_
    WSWRAP_ASYNC_CLEANUP
)
target_link_libraries(bprap_net PUBLIC
    bprap_core
    asio::asio
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    nlohmann_json::nlohmann_json
)
target_compile_features(bprap_net PUBLIC cxx_std_23)
set_target_properties(bprap_net PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(bprap SHARED
    app/app.cpp
    app/main.cpp
)


target_link_libraries(bprap PRIVATE
    bprap_core
    bprap_net
    bprap::imgui
)

target_sources(bprap PRIVATE 
    ui/login_window.cpp
)

target_compile_features(bprap PRIVATE cxx_std_23)

set(_bprap_mod_name "bpr_apclient.dll")
set_target_properties(bprap PROPERTIES
    OUTPUT_NAME "bpr_apclient"
    PREFIX ""
    SUFFIX ".dll"
    # Without this the PDB is named after the target (mthap.pdb) and mod.dll's debug
    # directory points at the wrong file.
    PDB_NAME "bpr_apclient"
)