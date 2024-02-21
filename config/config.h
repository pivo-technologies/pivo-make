#include <string>
#include <vector>

namespace config
{
#ifdef _WIN32
    inline std::string m_config_path = std::string( "pivo.config" );
#else
    inline std::string m_config_path = std::string( "/etc/pivo-make/pivo.config" );
#endif
    inline char m_cxx_compiler[ 256 ];
    inline char m_linker_windows[ 256 ];
    inline char m_linker_linux[ 256 ];

    inline std::vector< std::string > m_include_path;
    inline std::vector< std::string > m_lib_path;
    bool read( );
}