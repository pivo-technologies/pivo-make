#include "config.h"

#include "../nlohmann/json.hpp"

#include <string>
#include <fstream>

namespace config
{
    bool read( )
    {        
        std::ifstream file( m_config_path );

        if ( !file.is_open( ) || !file.good( ) )
        {
            strcpy( m_cxx_compiler, "clang++" );
            strcpy( m_linker_windows, "lld-link" );
            strcpy( m_linker_linux, "clang++" );

            return false;
        }

        nlohmann::json json;
        file >> json;

        file.close( );

        strncpy( m_cxx_compiler, json[ "cxx_compiler" ].get< std::string >( ).data( ), sizeof( m_cxx_compiler ) );
        strncpy( m_linker_windows, json[ "linker_windows" ].get< std::string >( ).data( ), sizeof( m_linker_windows ) );
        strncpy( m_linker_linux, json[ "linker_linux" ].get< std::string >( ).data( ), sizeof( m_linker_linux ) );

        if ( json.contains( "include_path" ) )
            m_include_path = json[ "include_path" ].get< std::vector< std::string > >( );

        if ( json.contains( "lib_path" ) )
            m_lib_path = json[ "lib_path" ].get< std::vector< std::string > >( );

        return true;
    }
}