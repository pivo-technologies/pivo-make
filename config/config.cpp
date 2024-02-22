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
            m_cxx_windows = "clang++";
            m_cxx_linux = "clang++";
            m_linker_windows = "lld-link";
            m_linker_linux = "clang++";

            return false;
        }

        nlohmann::json json;
        file >> json;

        file.close( );

        m_cxx_windows = json[ "cxx_windows" ].get< std::string >( );
        m_cxx_linux = json[ "cxx_linux" ].get< std::string >( );
        m_linker_windows = json[ "linker_windows" ].get< std::string >( );
        m_linker_linux = json[ "linker_linux" ].get< std::string >( );

        if ( json.contains( "include_path_windows" ) )
            m_include_path_windows = json[ "include_path_windows" ].get< std::vector< std::string > >( );

        if ( json.contains( "lib_path_windows" ) )
            m_lib_path_windows = json[ "lib_path_windows" ].get< std::vector< std::string > >( );

        if ( json.contains( "include_path_linux" ) )
            m_include_path_linux = json[ "include_path_linux" ].get< std::vector< std::string > >( );

        if ( json.contains( "lib_path_linux" ) )
            m_lib_path_linux = json[ "lib_path_linux" ].get< std::vector< std::string > >( );

        return true;
    }
}