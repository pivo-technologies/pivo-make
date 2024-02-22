#include "options.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>

namespace options
{
    void init( )
    {
        m_c_tab.m_name = "C/C++";
        m_linker_tab.m_name = "Linker"; 
        m_c_tab.m_subtabs.clear( );
        m_linker_tab.m_subtabs.clear( );

        std::memset( m_c_tab.m_command_line, 0, sizeof( m_c_tab.m_command_line ) );
        std::memset( m_linker_tab.m_command_line, 0, sizeof( m_linker_tab.m_command_line ) );

        {
            // c/c++
            subtab_t general;
            general.m_name = "General##c";

            option_t debug_info_format[ 2 ] = {
                option_t( "Debug Information Format", choice_t( "None", "-g0" ), choice_t( "Minimal Debug Information", "-g1" ), choice_t( "Full Debug Information (DWARF2)", "-g2 -gdwarf-2" ) ),
                option_t( "Debug Information Format", choice_t( "None" ), choice_t( "Yes", "-gcodeview" ) )
            };

            general.add_options( 
                option_t( true, "Additional Include Directories" ),
                option_t( true, "Exclude Files From Build" ),
                debug_info_format[ m_compiler ],
                option_t( "Just My Code Debugging", choice_t( "No" ), choice_t( "Yes", "-fjmc" ) ),
                option_t( "Warning Level", choice_t( "Level 1", "-Wall" ), choice_t( "Level 2", "-Wall -Wextra" ), choice_t( "Level 3", "-Weverything" ), choice_t( "Disable All", "-w" ) ),
                option_t( "Treat Warnings as Errors", choice_t( "No" ), choice_t( "Yes", "-Werror" ) ),
                option_t( true, "C Additional Warnings" ),
                option_t( true, "C++ Additional Warnings" ),
                option_t( "Enable Verbose Mode", choice_t( "No" ), choice_t( "Yes", "-v" ) ),
                option_t( "Multi-processor Compilation", choice_t( "No" ), choice_t( "Yes", "/MP" ) ),
                option_t( "Enable Address Sanitizer", choice_t( "No" ), choice_t( "Yes", "-fsanitize=address" ) )
            );

            m_c_tab.m_subtabs.push_back( general );

            subtab_t optimization;
            optimization.m_name = "Optimization##c";

            optimization.add_options( 
                option_t( "Optimization Level", choice_t( "None", "-O0" ), choice_t( "Minimize Size", "-Os" ), choice_t( "Maximize Speed", "-O2" ), choice_t( "Full Optimization", "-O3" ) ),
                option_t( "Strict Aliasing", choice_t( "No", "-fno-strict-aliasing" ), choice_t( "Yes", "-fstrict-aliasing" ) ),
                option_t( "Unroll Loops", choice_t( "No" ), choice_t( "Yes", "-funroll-loops" ) ),
                option_t( "Link Time Optimization", choice_t( "No" ), choice_t( "Yes", "-flto" ) ),
                option_t( "Omit Frame Pointers", choice_t( "No", "-fno-omit-frame-pointer" ), choice_t( "Yes", "-fomit-frame-pointer" ) )
            );

            m_c_tab.m_subtabs.push_back( optimization );

            subtab_t preprocessor;
            preprocessor.m_name = "Preprocessor##c";

            preprocessor.add_options( 
                    option_t( true, "Preprocessor Definitions" ),
                    option_t( true, "Undefine Preprocessor Definitions" ),
                    option_t( "Undefine All Preprocessor Definitions", choice_t( "No" ), choice_t( "Yes", "-undef" ) ),
                    option_t( "Show All Includes", choice_t( "No" ), choice_t( "Yes", "-H" ) )
            );

            m_c_tab.m_subtabs.push_back( preprocessor );

            subtab_t code_generation;
            code_generation.m_name = "Code Generation##c";

            option_t runtime_lib[ 2 ] = {
                option_t( "Run-Time Library" ),
                option_t( "Run-Time Library", choice_t( "Multi-threaded", "-fms-runtime-lib=static" ), choice_t( "Multi-threaded Debug", "-fms-runtime-lib=static_dbg" ), choice_t( "Multi-threaded DLL", "-fms-runtime-lib=dll" ), choice_t( "Multi-threaded Debug DLL", "-fms-runtime-lib=dll_dbg" ) )
            };

            code_generation.add_options( 
                option_t( "Enable C++ Exceptions", choice_t( "Yes", "-fexceptions" ), choice_t( "No", "-fno-exceptions" ) ),
                runtime_lib[ m_compiler ],
                option_t( "Struct Member Alignment", choice_t( "Default (8)", "-fpack-struct=8" ), choice_t( "1 Byte", "-fpack-struct=1" ), choice_t( "2 Bytes", "-fpack-struct=2" ), choice_t( "4 Bytes", "-fpack-strut=4" ), choice_t( "16 Bytes", "-fpack-struct=16" ) ),
                option_t( "Security Check", choice_t( "Yes", "-fstack-protector" ), choice_t( "Strong", "-fstack-protector-strong" ), choice_t( "All", "-fstack-protector-all" ), choice_t( "No", "-fno-stack-protector" ) ),
                //option_t( "Control Flow Guard", choice_t( "Yes", "-mguard=cf" ), choice_t( "Without Checks", "-mguard=cf-nochecks" ), choice_t( "No", "-mguard=none" ) ),
                option_t( "Enable Function-Level Linking", choice_t( "Yes", "-ffunction-sections" ), choice_t( "No", "-fno-function-sections" ) ),
                option_t( "Enable Enhanced Instruction Set", choice_t( "No" ), choice_t( "SSE", "-msse" ), choice_t( "SSE2", "-msse2" ), choice_t( "AVX", "-mavx" ), choice_t( "AVX2", "-mavx2" ), choice_t( "AVX512", "-mavx512" ) ),
                option_t( "Floating Point Model", choice_t( "Precise", "-ffp-model=precise" ), choice_t( "Strict", "-ffp-model=strict" ), choice_t( "Fast", "-ffp-model=fast" ) )
            );

            m_c_tab.m_subtabs.push_back( code_generation );

            subtab_t language;
            language.m_name = "Language##c";

            language.add_options(
                option_t( "Enable Run-Time Type Information", choice_t( "Yes", "-frtti" ), choice_t( "No", "-fno-rtti" ) ),
                option_t( "C++ Language Standard", choice_t( "Default", "-std=c++14" ), choice_t( "C++14", "-std=c++14" ), choice_t( "C++17", "-std=c++17" ), choice_t( "C++20", "-std=c++20" ) ),
                option_t( "C Language Standard", choice_t( "Default", "-std=c11" ), choice_t( "C11", "-std=c11" ), choice_t( "C17", "-std=c17" ) )
            );

            m_c_tab.m_subtabs.push_back( language );

            subtab_t precompiled_headers;
            precompiled_headers.m_name = "Precompiled Headers##c";

            precompiled_headers.add_options( option_t( true, "Precompiled Header File" ) );

            m_c_tab.m_subtabs.push_back( precompiled_headers );

            subtab_t advanced;
            advanced.m_name = "Advanced##c";

            advanced.add_options(
                option_t( "Compile As", choice_t( "Default" ), choice_t( "C++ Code", "-x c++" ), choice_t( "C Code", "-x c" ) ),
                option_t( true, "Force Include File" )
            );

            m_c_tab.m_subtabs.push_back( advanced );

            subtab_t all_options;
            all_options.m_name = "All Options##c";

            m_c_tab.m_subtabs.push_back( all_options );

            subtab_t command_line;
            command_line.m_name = "Command Line##c";

            m_c_tab.m_subtabs.push_back( command_line );
        }

        if ( m_compiler == 1 )
        {
            // linker
            {
                subtab_t general;
                general.m_name = "General##linker";

                general.add_options(
                    option_t( true, "Output File" ),
                    option_t( true, "Version" ),
                    option_t( "Enable Incremental Linking", choice_t( "Yes", "/incremental" ), choice_t( "No", "/incremental:NO" ) ),
                    option_t( "Ignore Import Library", choice_t( "No" ), choice_t( "Yes", "/ignore:LIB" ) ),
                    option_t( true, "Additional Library Directories" ),
                    option_t( "Prevent Dll Binding", choice_t( "##prevent_dll_binding" ), choice_t( "No", "/allowbind:NO" ), choice_t( "Yes", "/allowbind" ) ),
                    option_t( "Treat Linker Warnings As Errors", choice_t( "No", "/WX:NO" ), choice_t( "Yes", "/WX" ) ),
                    option_t( "Force File Output", choice_t( "No" ), choice_t( "Enabled", "/force" ), choice_t( "Multiple Defined Symbols Only", "/force:multiple" ), choice_t( "Undefined Symbols Only", "/force:unresolved" ) ),
                    option_t( true, "Specify Section Attributes" )
                );

                m_linker_tab.m_subtabs.push_back( general );

                subtab_t input;
                input.m_name = "Input##linker";

                input.add_options(
                    option_t( true, "Additional Dependencies" ),
                    option_t( "Ignore All Default Libraries", choice_t( "No" ), choice_t( "Yes", "/nodefaultlib" ) ),
                    option_t( true, "Ignore Specific Default Libraries" )
                );

                m_linker_tab.m_subtabs.push_back( input );

                subtab_t manifest_file;
                manifest_file.m_name = "Manifest File##linker";

                manifest_file.add_options(
                    option_t( "Generate Manifest", choice_t( "No", "/manifest:NO" ), choice_t( "Yes", "/manifest" ) ),
                    option_t( "Enable User Account Control", choice_t( "No", "/manifestuac:NO" ), choice_t( "Yes" ) ),
                    option_t( "UAC Execution Level", choice_t( "As Invoker", "/manifestuac:\"level='asInvoker'" ), choice_t( "Highest Available", "/manifestuac:\"level='highestAvailable'" ), choice_t( "Require Administrator", "/manifestuac:level=\"'requireAdministrator'" ) ),
                    option_t( "UAC Bypass UI Protection", choice_t( "No", "uiAccess='false'\"" ), choice_t( "Yes", "uiAccess='true'\"" ) )
                );

                m_linker_tab.m_subtabs.push_back( manifest_file );

                subtab_t debug;
                debug.m_name = "Debugging##linker";

                debug.add_options(
                    option_t( "Generate Debug Info", choice_t( "None", "/debug:NONE" ), choice_t( "Yes", "/debug" ), choice_t( "Optimized for faster links", "/debug:FASTLINK" ), choice_t( "Optimized for sharing and publishing", "/debug:FULL" ) ),
                    option_t( true, "Generate Program Database File" ),
                    option_t( true, "Map File" )
                );

                m_linker_tab.m_subtabs.push_back( debug );

                subtab_t system;
                system.m_name = "System##linker";

                system.add_options(
                    option_t( "SubSystem", choice_t( "Console", "/subsystem:CONSOLE" ), choice_t( "Windows", "/subsystem:WINDOWS" ), choice_t( "Native", "/subsystem:NATIVE" ), choice_t( "EFI Application", "/subsystem:EFI_APPLICATION" ), choice_t( "EFI Boot Service Driver", "/subsystem:EFI_BOOT_SERVICE_DRIVER" ), choice_t( "EFI ROM", "/subsystem:EFI_ROM" ), choice_t( "EFI Runtime Driver", "/subsystem:EFI_RUNTIME_DRIVER" ) ),
                    option_t( true, "Heap Size (reserve,commit)" ),
                    option_t( true, "Stack Size (reserve,commit)" ),
                    option_t( "Large Address Aware", choice_t( "Default" ), choice_t( "Yes", "/largeaddressaware" ), choice_t( "/largeaddressaware:no" ) ),
                    option_t( "Driver", choice_t( "Not set" ), choice_t( "Driver", "/driver" ), choice_t( "UP Only", "/driver:uponly" ), choice_t( "WDM", "/driver:wdm" ) )
                );

                m_linker_tab.m_subtabs.push_back( system );

                subtab_t optimization;
                optimization.m_name = "Optimization##linker";

                optimization.add_options(
                    option_t( "Optimize References", choice_t( "No", "/opt:noref" ), choice_t( "Yes", "/opt:ref" ) ),
                    option_t( "Enable COMDAT Folding", choice_t( "No", "/opt:noicf" ), choice_t( "Yes", "/opt:icf" ) ),
                    option_t( true, "Function Order" )
                );

                m_linker_tab.m_subtabs.push_back( optimization );

                subtab_t advanced;
                advanced.m_name = "Advanced##linker";

                advanced.add_options( 
                    option_t( true, "Entry Point" ),
                    option_t( "No Entry Point", choice_t( "No" ), choice_t( "Yes", "/noentry" ) ),
                    option_t( true, "Base Address" ),
                    option_t( "Randomized Base Address", choice_t( "Yes", "/dynamicbase" ), choice_t( "No", "/dynamicbase:no" ) ),
                    option_t( "Fixed Base Address", choice_t( "No", "/fixed:no" ), choice_t( "Yes", "/fixed" ) ),
                    option_t( "Data Execution Prevention (DEP)", choice_t( "Yes", "/nxcompat" ), choice_t( "No", "/nxcompat:no" ) ),
                    option_t( true, "Import Library" ),
                    option_t( true, "Merge Sections" ),
                    option_t( "Target Machine", choice_t( "Not Set" ), choice_t( "x86", "/machine:x86" ), choice_t( "x64", "/machine:x64" ), choice_t( "ARM", "/machine:arm" ), choice_t( "ARM64", "/machine:arm64" ) )
                );

                m_linker_tab.m_subtabs.push_back( advanced );

                subtab_t all_options;
                all_options.m_name = "All Options##linker";

                m_linker_tab.m_subtabs.push_back( all_options );

                subtab_t command_line;
                command_line.m_name = "Command Line##linker";

                m_linker_tab.m_subtabs.push_back( command_line );
            }
        }
        else
        {
            // linker
            {
                subtab_t general;
                general.m_name = "General##linker";

                general.add_options( 
                    option_t( true, "Output File" ),
                    option_t( "Show Progress", choice_t( "No" ), choice_t( "Yes", "-Wl,--stats" ) ),
                    option_t( "Version", choice_t( "No" ), choice_t( "Yes", "-Wl,--version" ) ),
                    option_t( "Enable Verbose Output", choice_t( "No" ), choice_t( "Yes", "-Wl,--verbose" ) ),
                    option_t( "Trace", choice_t( "No" ), choice_t( "Yes", "-Wl,--trace" ) ),
                    option_t( true, "Trace Symbols" ), 
                    option_t( "Print Map", choice_t( "No" ), choice_t( "Yes", "-Wl,--print-map" ) ),
                    option_t( "Report Unresolved Symbol References", choice_t( "Yes", "-Wl,--no-undefined" ), choice_t( "No" ) ),
                    option_t( "Optimize For Memory Usage", choice_t( "No" ), choice_t( "Yes", "-Wl,--no-keep-memory" ) ),
                    option_t( true, "Additional Library Directories" )
                );

                m_linker_tab.m_subtabs.push_back( general );

                subtab_t input;
                input.m_name = "Input##linker";

                input.add_options( 
                    option_t( true, "Ignore Specific Default Libraries" ),
                    option_t( "Ignore Default Libraries", choice_t( "No" ), choice_t( "Yes", "-Wl,-nostdlib" ) ),
                    option_t( true, "Force Undefined Symbol References" ),
                    option_t( true, "Library Dependencies" ),
                    option_t( true, "Additional Dependencies" )
                );

                m_linker_tab.m_subtabs.push_back( input );

                subtab_t debug;
                debug.m_name = "Debugging##linker";

                debug.add_options(
                    option_t( "Debugger Symbol Information", choice_t( "Include All" ), choice_t( "Omit Debugger Symbol Information", "-Wl,--strip-debug" ), choice_t( "Omit All Symbol Information", "-Wl,--strip-all" ) ),
                    option_t( true, "Map File Name" )
                );

                m_linker_tab.m_subtabs.push_back( debug );

                subtab_t advanced;
                advanced.m_name = "Advanced##linker";

                advanced.add_options( 
                    option_t( "Mark Variables ReadOnly After Relocation", choice_t( "Yes", "-Wl,-z,relro" ), choice_t( "No", "-Wl,-z,norelro" ) ),
                    option_t( "Enable Immediate Function Binding", choice_t( "Yes", "-Wl,-z,now" ), choice_t( "No" ) ),
                    option_t( "Executable Stack Not Required", choice_t( "Yes", "-Wl,-z,noexecstack" ), choice_t( "No" ) ),
                    option_t( "Whole Archive", choice_t( "No" ), choice_t( "Yes", "-Wl,--whole-archive" ) )
                );

                m_linker_tab.m_subtabs.push_back( advanced );

                subtab_t all_options;
                all_options.m_name = "All Options##linker";

                m_linker_tab.m_subtabs.push_back( all_options );

                subtab_t command_line;
                command_line.m_name = "Command Line##linker";

                m_linker_tab.m_subtabs.push_back( command_line );
            }
        }
    }

