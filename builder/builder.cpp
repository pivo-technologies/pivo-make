#include "builder.h"
#include "../options/options.h"

#include <iostream>
#include <filesystem>
#include <thread>

namespace builder
{
    std::string replace_symbol_with_sequence( std::string str, char symbol, const std::string& sequence ) 
    {
        size_t pos = 0;
        while ( ( pos = str.find( symbol, pos ) ) != std::string::npos ) 
        {
            str.replace( pos, 1, sequence );
            pos += sequence.length( );
        }

        return str;
    }

    bool build( )
    {
        bool multithread = false;

        std::vector< std::string > cxx_commands;
        std::vector< std::string > linker_commands;

        cxx_commands.push_back( "clang++" );

        std::string cxx_standard;
        std::string c_standard;

        if ( options::m_compiler == 1 )
        {
            std::cout << "Builing Windows binary (COFF)\n";


        }
        else
        {
            std::cout << "Building Linux binary (ELF)\n";
            
            linker_commands.push_back( "clang++" );

            cxx_commands.push_back( "-c" );

            for ( auto& sub_tab : options::m_c_tab.m_subtabs )
            {
                for ( auto& option : sub_tab.m_options )
                {
                    if ( option.m_is_single_line )
                    {
                        if ( option.m_params.empty( ) )
                            continue;

                        if ( option.m_name == "Additional Include Directories" )
                        {
                            cxx_commands.push_back( "-I" );
                            cxx_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', " -I" ) );
                        }

                        if ( option.m_name.find( "Additional Warnings" ) != std::string::npos )
                        {
                            cxx_commands.push_back( "-W\"" );
                            cxx_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', "\" -W\"" ) + std::string( "\"" ) );
                        }

                        if ( option.m_name == "Preprocessor Definitions" )
                        {
                            cxx_commands.push_back( "-D \"" );
                            cxx_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', "\" -D\"" ) + std::string( "\"" ) );                        
                        }

                        if ( option.m_name == "Undefine Preprocessor Definitions" )
                        {
                            cxx_commands.push_back( "-U \"" );
                            cxx_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', "\" -U\"" ) + std::string( "\"" ) );     
                        }

                        if ( option.m_name == "Force Include File" )
                        {
                            cxx_commands.push_back( "-include \"" );
                            cxx_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', "\" -include \"" ) + std::string( "\"" ) );                        
                        }
                    }
                    else
                    {
                        const std::string& command_line = option.m_choices[ option.m_current_choice ].m_cmd_line;
                        if ( !multithread && command_line == "/MP" )
                        {
                            multithread = true;
                            std::cout << "/MP argument found, files will be compiled in parallel\n";
                        }
                        else if ( !command_line.empty( ) )
                        {
                            if ( option.m_name.find( "C++ Language Standard" ) != std::string::npos )
                                cxx_standard = command_line;
                            else if ( option.m_name.find( "C Language Standard" ) != std::string::npos )
                                c_standard = command_line;
                            else
                                cxx_commands.push_back( command_line );
                        }
                    }
                }
            }

            cxx_commands.push_back( options::m_c_tab.m_command_line );

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
                            linker_commands.push_back( ( option.m_params.empty( ) || options::m_output_file[ 0 ] ) ? options::m_output_file : option.m_params.data( ) );
                        }

                        if ( option.m_name == "Trace Symbols" )
                        {
                            linker_commands.push_back( "-Wl,--trace-symbol" );
                            linker_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', " -Wl,--trace-symbol" ) );
                        }

                        if ( option.m_name == "Additional Library Directories" )
                        {
                            linker_commands.push_back( "-L" );
                            linker_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', " -L" ) );
                        }

                        if ( option.m_name == "Ignore Specific Default Libraries" )
                        {
                            linker_commands.push_back( "-Wl,--exclude-libs" );
                            linker_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', "," ) );
                        }

                        if ( option.m_name == "Force Undefined Symbol References" )
                        {
                            linker_commands.push_back( "-Wl,--undefined" );
                            linker_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', " -Wl,--undefined" ) );
                        }

                        if ( option.m_name == "Library Dependencies" )
                        {
                            linker_commands.push_back( "-l" );
                            linker_commands.push_back( replace_symbol_with_sequence( option.m_params, ';', " -l" ) );
                        }

                        if ( option.m_name == "Additional Dependencies" )
                            linker_commands.push_back( option.m_params );

                        if ( option.m_name == "Map File Name" )
                            linker_commands.push_back( "-Wl,-Map=" + option.m_params );                        
                    }
                    else
                    {
                        const std::string& command_line = option.m_choices[ option.m_current_choice ].m_cmd_line;
                        if ( !command_line.empty( ) )
                            linker_commands.push_back( command_line );
                    }
                }
            }

            linker_commands.push_back( options::m_linker_tab.m_command_line );
        }

        std::string final_cxx_command;
        for ( const auto& command : cxx_commands )
            final_cxx_command += command + " ";

        std::filesystem::path current_dir = std::filesystem::current_path( );
        std::vector< std::filesystem::path > files;

        for ( const auto& entry : std::filesystem::recursive_directory_iterator( current_dir ) )
        {
            if ( entry.is_regular_file( ) )
            {
                std::string extension = entry.path( ).extension( ).string( );
                if ( extension == ".cpp" || extension == ".c" )
                    files.push_back( entry.path( ).lexically_relative( current_dir ) );
            }
        }

        std::vector< std::thread > threads;

        std::filesystem::create_directory( "pivo-build" );

        for ( auto& file : files )
        {
            std::string command = final_cxx_command + ( file.extension( ) == ".cpp" ? cxx_standard : c_standard ) + " " + file.string( ) + " -o " + "pivo-build/" + file.filename( ).replace_extension( ".o" ).string( );

            if ( multithread )
                threads.push_back( std::thread( [ ]( std::string command ){ system( command.c_str( ) ); }, command ) );
            else
                system( command.c_str( ) );
        }

        for ( auto& thread : threads )
            thread.join( );

        std::string final_linker_command;
        for ( const auto& command : linker_commands )
            final_linker_command += command + " ";

        for ( auto& file : files )
        {
            file = file.filename( ).replace_extension( ".o" );

            if ( !std::filesystem::exists( "pivo-build/" + file.string( ) ) )
            {
                std::cout << "Failed to compile " << file.string( ) << std::endl;
                return false;
            }

            final_linker_command += " pivo-build/" + file.string( );
        }

        system( final_linker_command.c_str( ) );

        std::cout << "Compiled\n";

        return true;
    }
}