#include <string>
#include <vector>

namespace config
{
#ifdef _WIN32
    inline std::string m_config_path = std::string( "pivo.config" );
#else
    inline std::string m_config_path = std::string( "/etc/pivo-make/pivo.config" );
#endif
    inline std::string m_cxx_windows;
    inline std::string m_cxx_linux;
    inline std::string m_linker_windows;
    inline std::string m_linker_linux;

    inline std::vector< std::string > m_include_path_windows;
    inline std::vector< std::string > m_lib_path_windows;
    inline std::vector< std::string > m_include_path_linux;
    inline std::vector< std::string > m_lib_path_linux;
    bool read( );
}