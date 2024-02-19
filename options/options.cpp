#include "options.h"

#include <cstdio>
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

        if ( m_compiler == 1 )
        {
            // c/c++
            {
                subtab_t general;
                general.m_name = "General##c";
                general.add_options( 
                    option_t( true, "Additional Include Directories" ),
                    option_t( "Debug information format", choice_t( "None" ), choice_t( "C7 compatible", "/Z7" ), choice_t( "Program Database", "/Zi" ), choice_t( "Program Database for Edit and Continue", "/ZI" ) ),
                    option_t( "Support Just My Code Debugging", choice_t( "No", "/JMC-" ), choice_t( "Yes", "/JMC" ) ),
                    option_t( "Warning Level", choice_t( "Turn off all warnings", "/W0" ), choice_t( "Level 1", "/W1" ), choice_t( "Level 2", "/W2" ), choice_t( "Level 3", "/W3" ), choice_t( "Level 4", "/W4" ) ),
                    option_t( "Treat Warnings as Errors", choice_t( "No", "/WX-" ), choice_t( "Yes", "/WX" ) ),
                    //option_t( "Warning Version", choice_t( "##warning_version_label" ), choice_t( "Default", "/Wv:18" ), choice_t( "14", "/Wv:14" ), choice_t( "17", "/Wv:17" ), choice_t( "19", "/Wv:19" ) ),
                    option_t( "Diagnostics Format", choice_t( "Column", "/diagnostics:column" ), choice_t( "Caret", "/diagnostics:caret" ), choice_t( "Classic", "/diagnostics:classic" ) ),
                    option_t( "Multi-processor Compilation", choice_t( "No" ), choice_t( "Yes", "/MP" ) ),
                    option_t( "Enable Address Sanitizer", choice_t( "No" ), choice_t( "Yes", "/fsanitize=address" ) )
                );

                m_c_tab.m_subtabs.push_back( general );

                subtab_t optimization;
                optimization.m_name = "Optimization##c";

                optimization.add_options(
                    option_t( "Optimization", choice_t( "Disabled", "/Od" ), choice_t( "Minimize Size", "/Os" ), choice_t( "Maximize Speed", "/O2" ), choice_t( "Optimizations", "/Ox" ) ),
                    option_t( "Inline Function Expansion", choice_t( "Default", "/Ob2" ), choice_t( "Only __inline", "/Ob1" ), choice_t( "Any Suitable", "/Ob2" ) ),
                    option_t( "Enable Intrinsic Functions", choice_t( "No" ), choice_t( "Yes", "/Oi" ) ),
                    option_t( "Favor Size Or Speed", choice_t( "Favor fast code", "/Ot" ), choice_t( "Neither" ), choice_t( "Favor small code", "/Os" ) ),
                    option_t( "Omit Frame Pointers", choice_t( "No", "/Oy-" ), choice_t( "Yes", "/Oy" ) )//,
                    //option_t( "Enable Fiber-Safe Optimizations", choice_t( "No" ), choice_t( "Yes", "/GT" ) )
                );

                m_c_tab.m_subtabs.push_back( optimization );

                subtab_t preprocessor;
                preprocessor.m_name = "Preprocessor##c";

                preprocessor.add_options( 
                    option_t( true, "Preprocessor Definitions" ),
                    option_t( true, "Undefine Preprocessor Definitions" ),
                    option_t( "Undefine All Preprocessor Definitions", choice_t( "No" ), choice_t( "Yes", "/u" ) ),
                    option_t( "Preprocess to a File", choice_t( "No" ), choice_t( "Yes", "/P" ) ),
                    option_t( "Preprocess Suppress Line Numbers", choice_t( "No" ), choice_t( "Yes", "/EP" ) ),
                    option_t( "Keep Comments", choice_t( "No" ), choice_t( "Yes", "/C" ) )
                );

                m_c_tab.m_subtabs.push_back( preprocessor );

                subtab_t code_generation;
                code_generation.m_name = "Code Generation##c";

                code_generation.add_options(
                    option_t( "Enable String Pooling", choice_t( "No", "/GF-" ), choice_t( "Yes", "/GF" ) ),
                    option_t( "Enable C++ Exceptions", choice_t( "Yes", "/EHsc" ), choice_t( "No", "/EHsc-" ) ),
                    //option_t( "Smaller Type Check", choice_t( "No" ), choice_t( "Yes", "/RTCc" ) ),
                    option_t( "Runtime Library", choice_t( "Multi-threaded Debug DLL", "/MDd" ), choice_t( "Multi-threaded DLL", "/MD" ), choice_t( "Multi-threaded Debug", "/MTd" ), choice_t( "Multi-threaded", "/MT" ) ),
                    option_t( "Struct Member Alignment", choice_t( "Default", "/Zp8" ), choice_t( "1 Byte", "/Zp1" ), choice_t( "2 Bytes", "/Zp2" ), choice_t( "4 Bytes", "/Zp4" ), choice_t( "8 Bytes", "/Zp8" ), choice_t( "16 Bytes", "/Zp16" ) ),
                    option_t( "Security Check", choice_t( "No", "/GS-" ), choice_t( "Yes", "/GS" ) ),
                    option_t( "Control Flow Guard", choice_t( "No", "/guard:cf-" ), choice_t( "Yes", "/guard:cf" ) ),
                    option_t( "Enable Function-Level Linking", choice_t( "No", "/Gy-" ), choice_t( "Yes", "/Gy" ) ),
                    option_t( "Enable Enhanced Instruction Set", choice_t( "Not Set" ), choice_t( "No Enhanced Instructions", "/arch:IA32" ), choice_t( "SSE", "/arch:SSE" ), choice_t( "SSE2", "/arch:SSE2" ), choice_t( "AVX", "/arch:AVX" ), choice_t( "AVX2", "/arch:AVX2" ), choice_t( "AVX512", "/arch:AVX512" ) ),
                    option_t( "Floating Point Model", choice_t( "Precise", "/fp:precise" ), choice_t( "Strict", "/fp:strict" ), choice_t( "Fast", "/fp:fast" ) ),
                    option_t( "Enable Floating Point Exceptions", choice_t( "##fp-eh" ), choice_t( "No", "/fp:except-" ), choice_t( "Yes", "/fp:except" ) ),
                    option_t( "Enable Intel JCC Erratum Mitigation", choice_t( "No" ), choice_t( "Yes", "/QIntelJccErratum" ) ),
                    option_t( "Enable EH Continuation Metadata", choice_t( "##eh-cont-metadata" ), choice_t( "No", "/guard:ehcont-" ), choice_t( "Yes", "/guard:ehcont" ) )
                );

                m_c_tab.m_subtabs.push_back( code_generation );

                subtab_t language;
                language.m_name = "Language##c";

                language.add_options(
                    option_t( "Enable Run-Time Type Information", choice_t( "Yes", "/GR" ), choice_t( "No", "/GR-" ) ),
                    option_t( "C++ Language Standard", choice_t( "Default", "/std:c++14" ), choice_t( "C++14", "/std:c++14" ), choice_t( "C++17", "/std:c++17" ), choice_t( "C++20", "/std:c++20" ), choice_t( "Preview", "/std:c++latest" ) ),
                    option_t( "C Language Standard", choice_t( "Default", "/std:c11" ), choice_t( "C11", "/std:c11" ), choice_t( "C17", "/std:c17" ) )
                );

                m_c_tab.m_subtabs.push_back( language );

                subtab_t precompiled_headers;
                precompiled_headers.m_name = "Precompiled Headers##c";

                precompiled_headers.add_options( 
                    option_t( "Precompiled Header", choice_t( "Not Using Precompiled Headers", "/Y-" ), choice_t( "Create Precompiled Header", "/Yc" ), choice_t( "Use Precompiled Header", "/Yu" ) ),
                    option_t( true, "Precompiled Header File" ),
                    option_t( true, "Precompiled Header Output File" )
                );

                m_c_tab.m_subtabs.push_back( precompiled_headers );

                subtab_t output_files;
                output_files.m_name = "Output Files##c";

                output_files.add_options(
                    option_t( "Assembler Output", choice_t( "No Listing", "/FA-" ), choice_t( "Assembly-Only Listing", "/FA" ), choice_t( "Assembly and Machine Code Listing", "/FAs" ), choice_t( "Assembly, Machine Code and Source", "/FAcs" ) ),
                    option_t( true, "ASM List Location" ),
                    option_t( true, "Object File" )
                );

                m_c_tab.m_subtabs.push_back( output_files );

                subtab_t advanced;
                advanced.m_name = "Advanced##c";

                advanced.add_options(
                    option_t( "Calling Convention", choice_t( "__cdecl", "/Gd" ), choice_t( "__fastcall", "/Gr" ), choice_t( "__stdcall", "/Gz" ), choice_t( "__vectorcall", "/Gv" ) ),
                    option_t( "Compile As", choice_t( "Default" ), choice_t( "Compile as C++ Code", "/TP" ), choice_t( "Compile as C Code", "/TC" ) ),
                    option_t( true, "Force Include File" ),
                    option_t( "Show Includes", choice_t( "No" ), choice_t( "Yes", "/showIncludes" ) ),
                    option_t( "Omit Default Library Name", choice_t( "No" ), choice_t( "Yes", "/Zl" ) )
                );

                m_c_tab.m_subtabs.push_back( advanced );

                subtab_t all_options;
                all_options.m_name = "All Options##c";

                m_c_tab.m_subtabs.push_back( all_options );

                subtab_t command_line;
                command_line.m_name = "Command Line##c";

                m_c_tab.m_subtabs.push_back( command_line );
            }

            // linker
            {
                subtab_t general;
                general.m_name = "General##linker";

                general.add_options(
                    option_t( true, "Output File" ),
                    option_t( true, "Version" ),
                    option_t( "Enable Incremental Linking", choice_t( "Yes", "/INCREMENTAL" ), choice_t( "No", "/INCREMENTAL:NO" ) ),
                    option_t( "Ignore Import Library", choice_t( "No" ), choice_t( "Yes", "/IGNORE:LIB" ) ),
                    //option_t( "Register output", choice_t( "No" ), choice_t( "Yes", "/REGISTER" ) ),
                    option_t( true, "Additional Library Directories" ),
                    option_t( "Prevent Dll Binding", choice_t( "##prevent_dll_binding" ), choice_t( "No", "/ALLOWBIND:NO" ), choice_t( "Yes", "/ALLOWBIND" ) ),
                    option_t( "Treat Linker Warnings As Errors", choice_t( "No", "/WX:NO" ), choice_t( "Yes", "/WX" ) ),
                    option_t( "Force File Output", choice_t( "No" ), choice_t( "Enabled", "/FORCE" ), choice_t( "Multiple Defined Symbols Only", "/FORCE:MULTIPLE" ), choice_t( "Undefined Symbols Only", "/FORCE:UNRESOLVED" ) ),
                    option_t( true, "Specify Section Attributes" )
                );

                m_linker_tab.m_subtabs.push_back( general );

                subtab_t input;
                input.m_name = "Input##linker";

                input.add_options(
                    option_t( true, "Additional Dependencies" ),
                    option_t( "Ignore All Default Libraries", choice_t( "No" ), choice_t( "Yes", "/NODEFAULTLIB" ) ),
                    option_t( true, "Ignore Specific Default Libraries" )
                );

                m_linker_tab.m_subtabs.push_back( input );

                subtab_t manifest_file;
                manifest_file.m_name = "Manifest File##linker";

                manifest_file.add_options(
                    option_t( "Generate Manifest", choice_t( "No", "/MANIFEST:NO" ), choice_t( "Yes", "/MANIFEST" ) ),
                    option_t( "Enable User Account Control", choice_t( "No", "/MANIFESTUAC:NO" ), choice_t( "Yes", "/MANIFESTUAC:" ) ),
                    option_t( "UAC Execution Level", choice_t( "As Invoker", "level='asInvoker'" ), choice_t( "Highest Available", "level='highestAvailable'" ), choice_t( "Require Administrator", "level='requireAdministrator'" ) ),
                    option_t( "UAC Bypass UI Protection", choice_t( "No", "uiAccess='false'" ), choice_t( "Yes", "uiAccess='true'" ) )
                );

                m_linker_tab.m_subtabs.push_back( manifest_file );

                subtab_t debug;
                debug.m_name = "Debugging##linker";

                debug.add_options(
                    option_t( "Generate Debug Info", choice_t( "None", "/DEBUG:NONE" ), choice_t( "Yes", "/DEBUG" ), choice_t( "Optimized for faster links", "/DEBUG:FASTLINK" ), choice_t( "Optimized for sharing and publishing", "/DEBUG:FULL" ) ),
                    option_t( true, "Generate Program Database File" )
                );

                m_linker_tab.m_subtabs.push_back( debug );

                subtab_t system;
                system.m_name = "System##linker";

                system.add_options(
                    option_t( "SubSystem", choice_t( "Console", "/SUBSYSTEM:CONSOLE" ), choice_t( "Windows", "/SUBSYSTEM:WINDOWS" ), choice_t( "Native", "/SUBSYSTEM:NATIVE" ), choice_t( "EFI Application", "/SUBSYSTEM:EFI_APPLICATION" ), choice_t( "EFI Boot Service Driver", "/SUBSYSTEM:EFI_BOOT_SERVICE_DRIVER" ), choice_t( "EFI ROM", "/SUBSYSTEM:EFI_ROM" ), choice_t( "EFI Runtime Driver", "/SUBSYSTEM:EFI_RUNTIME_DRIVER" ) ),
                    option_t( true, "Minimum Required Version" ),
                    option_t( true, "Heap Reserve Size" ),
                    option_t( true, "Heap Commit Size" ),
                    option_t( true, "Stack Reserve Size" ),
                    option_t( true, "Stack Commit Size" ),
                    option_t( true, "Large Address Aware" ),
                    option_t( "Driver", choice_t( "Not set" ), choice_t( "Driver", "/Driver" ), choice_t( "UP Only", "/DRIVER:UPONLY" ), choice_t( "WDM", "/DRIVER:WDM" ) )
                );

                m_linker_tab.m_subtabs.push_back( system );

                subtab_t optimization;
                optimization.m_name = "Optimization##linker";

                optimization.add_options(
                    option_t( "Optimize References", choice_t( "No", "/OPT:NOREF" ), choice_t( "Yes", "/OPT:REF" ) ),
                    option_t( "Enable COMDAT Folding", choice_t( "No", "/OPT:NOICF" ), choice_t( "Yes", "/OPT:ICF" ) ),
                    option_t( true, "Function Order" )
                );

                m_linker_tab.m_subtabs.push_back( optimization );

                subtab_t advanced;
                advanced.m_name = "Advanced##linker";

                advanced.add_options( 
                    option_t( true, "Entry Point" ),
                    option_t( "No Entry Point", choice_t( "No" ), choice_t( "Yes", "/NOENTRY" ) ),
                    option_t( true, "Base Address" ),
                    option_t( "Randomized Base Address", choice_t( "Yes", "/DYNAMICBASE" ), choice_t( "No", "/DYNAMICBASE:NO" ) ),
                    option_t( "Fixed Base Address", choice_t( "No", "/FIXED:NO" ), choice_t( "Yes", "/FIXED" ) ),
                    option_t( "Data Execution Prevention (DEP)", choice_t( "Yes", "/NXCOMPAT" ), choice_t( "No", "/NXCOMPAT:NO" ) ),
                    option_t( true, "Import Library" ),
                    option_t( true, "Merge Sections" ),
                    option_t( "Target Machine", choice_t( "Not Set" ), choice_t( "MachineX86", "/MACHINE:X86" ), choice_t( "MachineX64", "/MACHINE:X64" ), choice_t( "MachineARM", "/MACHINE:ARM" ), choice_t( "MachineARM64", "/MACHINE:ARM64" ) )
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
            // c/c++
            {
                subtab_t general;
                general.m_name = "General##c";

                general.add_options( 
                    option_t( true, "Additional Include Directories" ),
                    //option_t( true, "Public Project Include Directories" ),
                    option_t( "Debug Information Format", choice_t( "None", "-g0" ), choice_t( "Minimal Debug Information", "-g1" ), choice_t( "Full Debug Information (DWARF2)", "-g2 -gdwarf-2" ) ),
                    option_t( "Warning Level", choice_t( "Enable All Warnings", "-Wall" ), choice_t( "Turn Off All Warnings", "-w" ) ),
                    option_t( "Treat Warnings As Errors", choice_t( "No" ), choice_t( "Yes", "-Werror" ) ),
                    option_t( true, "C Additional Warnings" ),
                    option_t( true, "C++ Additional Warnings" ),
                    option_t( "Enable Verbose Mode", choice_t( "No" ), choice_t( "Yes", "-v" ) ),
                    option_t( "Multi-processor Compilation", choice_t( "No" ), choice_t( "Yes", "/MP" ) )
                );

                m_c_tab.m_subtabs.push_back( general );

                subtab_t optimization;
                optimization.m_name = "Optimization##c";

                optimization.add_options(
                    option_t( "Optimization Level", choice_t( "None", "-O0" ), choice_t( "Minimize Size", "-Os" ), choice_t( "Maximize Speed", "-O2" ), choice_t( "Full Optimization", "-O3" ) ),
                    option_t( "Strict Aliasing", choice_t( "No", "-fno-strict-aliasing" ), choice_t( "Yes", "-fstrict-aliasing" ) ),
                    option_t( "Unroll Loops", choice_t( "No" ), choice_t( "Yes", "-funroll-loops" ) ),
                    option_t( "Link Time Optimization", choice_t( "No" ), choice_t( "Yes", "-flto" ) ),
                    option_t( "Omit Frame Pointers", choice_t( "No", "-fno-omit-frame-pointer" ), choice_t( "Yes", "-fomit-frame-pointer" ) ),
                    option_t( "No Common Blocks", choice_t( "No" ), choice_t( "Yes", "-fno-common" ) )
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

                code_generation.add_options(
                    option_t( "Position Independent Code", choice_t( "No" ), choice_t( "Yes", "-fPIC" ) ),
                    option_t( "Statics are thread safe", choice_t( "Yes", "-fthreadsafe-statics" ), choice_t( "No", "-fno-threadsafe-statics" ) ),
                    option_t( "Floating Point Optimization", choice_t( "No" ), choice_t( "Yes", "-ffast-math" ) ),
                    option_t( "Inline Methods Hidden", choice_t( "No" ), choice_t( "Yes", "-fvisibility-inlines-hidden" ) ),
                    option_t( "Symbol Hiddens By Default", choice_t( "No" ), choice_t( "Yes", "-fvisibility=hidden" ) ),
                    option_t( "Enable C++ Exceptions", choice_t( "Yes", "-fexceptions" ), choice_t( "No", "-fno-exceptions" ) ),
                    option_t( "Enable Enhanced Instruction Set", choice_t( "Not Set" ), choice_t( "No Enhanced Instructions", "-march=generic" ), choice_t( "SSE", "-msse" ), choice_t( "SSE2", "-msse2" ), choice_t( "AVX", "-mavx" ), choice_t( "AVX2", "-mavx2" ), choice_t( "AVX512", "-mavx512" ) )
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

                subtab_t advanced;
                advanced.m_name = "Advanced##c";

                advanced.add_options( 
                    option_t( "Compile As", choice_t( "Default" ), choice_t( "Compile as C++ Code", "-x c++" ), choice_t( "Compile as C Code", "-x c" ) ),
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

    void open( )
    {
        std::ifstream file( m_config_file );

        if ( !file.is_open( ) || !file.good( ) )
        {
            fprintf( stderr, "failed to open file %s\n", m_config_file );
            return;
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
    }

    void save( )
    {
        std::ofstream file( m_config_file );

        if ( !file.is_open( ) || !file.good( ) )
        {
            fprintf( stderr, "failed to open file %s\n", m_config_file );
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

        file << json;

        file.close( );
    }
}