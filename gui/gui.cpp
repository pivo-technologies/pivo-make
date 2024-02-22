#include "gui.h"
#include "font.h"

#include "imgui/imgui.h"

#ifndef _WIN32
#include <GLFW/glfw3.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#else
#include <Windows.h>
#include <d3d11.h>
#include <cstdint>
#include <cstring>
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND, UINT, WPARAM, LPARAM );
#endif

#include <iostream>
#include <cstdio>

namespace gui
{
#ifndef _WIN32
    GLFWwindow* m_window = nullptr;
#else
    HWND m_window = nullptr;
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_device_context = nullptr;
    IDXGISwapChain* m_swap_chain = nullptr;
    ID3D11RenderTargetView* m_render_target_view = nullptr;

    LRESULT wnd_proc( HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam )
    {
        if ( ImGui_ImplWin32_WndProcHandler( hwnd, msg, wparam, lparam ) )
            return true;

        if ( msg == WM_DESTROY )
            PostQuitMessage( 0 );

        return DefWindowProcW( hwnd, msg, wparam, lparam );
    }
#endif

    bool init( )
    {
#ifndef _WIN32
        glfwSetErrorCallback( [ ]( int error, const char* description )
        {
            fprintf( stderr, "GLFW Error %d: %s\n", error, description );
        } );

        if ( !glfwInit( ) )
            return false;

        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
        glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

        m_window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "pivo-make", nullptr, nullptr );
        if ( !m_window )
            return false;

        glfwMakeContextCurrent( m_window );
        glfwSwapInterval( 1 );
#else
        WNDCLASSEXW wc;
        std::memset( &wc, 0, sizeof( wc ) );

        wc.cbSize = sizeof( wc );
        wc.style = CS_CLASSDC;
        wc.lpfnWndProc = wnd_proc;
        wc.hInstance = GetModuleHandle( nullptr );
        wc.lpszClassName = L"pivo-make";

        RegisterClassExW( &wc );

        RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW, FALSE, 0 );

        m_window = CreateWindowExW( 0, wc.lpszClassName, L"pivo-make", WS_OVERLAPPEDWINDOW & ~( WS_MINIMIZEBOX | WS_MAXIMIZEBOX ), CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, wc.hInstance, nullptr );

        if ( !m_window )
            return false;

        DXGI_SWAP_CHAIN_DESC sd;
        std::memset( &sd, 0, sizeof( sd ) );

        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = m_window;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        uint32_t create_device_flags = 0;
        D3D_FEATURE_LEVEL feature_level;
        const D3D_FEATURE_LEVEL feature_level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        HRESULT res = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &sd, &m_swap_chain, &m_device, &feature_level, &m_device_context );
        if ( res == DXGI_ERROR_UNSUPPORTED )
            res = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &sd, &m_swap_chain, &m_device, &feature_level, &m_device_context );

        if ( res != S_OK )
        {
            shutdown( );
            return false;
        }

        ID3D11Texture2D* back_buffer = nullptr;
        m_swap_chain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( void** )&back_buffer );
        m_device->CreateRenderTargetView( back_buffer, nullptr, &m_render_target_view );
        back_buffer->Release( );

        ShowWindow( m_window, SW_SHOWDEFAULT );
        UpdateWindow( m_window );
#endif

        ImGui::CreateContext( );
        ImGuiIO& io = ImGui::GetIO( );
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = nullptr;

        ImFontConfig font_config = ImFontConfig( );
        font_config.FontDataOwnedByAtlas = false;
        io.Fonts->AddFontFromMemoryTTF( NotoSans_Regular, sizeof( NotoSans_Regular ), 18.0f, &font_config );

        ImGui::embraceTheDarkness( );

#ifndef _WIN32
        ImGui_ImplGlfw_InitForOpenGL( m_window, true );    
        ImGui_ImplOpenGL3_Init( "#version 130" );    
#else
        ImGui_ImplWin32_Init( m_window );
        ImGui_ImplDX11_Init( m_device, m_device_context );
#endif

        return true;
    }

    void shutdown( )
    {
#ifndef _WIN32
        ImGui_ImplOpenGL3_Shutdown( );
        ImGui_ImplGlfw_Shutdown( );
#else
        ImGui_ImplDX11_Shutdown( );
        ImGui_ImplWin32_Shutdown( );
#endif

        ImGui::DestroyContext( );

#ifdef _WIN32
        if ( m_render_target_view )
        {
            m_render_target_view->Release( );
            m_render_target_view = nullptr;
        }

        if ( m_swap_chain )
        {
            m_swap_chain->Release( );
            m_swap_chain = nullptr;
        }

        if ( m_device_context )
        {
            m_device_context->Release( );
            m_device_context = nullptr;
        }

        if ( m_device )
        {
            m_device->Release( );
            m_device = nullptr;
        }
#endif

        if ( m_window )
        {
#ifndef _WIN32
            glfwDestroyWindow( m_window );
#else
            DestroyWindow( m_window );
            UnregisterClassW( L"pivo-make", GetModuleHandle( nullptr ) );
#endif
            m_window = nullptr;
        }

#ifndef _WIN32
        glfwTerminate( );
#endif
    }

    bool new_frame( )
    {
#ifndef _WIN32
        if ( glfwWindowShouldClose( m_window ) )
            return false;

        glfwPollEvents( );
        ImGui_ImplOpenGL3_NewFrame( );
        ImGui_ImplGlfw_NewFrame( );
#else
        MSG msg;
        while ( PeekMessageW( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessageW( &msg );

            if ( msg.message == WM_QUIT )
                return false;
        }

        ImGui_ImplDX11_NewFrame( );
        ImGui_ImplWin32_NewFrame( );
#endif

        ImGui::NewFrame( );

        return true;
    }

    void render( )
    {
        ImGui::Render( );
#ifndef _WIN32
                int display_w, display_h;
                glfwGetFramebufferSize( m_window, &display_w, &display_h) ;
                glViewport( 0, 0, display_w, display_h );
                glClearColor( 0.25f, 0.25f, 0.25f, 1.00f );
                glClear( GL_COLOR_BUFFER_BIT );
                ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData( ) );

                glfwSwapBuffers( m_window );
            #else
                m_device_context->OMSetRenderTargets( 1, &m_render_target_view, nullptr );
                float clear_color[4] = { 0.25f, 0.25f, 0.25f, 1.00f };
                m_device_context->ClearRenderTargetView( m_render_target_view, clear_color );
                ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

                m_swap_chain->Present( 1, 0 );
#endif
    }
}