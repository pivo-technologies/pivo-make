#include "gui.h"
#include "font.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include <cstdio>

namespace gui
{
    GLFWwindow* m_window = nullptr;

    bool init( )
    {
        glfwSetErrorCallback( [ ]( int error, const char* description )
        {
            fprintf( stderr, "GLFW Error %d: %s\n", error, description );
        } );

        if ( !glfwInit( ) )
            return false;

        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
        glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

        m_window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "pivo-make", nullptr, nullptr) ;
        if ( !m_window )
            return false;

        glfwMakeContextCurrent( m_window );
        glfwSwapInterval( 1 );

        ImGui::CreateContext( );
        ImGuiIO& io = ImGui::GetIO( );
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = nullptr;

        ImFontConfig font_config = ImFontConfig( );
        font_config.FontDataOwnedByAtlas = false;
        io.Fonts->AddFontFromMemoryTTF( NotoSans_Regular, sizeof( NotoSans_Regular ), 18.0f, &font_config );

        ImGui::embraceTheDarkness( );

        ImGui_ImplGlfw_InitForOpenGL( m_window, true );    
        ImGui_ImplOpenGL3_Init( "#version 130" );    

        return true;
    }

    void shutdown( )
    {
        ImGui_ImplOpenGL3_Shutdown( );
        ImGui_ImplGlfw_Shutdown( );
        ImGui::DestroyContext( );

        if ( m_window )
        {
            glfwDestroyWindow( m_window );
            m_window = nullptr;
        }

        glfwTerminate( );
    }

    bool new_frame( )
    {
        if ( glfwWindowShouldClose( m_window ) )
            return false;

        glfwPollEvents( );
        ImGui_ImplOpenGL3_NewFrame( );
        ImGui_ImplGlfw_NewFrame( );
        ImGui::NewFrame( );

        return true;
    }

    void render( )
    {
        ImGui::Render( );
        int display_w, display_h;
        glfwGetFramebufferSize( m_window, &display_w, &display_h) ;
        glViewport( 0, 0, display_w, display_h );
        glClearColor( 0.25f, 0.25f, 0.25f, 1.00f );
        glClear( GL_COLOR_BUFFER_BIT );
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData( ) );

        glfwSwapBuffers( m_window );
    }
}