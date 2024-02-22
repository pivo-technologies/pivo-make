#include "builder.h"
#include "../options/options.h"
#include "../config/config.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

namespace builder
{
    std::string replace_sequence( std::string str, const std::string& seq, const std::string& new_seq ) 
    {
        size_t pos = 0;
        while ( ( pos = str.find( seq, pos ) ) != std::string::npos ) 
        {
            str.replace( pos, 1, new_seq );
            pos += new_seq.length( );
        }

        return str;
    }

    std::string wrap_string( const std::string& str )
    {
        return "\"" + str + "\"";
    }

    std::string trim_spaces( const std::string& str )
    {
        std::string result = str;
        result.erase( result.begin( ), std::find_if( result.begin( ), result.end( ), [ ]( unsigned char ch ) {
            return !std::isspace( ch );
        } ) );
        result.erase( std::find_if( result.rbegin( ), result.rend( ), [ ]( unsigned char ch ) {
            return !std::isspace( ch );
        } ).base( ), result.end( ) );

        return result;
    }

    std::string parse_params_list( const std::string& params, const std::string& begin = std::string( ) )
    {
        std::string result;
        std::istringstream iss( params );

        for( std::string s; std::getline( iss, s, ';' ); )
            result += begin + wrap_string( trim_spaces( s ) ) + " ";

        return trim_spaces( result );
    }

    void run_process( const std::string& process )
    {
#ifdef _WIN32
        STARTUPINFOA startup_info;
        PROCESS_INFORMATION process_info;

        std::memset( &startup_info, 0, sizeof( startup_info ) );
        std::memset( &process_info, 0, sizeof( process_info ) );

        startup_info.cb = sizeof( startup_info );

        startup_info.dwFlags = STARTF_USESHOWWINDOW;
        startup_info.wShowWindow = SW_HIDE;

        std::string command = "cmd.exe /C " + wrap_string( process );
        char* cmd = _strdup( command.c_str( ) );

        if ( !CreateProcessA( NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startup_info, &process_info ) ) 
        {
            std::cout << "CreateProcessA failed with error " << GetLastError( ) << std::endl;
            return;
        }

        WaitForSingleObject( process_info.hProcess, INFINITE );

        CloseHandle( process_info.hProcess );
        CloseHandle( process_info.hThread );

        free( cmd );
#else
        system( process.c_str( ) );
#endif
    }

