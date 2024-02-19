#include <vector>
#include <string>

#include <nlohmann/json.hpp>

namespace options
{
    struct choice_t
    {
        std::string m_name;
        std::string m_cmd_line;
        std::string m_user_output;

        choice_t( const std::string& name, const std::string& cmd_line = std::string( "" ) )
            : m_name( name ), m_cmd_line( cmd_line )
        {
            m_user_output = name;
            if ( !cmd_line.empty( ) )
                m_user_output += std::string( " (" ) + cmd_line + std::string( ")" );
        }
    };

    struct option_t
    {
        std::string m_name;
        int m_current_choice;
        std::vector< choice_t > m_choices;

        bool m_is_single_line;
        std::string m_params;

        template< typename... args >
        option_t( bool single_line, const std::string& name, args... choices )
        {
            m_current_choice = 0;
            m_name = name;
            m_is_single_line = single_line;
            m_params = "";
            if ( !single_line )
                ( m_choices.push_back( choices ), ... );
        }

        template< typename... args >
        option_t( const std::string& name, args... choices )
        {
            m_current_choice = 0;
            m_name = name;
            m_is_single_line = false;
            m_params = "";
            ( m_choices.push_back( choices ), ... );                
        }

        template< typename _ty >
        static inline _ty read( nlohmann::json& json, const std::string& name )
        {
            if ( json.find( name ) == json.end( ) )
                return _ty( );

            return json[ name ].get< _ty >( );
        }
    };

    struct subtab_t
    {
        std::string m_name;
        std::vector< option_t > m_options;

        template< typename... args >
        void add_options( args... options )
        {
            ( m_options.push_back( options ), ... );
        }
    };

    struct tab_t
    {
        std::string m_name;
        std::vector< subtab_t > m_subtabs;
        char m_command_line[ 1024 ];
    };

    inline tab_t m_c_tab;
    inline tab_t m_linker_tab;
    inline int m_compiler = 1;
    inline char m_config_file[ 256 ];
    inline char m_output_file[ 256 ];

    void init( );
    void open( );
    void save( );
}