    bool open( )
    {
        std::string config_filename = m_config_file;
        std::string ending = ".pivo";
        if ( !std::equal( ending.rbegin( ), ending.rend( ), config_filename.rbegin( ) ) )
            config_filename += ".pivo";

        std::ifstream file( config_filename );

        if ( !file.is_open( ) || !file.good( ) )
        {
            std::cout << "Failed to open file " << config_filename << std::endl;
            return false;
        }

        nlohmann::json json;
        file >> json;

        file.close( );

        int compiler = json[ "compiler" ].get< int >( );

        if ( compiler != m_compiler )
        {
            m_compiler = compiler;
            init( );
        }

        tab_t* tabs[ ] = { &m_c_tab, &m_linker_tab };

        for ( const auto& tab : tabs )
        {
            if ( json.find( tab->m_name ) == json.end( ) )
                continue;

            if ( json[ tab->m_name ].find( "command_line" ) != json[ tab->m_name ].end( ) )
                std::strcpy( tab->m_command_line, json[ tab->m_name ][ "command_line" ].get< std::string >( ).c_str( ) );

            for ( auto& sub_tab : tab->m_subtabs )
            {
                if ( json[ tab->m_name ].find( sub_tab.m_name ) == json[ tab->m_name ].end( ) )
                    continue;

                for ( auto& option : sub_tab.m_options )
                {
                    if ( json[ tab->m_name ][ sub_tab.m_name ].find( option.m_name ) == json[ tab->m_name ][ sub_tab.m_name ].end( ) )
                        continue;                    

                    if ( !option.m_is_single_line )
                    {
                        if ( json[ tab->m_name ][ sub_tab.m_name ][ option.m_name ].find( "choice" ) != json[ tab->m_name ][ sub_tab.m_name ][ option.m_name ].end( ) )
                            option.m_current_choice = json[ tab->m_name ][ sub_tab.m_name ][ option.m_name ][ "choice" ].get< int >( );
                        else
                            option.m_current_choice = 0;
                    }
                    else
                    {
                        if ( json[ tab->m_name ][ sub_tab.m_name ][ option.m_name ].find( "params" ) != json[ tab->m_name ][ sub_tab.m_name ][ option.m_name ].end( ) )
                            option.m_params = json[ tab->m_name ][ sub_tab.m_name ][ option.m_name ][ "params" ].get< std::string >( );
                        else
                            option.m_params = "";
                    }
                }
            }
        }

        return true;
    }

