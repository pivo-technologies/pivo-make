#include "gui/gui.h"
#include "gui/imgui/imgui.h"

#include "options/options.h"
#include "builder/builder.h"
#include "config/config.h"

#include <iostream>
#include <algorithm>
#include <cstring>

std::string to_lower( const std::string& str )
{
    std::string lower_str = str;
    std::transform( lower_str.begin( ), lower_str.end( ), lower_str.begin( ), ::tolower );
    return lower_str;
}

size_t stristr( const std::string& str, const std::string& substr )
{
    std::string lower_str = to_lower( str );
    std::string lower_sub_str = to_lower( substr );
    return lower_str.find( lower_sub_str );
}

int main( int argc, char** argv )
{
    bool config_present = config::read( );
    options::init( );

    std::memset( options::m_config_file, 0, sizeof( options::m_config_file ) );
    std::memset( options::m_output_file, 0, sizeof( options::m_output_file ) );
    std::memset( options::m_intermediate_dir, 0, sizeof( options::m_intermediate_dir ) );

    if ( argc > 1 )
    {
        for ( int i = 1; i < argc; i++ )
        {
            if ( !strcmp( argv[ i ], "-c" ) || !strcmp( argv[ i ], "--config" ) )
            {
                if ( i + 1 >= argc )
                {
                    std::cout << "Usage: pivo-make {-c|--config} <config_file> {-o|--output} <output_file>\n";
                    return 1;
                }

                strncpy( options::m_config_file, argv[ i + 1 ], sizeof( options::m_config_file ) );

                ++i;
                continue;
            }

            if ( !strcmp( argv[ i ], "-o" ) || !strcmp( argv[ i ], "--output" ) )
            {
                if ( i + 1 >= argc )
                {
                    std::cout << "Usage: pivo-make {-c|--config} <config_file> {-o|--output} <output_file>\n";
                    return 1;
                }

                strncpy( options::m_output_file, argv[ i + 1 ], sizeof( options::m_output_file ) );

                ++i;
                continue;
            }

            if ( !strcmp( argv[ i ], "-i" ) || !strcmp( argv[ i ], "--intermediate" ) )
            {
                if ( i + 1 >= argc )
                {
                    std::cout << "Usage: pivo-make {-c|--config} <config_file> {-o|--output} <output_file>\n";
                    return 1;
                }

                strncpy( options::m_intermediate_dir, argv[ i + 1 ], sizeof( options::m_intermediate_dir ) );

                ++i;
                continue;
            }

            if ( !strcmp( argv[ i ], "-h" ) || !strcmp( argv[ i ], "--help" ) )
            {
                std::cout << "Usage: pivo-make {-c|--config} <config_file> {-o|--output} <output_file>\n";
                return 0;
            }

            std::cout << "Ignoring unknown argument: " << argv[ i ] << "\n";
        }

        if ( options::m_config_file[ 0 ] )
        {
            options::open( );
            return builder::build( );
        }
    }

    if ( !gui::init( ) )
        return 1;

    int current_tab = 0;
    int current_sub_tab = 0;
    char all_options_search[ 256 ];

    std::memset( all_options_search, 0, sizeof( all_options_search ) );

    int build = -1;

    while ( gui::new_frame( ) )
    {
        ImGui::SetNextWindowSize( ImVec2( WINDOW_WIDTH, WINDOW_HEIGHT ) );
        ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
        ImGui::Begin( "##main_window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar ); 

        ImGui::Text( "Choose platform:" ); 
        ImGui::SameLine( );
        bool change = ImGui::RadioButton( "Linux", &options::m_compiler, 0 );
        ImGui::SameLine( );
        change = change || ImGui::RadioButton( "Windows", &options::m_compiler, 1 );

        if ( change )
        {
            current_tab = 0;
            current_sub_tab = 0;
            options::init( );
        }

        ImGui::SetCursorPos( ImVec2( 326, 8 ) );
        ImGui::Text( "  Config file:" );
        ImGui::SameLine( );
        ImGui::SetNextItemWidth( 300 );
        ImGui::InputText( "##config_file", options::m_config_file, sizeof( options::m_config_file ) );
        ImGui::SameLine( );        
        if ( ImGui::Button( "Open" ) )
            options::open( );

        ImGui::SameLine( );
        if ( ImGui::Button( "Save" ) )
            options::save( );

        ImGui::SameLine( );
        if ( ImGui::Button( "Build" ) )
            build = builder::build( );

        if ( build != -1 )
        {
            ImGui::SameLine( );
            
            if ( build )
                ImGui::Text( "Compiled" );
            else
                ImGui::Text( "Build failed" );
        }

        ImVec2 cursor_pos = ImGui::GetCursorPos( );
        ImGui::BeginChild( "##tabs", ImVec2( 300, 500 ) );
        if ( ImGui::CollapsingHeader( options::m_c_tab.m_name.c_str( ) ) )
        {
            for ( int i = 0; i < options::m_c_tab.m_subtabs.size( ); i++ )
            {
                if ( ImGui::Selectable( options::m_c_tab.m_subtabs[ i ].m_name.c_str( ), current_sub_tab == i && !current_tab ) )
                {
                    std::memset( all_options_search, -0, sizeof( all_options_search ) );
                    current_tab = 0;
                    current_sub_tab = i;
                }
            }
        }

        if ( ImGui::CollapsingHeader( options::m_linker_tab.m_name.c_str( ) ) )
        {
            for ( int i = 0; i < options::m_linker_tab.m_subtabs.size( ); i++ )
            {
                if ( ImGui::Selectable( options::m_linker_tab.m_subtabs[ i ].m_name.c_str( ), current_sub_tab == i && current_tab ) )
                {
                    std::memset( all_options_search, -0, sizeof( all_options_search ) );
                    current_tab = 1;
                    current_sub_tab = i;
                }
            }
        }

        ImGui::EndChild( );

        cursor_pos.x += 325;
        ImGui::SetCursorPos( cursor_pos );
        ImGui::BeginChild( "##options", ImVec2( WINDOW_WIDTH - 325, 0 ) );

        options::tab_t* chosen_tab = current_tab == 1 ? &options::m_linker_tab : &options::m_c_tab;

        if ( chosen_tab->m_subtabs.empty( ) )
        {
            ImGui::EndChild( );
            ImGui::End( );
            gui::render( );
            continue;
        }

        options::subtab_t* chosen_subtab = &chosen_tab->m_subtabs[ current_sub_tab ];

        int sub_tab = current_sub_tab;

        if ( chosen_subtab->m_name.find( "All Options" ) != std::string::npos )
        {
            sub_tab = 0;

            ImGui::Text( "Search:" ); 
            ImGui::SameLine( 300.f );
            ImGui::SetNextItemWidth( 362.f );
            ImGui::InputText( "##search", all_options_search, sizeof( all_options_search ) );
            ImGui::NewLine( );
        }
        else
            std::memset( all_options_search, 0, sizeof( all_options_search ) );
        
        do
        {
            chosen_subtab = &chosen_tab->m_subtabs[ sub_tab ];
            for ( int i = 0; i < chosen_subtab->m_options.size( ); i++ )
            {
                options::option_t* option = &chosen_subtab->m_options[ i ];

                if ( all_options_search[ 0 ] && stristr( option->m_name, all_options_search ) == std::string::npos )
                    continue;

                ImGui::Text( "%s", option->m_name.c_str( ) );
                ImGui::SameLine( 300.f );
                ImGui::SetNextItemWidth( 362.f );

                if ( option->m_is_single_line )
                {
                    char buffer[ 4096 ];
                    strcpy( buffer, option->m_params.c_str( ) );
                    ImGui::InputText( ( std::string( "##params" ) + std::to_string( i ) ).c_str( ), buffer, sizeof( buffer ) );
                    option->m_params = buffer;
                }
                else
                {                
                    ImGui::Combo( ( std::string( "##choices" ) + std::to_string( i ) ).c_str( ), &option->m_current_choice, [ ]( void* user_data, int idx ) -> const char*
                    {
                        return reinterpret_cast< options::option_t* >( user_data )->m_choices[ idx ].m_user_output.data( );
                    }, ( void* )option, option->m_choices.size( ) );
                }
            }

            if ( chosen_subtab->m_name.find( "Command Line" ) != std::string::npos )
            {
                ImGui::Text( "Command Line:" );
                ImGui::InputTextMultiline( "##command_line", chosen_tab->m_command_line, sizeof( chosen_tab->m_command_line ), ImVec2( 662, 0 ) );
            }
        } while ( ++sub_tab < current_sub_tab );

        ImGui::EndChild( );

        ImGui::SetCursorPos( ImVec2( 20, WINDOW_HEIGHT - 30 ) );

        ImGui::Text( "Load Defaults:" );

        ImGui::SameLine( );        
        if ( ImGui::Button( "Debug" ) )
            options::load_defaults_debug( );

        ImGui::SameLine( );
        if ( ImGui::Button( "Release" ) )
            options::load_defaults_release( );

        if ( !config_present )
        {
            ImGui::SameLine( );
            ImGui::Text( "Config file is not found. Windows includes and libs are not present. Try reinstalling pivo-make using install.py script" );
        }

        ImGui::End( );

        gui::render( );    
    }

    gui::shutdown( );

    return 0;
}