    bool build( )
    {
        bool multithread = false;

        std::vector< std::string > cxx_commands;
        std::vector< std::string > linker_commands;
        std::vector< std::string > excluded_files;

        std::string cxx_standard;
        std::string c_standard;
        std::string output_filename;

        cxx_commands.push_back( options::m_compiler == 1 ? wrap_string( config::m_cxx_windows ) : wrap_string( config::m_cxx_linux ) );
        cxx_commands.push_back( "-c" );

        if ( options::m_compiler == 1 )
        {
            cxx_commands.push_back( "--target=x86_64-pc-windows-msvc" );
            linker_commands.push_back( wrap_string( config::m_linker_windows ) );

            bool manifest_uac_control = false;

            for ( auto& sub_tab : options::m_linker_tab.m_subtabs )
            {
                for ( auto& option : sub_tab.m_options )
                {
                    if ( option.m_is_single_line )
                    {
                        if ( option.m_params.empty( ) )
                            continue;

                        if ( option.m_name == "Output File" )
                        {
                            output_filename = ( option.m_params.empty( ) || options::m_output_file[ 0 ] ) ? options::m_output_file : option.m_params.data( );
                            linker_commands.push_back( "/out:" + wrap_string( output_filename ) );
                        }

                        if ( option.m_name == "Version" )
                            linker_commands.push_back( "/version:" + wrap_string( option.m_params ) );

                        if ( option.m_name == "Additional Library Directories" )
                            linker_commands.push_back( parse_params_list( option.m_params, "/libpath:" ) );

                        if ( option.m_name == "Specify Section Attributes" )
                            linker_commands.push_back( parse_params_list( option.m_params, "/section:" ) );

                        if ( option.m_name == "Additional Dependencies" )
                            linker_commands.push_back( parse_params_list( option.m_params ) );
                        
                        if ( option.m_name == "Ignore Specific Default Libraries" )
                            linker_commands.push_back( parse_params_list( option.m_params, "/nodefaultlib:" ) );

                        if ( option.m_name == "Generate Program Database File" )
                            linker_commands.push_back( "/pdb:" + wrap_string( option.m_params ) );

                        if ( option.m_name == "Map File" )
                            linker_commands.push_back( "/map:" + wrap_string( option.m_params ) );

                        if ( option.m_name == "Heap Size (reserve,commit)" )
                        {
                            std::string reserve, commit;
                            std::istringstream iss( option.m_params );
                            std::getline( iss, reserve, ';' );
                            std::getline( iss, commit, ';' );

                            std::string heap = reserve + ( commit.empty( ) ? "" : "," + commit );
                            linker_commands.push_back( "/heap:" + wrap_string( heap ) );
                        }

                        if ( option.m_name == "Stack Size (reserve,commit)" )
                        {
                            std::string reserve, commit;
                            std::istringstream iss( option.m_params );
                            std::getline( iss, reserve, ';' );
                            std::getline( iss, commit, ';' );

                            std::string stack = reserve + ( commit.empty( ) ? "" : "," + commit );
                            linker_commands.push_back( "/stack:" + wrap_string( stack ) );
                        }

                        if ( option.m_name == "Function Order" )
                            linker_commands.push_back( "/order:" + wrap_string( option.m_params ) );

                        if ( option.m_name == "Entry Point" )
                            linker_commands.push_back( "/entry:" + option.m_params );

                        if ( option.m_name == "Base Address" )
                            linker_commands.push_back( "/base:" + wrap_string( option.m_params ) );

                        if ( option.m_name == "Import Library" )
                            linker_commands.push_back( "/implib:" + wrap_string( option.m_params ) );

                        if ( option.m_name == "Merge Sections" )
                            linker_commands.push_back( "/merge:" + wrap_string( option.m_params ) );
                    }
                    else
                    {
                        if ( option.m_name == "Enable User Account Control" )
                        {
                            manifest_uac_control = option.m_current_choice == 1;
                            continue;
                        }

                        const std::string& command_line = option.m_choices[ option.m_current_choice ].m_cmd_line;

                        if ( !manifest_uac_control && ( option.m_name == "UAC Execution Level" || option.m_name == "UAC Bypass UI Protection" ) )
                            continue;

                        if ( !command_line.empty( ) )
                            linker_commands.push_back( command_line );
                    }
                }
            }
        }
        else
        {       
            cxx_commands.push_back( "--target=x86_64-unknown-linux-gnu" );     
            linker_commands.push_back( wrap_string( config::m_linker_linux ) );

            for ( auto& sub_tab : options::m_linker_tab.m_subtabs )
            {
                for ( auto& option : sub_tab.m_options )
                {
                    if ( option.m_is_single_line )
                    {
                        if ( option.m_params.empty( ) )
                            continue;

                        if ( option.m_name == "Output File" )
                        {
                            linker_commands.push_back( "-o" );

                            output_filename = ( option.m_params.empty( ) || options::m_output_file[ 0 ] ) ? options::m_output_file : option.m_params.data( );
                            linker_commands.push_back( wrap_string( output_filename ) );
                        }

                        if ( option.m_name == "Trace Symbols" )
                            linker_commands.push_back( parse_params_list( option.m_params, "-Wl,--trace-symbol" ) );

                        if ( option.m_name == "Additional Library Directories" )
                            linker_commands.push_back( parse_params_list( option.m_params, "-L" ) );

                        if ( option.m_name == "Ignore Specific Default Libraries" )
                        {
                            linker_commands.push_back( "-Wl,--exclude-libs" );
                            linker_commands.push_back( replace_sequence( option.m_params, ";", "," ) );
                        }

                        if ( option.m_name == "Force Undefined Symbol References" )
                            linker_commands.push_back( parse_params_list( option.m_params, "-Wl,--undefined" ) );

                        if ( option.m_name == "Library Dependencies" )
                            linker_commands.push_back( parse_params_list( option.m_params, "-l" ) );

                        if ( option.m_name == "Additional Dependencies" )
                            linker_commands.push_back( option.m_params );

                        if ( option.m_name == "Map File Name" )
                            linker_commands.push_back( "-Wl,-Map=" + wrap_string( option.m_params ) );
                    }
                    else
                    {
                        const std::string& command_line = option.m_choices[ option.m_current_choice ].m_cmd_line;
                        if ( !command_line.empty( ) )
                            linker_commands.push_back( command_line );
                    }
                }
            }
        }

        std::string lib_command = options::m_compiler == 1 ? "/libpath:" : "-L ";
        const std::vector< std::string >& lib_path_vector = options::m_compiler == 1 ? config::m_lib_path_windows : config::m_lib_path_linux;

        for ( const auto& path : lib_path_vector )
            linker_commands.push_back( lib_command + wrap_string( path ) );

        linker_commands.push_back( options::m_linker_tab.m_command_line );

        for ( auto& sub_tab : options::m_c_tab.m_subtabs )
        {
            for ( auto& option : sub_tab.m_options )
            {
                if ( option.m_is_single_line )
                {
                    if ( option.m_params.empty( ) )
                        continue;

                    if ( option.m_name == "Additional Include Directories" )
                        cxx_commands.push_back( parse_params_list( option.m_params, "-idirafter" ) );

                    if ( option.m_name == "Exclude Files From Build" )
                    {
                        std::istringstream iss( option.m_params );

                        for( std::string s; std::getline( iss, s, ';' ); )
                            excluded_files.push_back( trim_spaces( s ) );
                    }

                    if ( option.m_name.find( "Additional Warnings" ) != std::string::npos )
                        cxx_commands.push_back( parse_params_list( option.m_params, "-W" ) );

                    if ( option.m_name == "Preprocessor Definitions" )
                        cxx_commands.push_back( parse_params_list( replace_sequence( option.m_params, "\"", "\\\"" ), "-D " ) );

                    if ( option.m_name == "Undefine Preprocessor Definitions" )
                        cxx_commands.push_back( parse_params_list( option.m_params, "-U " ) );

                    if ( option.m_name == "Precompiled Header File" )
                    {
                        cxx_commands.push_back( "-include-pch" );
                        cxx_commands.push_back( wrap_string( option.m_params ) );
                    }

                    if ( option.m_name == "Force Include File" )
                        cxx_commands.push_back( parse_params_list( option.m_params, "-include " ) );
                }
                else if ( option.m_choices.size( ) )
                {
                    const std::string& command_line = option.m_choices[ option.m_current_choice ].m_cmd_line;
                    if ( !multithread && command_line == "/MP" )
                    {
                        multithread = true;
                    }
                    else if ( !command_line.empty( ) )
                    {
                        if ( option.m_name == "C++ Language Standard" )
                            cxx_standard = command_line;
                        else if ( option.m_name == "C Language Standard" )
                            c_standard = command_line;
                        else
                            cxx_commands.push_back( command_line );
                    }
                }
            }
        }

        const std::vector< std::string >& include_path_vector = options::m_compiler == 1 ? config::m_include_path_windows : config::m_include_path_linux;
        for ( const auto& path : include_path_vector )
        {
            cxx_commands.push_back( "-idirafter" );
            cxx_commands.push_back( wrap_string( path ) );
        }

        cxx_commands.push_back( options::m_c_tab.m_command_line );

        std::string final_cxx_command;
        for ( const auto& command : cxx_commands )
            final_cxx_command += command + " ";

        replace_sequence( final_cxx_command, "$(output)", output_filename );

        std::filesystem::path current_dir = std::filesystem::current_path( );
        std::vector< std::filesystem::path > files;

        for ( const auto& entry : std::filesystem::recursive_directory_iterator( current_dir ) )
        {
            if ( entry.is_regular_file( ) )
            {
                std::string extension = entry.path( ).extension( ).string( );
                if ( ( extension == ".cpp" || extension == ".c" ) && std::find( excluded_files.begin( ), excluded_files.end( ), entry.path( ).filename( ).string( ) ) == excluded_files.end( ) )
                    files.push_back( entry.path( ).lexically_relative( current_dir ) );
            }
        }

        std::vector< std::thread > threads;

        std::string intermediate_dir;
        if ( options::m_intermediate_dir[ 0 ] )
            intermediate_dir = options::m_intermediate_dir;
        else
            intermediate_dir = "pivo-build";

        std::filesystem::create_directory( intermediate_dir );

        for ( auto& file : files )
        {
            std::string command = final_cxx_command + ( file.extension( ) == ".cpp" ? cxx_standard : c_standard ) + " " + file.string( ) + " -o " + intermediate_dir + "/" + file.filename( ).replace_extension( ".o" ).string( );
    
            if ( multithread )
                threads.push_back( std::thread( [ ]( std::string command ){ run_process( command ); }, command ) );
            else
                run_process( command );
        }

        for ( auto& thread : threads )
            thread.join( );

        std::string final_linker_command;
        for ( const auto& command : linker_commands )
            final_linker_command += command + " ";

        replace_sequence( final_linker_command, "$(output)", output_filename );

        for ( auto& file : files )
        {
            file = file.filename( ).replace_extension( ".o" );

            if ( !std::filesystem::exists( intermediate_dir + "/" + file.string( ) ) )
            {
                std::cout << "Failed to compile " << file.string( ) << std::endl;
                return false;
            }

            final_linker_command += " " + intermediate_dir + "/" + file.string( );
        }

        run_process( final_linker_command );

        if ( !std::filesystem::exists( output_filename ) )
        {
            std::cout << "Failed to link\n";
            return false;
        }

        std::cout << "Compiled\n";

        return true;
    }
}