    void save( )
    {
        std::string config_filename = m_config_file;
        std::string ending = ".pivo";
        if ( !std::equal( ending.rbegin( ), ending.rend( ), config_filename.rbegin( ) ) )
            config_filename += ".pivo";

        std::ofstream file( config_filename );

        if ( !file.is_open( ) || !file.good( ) )
        {
            std::cout << "Failed to open file " << config_filename << std::endl;
            return;
        }

        nlohmann::json json;

        json[ "compiler" ] = m_compiler;

        tab_t* tabs[ ] = { &m_c_tab, &m_linker_tab };

        for ( const auto& tab : tabs )
        {
            json[ tab->m_name ][ "command_line" ] = std::string( tab->m_command_line );

            for ( const auto& sub_tab : tab->m_subtabs )
            {
                for ( const auto& option : sub_tab.m_options )
                {
                    if ( !option.m_is_single_line )
                        json[ tab->m_name ][ sub_tab.m_name ][ option.m_name ][ "choice" ] = option.m_current_choice;
                    else
                        json[ tab->m_name ][ sub_tab.m_name ][ option.m_name ][ "params" ] = option.m_params;
                }
            }
        }

        file << std::setw( 4 ) << json;

        file.close( );
    }

    void load_defaults_debug( )
    {
        init( );

        std::memset( m_c_tab.m_command_line, 0, sizeof( m_c_tab.m_command_line ) );
        std::memset( m_linker_tab.m_command_line, 0, sizeof( m_linker_tab.m_command_line ) );

        m_c_tab[ "General##c" ][ "Debug Information Format" ].set_choice( m_compiler == 1 ? "Yes" : "Full Debug Information (DWARF2)" );
        m_c_tab[ "General##c" ][ "Multi-processor Compilation" ].set_choice( "Yes" );

        if ( m_compiler == 1 )
        {            
            m_c_tab[ "General##c" ][ "Just My Code Debugging" ].set_choice( "Yes" );
            m_c_tab[ "Preprocessor##c" ][ "Preprocessor Definitions" ].set_params( "_DEBUG; _WIN32; _WINDOWS; _UNICODE; UNICODE" );
            m_c_tab[ "Code Generation##c" ][ "Run-Time Library" ].set_choice( "Multi-threaded Debug DLL" );
            m_c_tab[ "Code Generation##c" ][ "Enable Function-Level Linking" ].set_choice( "No" );

            m_linker_tab[ "Input##linker" ][ "Additional Dependencies" ].set_params( "kernel32.lib; user32.lib; gdi32.lib; winspool.lib; comdlg32.lib; advapi32.lib; shell32.lib; ole32.lib; oleaut32.lib; uuid.lib; odbc32.lib; odbccp32.lib" );
            m_linker_tab[ "Debugging##linker" ][ "Generate Debug Info" ].set_choice( "Yes" );
            m_linker_tab[ "Debugging##linker" ][ "Generate Program Database File" ].set_params( "$(output).pdb" );
        }
        else
        {
            m_c_tab[ "General##c" ][ "C Additional Warnings" ].set_params( "switch; no-deprecated-declarations; empty-body; conversion; return-type; parentheses; no-pointer-sign; no-format; uninitialized; unreachable-code; unused-function; unused-value; unused-variable" );
            m_c_tab[ "General##c" ][ "C++ Additional Warnings" ].set_params( "switch; no-deprecated-declarations; empty-body; conversion; return-type; parentheses; no-pointer-sign; no-format; uninitialized; unreachable-code; unused-function; unused-value; unused-variable" );
        }
    }

