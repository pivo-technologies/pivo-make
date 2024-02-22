#include <vector>
#include <string>

#include "../nlohmann/json.hpp"

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

        inline void set_params( const std::string& params )
        {
            m_params = params;
        }

        inline void set_choice( const std::string& choice_name )
        {
            for ( int i = 0; i < m_choices.size( ); i++ )
            {
                if ( m_choices[ i ].m_name == choice_name )
                {
                    m_current_choice = i;
                    return;
                }
            }

            asm( "ud2" );
        }
    };

    struct subtab_t
    {
        std::string m_name;
        std::vector< option_t > m_options;

        inline option_t& operator[ ]( const std::string& option )
        {
            for ( auto& o : m_options )
                if ( o.m_name == option )
                    return o;
            
            asm( "ud2" );
        }

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

        inline subtab_t& operator[ ]( const std::string& subtab )
        {
            for ( auto& s : m_subtabs )
                if ( s.m_name == subtab )
                    return s;
            
            asm( "ud2" );
        }
    };

    inline tab_t m_c_tab;
    inline tab_t m_linker_tab;
    inline int m_compiler = 1;
    inline char m_config_file[ 256 ];
    inline char m_output_file[ 256 ];
    inline char m_intermediate_dir[ 256 ];

    void init( );
    bool open( );
    void save( );
    void load_defaults_debug( );
    void load_defaults_release( );
}