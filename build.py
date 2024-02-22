import subprocess
import os
import sys
import platform

windows = False

if platform.system( ) == 'Windows':
    windows = True

output_file = ""
remove_file = True
do_not_run = False
app_flags = ""

def print_usage( ):
    print( "usage: python <script_name> -o <output> [--no-remove] [--do-not-run] --args [arguments]" )

if len( sys.argv ) < 2:
    print_usage( )
    sys.exit( 1 )

for i in range ( 1, len( sys.argv ) ):
    if sys.argv[ i ] == "-o":
        if i + 1 < len( sys.argv ):
            output_file = sys.argv[ i + 1 ]
            i += 1
        else:
            print_usage( )
            sys.exit( 1 )
    elif sys.argv[ i ] == "--help":
        print_usage( )
        sys.exit( 0 )
    elif sys.argv[ i ] == "--no-remove":
        remove_file = False
    elif sys.argv[ i ] == "--do-not-run":
        do_not_run = True
    elif sys.argv[ i ] == "--args":
        for j in range ( i + 1, len( sys.argv ) ):
            app_flags += sys.argv[ j ] + " "
        break

if not windows:
    cxx_flags = "-std=c++20 -g2 -gdwarf-2 "
    linker_flags = "-lglfw -lGLU -lGL -lpthread"

    print( "Building pivo-make for Linux" )

    subprocess.run( 'find . -name "*.o" -delete', shell=True )
    subprocess.run( 'find . -name "*.cpp" | xargs -I{} -P$(nproc) clang++ -c {} ' + cxx_flags + '-o {}.o', shell=True )
    subprocess.run( f'find . -name "*.o" | xargs clang++ -o {output_file} {linker_flags}', shell=True )
    subprocess.run( 'find . -name "*.o" -delete', shell=True )
else:
    print( "Building pivo-make for Windows" )
    subprocess.run( 'del /s /q *.o', shell=True )
    subprocess.run( 'for /r %i in (*.cpp) do ( clang++ -c -std=c++20 -D _WIN32 -D UNICODE -D _CRT_SECURE_NO_WARNINGS "%i" -o "%~ni.o" )', shell=True )
    if os.path.exists( output_file ):
        os.remove( output_file )
    subprocess.run( f'lld-link /libpath:"C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x64" /libpath:"C:\\Program Files (x86)\\Windows Kits\\10\Lib\\10.0.22621.0\\um\\x64" /libpath:"C:\\Program Files (x86)\\Windows Kits\10\Lib\10.0.22621.0\\ucrt\\x64" /defaultlib:LIBCMT.lib d3d11.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /DYNAMICBASE /NXCOMPAT *.o /subsystem:CONSOLE /out:{output_file}', shell=True )
    subprocess.run( 'del /s /q *.o', shell=True )

if os.path.exists( output_file ):
    print( "File is successfully compiled" )
    if not do_not_run:
        subprocess.run( f'lldb -o "settings set target.x86-disassembly-flavor intel" -o "run {app_flags}" {output_file}', shell=True )
else:
    print( "Failed to compile" )

if remove_file and os.path.exists( output_file ):
    os.remove( output_file )
