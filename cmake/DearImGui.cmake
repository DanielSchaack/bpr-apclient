# DearImGui.cmake - Dear ImGui via FetchContent, built as an OBJECT lib. The
# the stock D3D12 + Win32 backends on Windows. Vendored third-party: warnings
# silenced, not held to our -Werror policy.
include_guard(GLOBAL)
include(FetchContent)

FetchContent_Declare(
    dearimgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(dearimgui)

# Windows: stock D3D12 + Win32 backends. They include only <d3d12.h>,
# <dxgi*.h>, <windows.h> - all provided by the MSVC SDK
add_library(bprap_imgui OBJECT
    "${dearimgui_SOURCE_DIR}/imgui.cpp"
    "${dearimgui_SOURCE_DIR}/imgui_draw.cpp"
    "${dearimgui_SOURCE_DIR}/imgui_tables.cpp"
    "${dearimgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${dearimgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp"
    "${dearimgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
)
target_include_directories(bprap_imgui SYSTEM PUBLIC
    "${dearimgui_SOURCE_DIR}"
    "${dearimgui_SOURCE_DIR}/backends"
)
# Skip the XInput gamepad path (avoids the xinput import lib + runtime dep).
target_compile_definitions(bprap_imgui PUBLIC IMGUI_IMPL_WIN32_DISABLE_GAMEPAD)
# d3dcompiler: imgui_impl_dx12 compiles its shaders at init via D3DCompile.
# dxguid: COM IID_* symbols. dwmapi: imgui_impl_win32's alpha-compositing
# helper calls Dwm*. clang-cl picks these up via #pragma comment(lib) too,
# but mingw ignores those pragmas, so list them explicitly for both.
target_link_libraries(bprap_imgui PUBLIC d3d11 dxgi dxguid d3dcompiler dwmapi user32)


set_target_properties(bprap_imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Vendored third-party: silence its warnings (we are -Werror elsewhere).
if(MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(bprap_imgui PRIVATE /W0)
else()
    target_compile_options(bprap_imgui PRIVATE -w)
endif()

add_library(bprap::imgui ALIAS bprap_imgui)