    void load_defaults_release( )
    {
        init( );

        std::memset( m_c_tab.m_command_line, 0, sizeof( m_c_tab.m_command_line ) );
        std::memset( m_linker_tab.m_command_line, 0, sizeof( m_linker_tab.m_command_line ) );
        
        m_c_tab[ "General##c" ][ "Debug Information Format" ].set_choice( m_compiler == 1 ? "Yes" : "Minimal Debug Information" );
        m_c_tab[ "General##c" ][ "Multi-processor Compilation" ].set_choice( "Yes" );    
        m_c_tab[ "Optimization##c" ][ "Optimization Level" ].set_choice( "Full Optimization" );
        m_c_tab[ "Optimization##c" ][ "Omit Frame Pointers" ].set_choice( "Yes" );

        if ( m_compiler == 1 )
        {
            m_c_tab[ "Preprocessor##c" ][ "Preprocessor Definitions" ].set_params( "NDEBUG; _WIN32; _WINDOWS; _UNICODE; UNICODE" );
            m_c_tab[ "Code Generation##c" ][ "Run-Time Library" ].set_choice( "Multi-threaded DLL" );
            m_c_tab[ "Code Generation##c" ][ "Enable Function-Level Linking" ].set_choice( "Yes" );

            m_linker_tab[ "General##linker" ][ "Enable Incremental Linking" ].set_choice( "No" );
            m_linker_tab[ "Debugging##linker" ][ "Generate Debug Info" ].set_choice( "Yes" );
            m_linker_tab[ "Debugging##linker" ][ "Generate Program Database File" ].set_params( "$(output).pdb" );
            m_linker_tab[ "Debugging##linker" ][ "Map File" ].set_params( "$(output).map" );
            m_linker_tab[ "Optimization##linker" ][ "Optimize References" ].set_choice( "Yes" );
            m_linker_tab[ "Optimization##linker" ][ "Enable COMDAT Folding" ].set_choice( "Yes" );
        }
        else
        {
            m_c_tab[ "General##c" ][ "C Additional Warnings" ].set_params( "switch; no-deprecated-declarations; empty-body; conversion; return-type; parentheses; no-pointer-sign; no-format; uninitialized; unreachable-code; unused-function; unused-value; unused-variable" );
            m_c_tab[ "General##c" ][ "C++ Additional Warnings" ].set_params( "switch; no-deprecated-declarations; empty-body; conversion; return-type; parentheses; no-pointer-sign; no-format; uninitialized; unreachable-code; unused-function; unused-value; unused-variable" );
            m_c_tab[ "Preprocessor##c" ][ "Preprocessor Definitions" ].set_params( "NDEBUG" );
        }
    }
}