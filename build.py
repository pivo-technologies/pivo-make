import subprocess
import os
import sys

output_file = ""

if len( sys.argv ) > 1:
    output_file = sys.argv[ 1 ]
else:
    print( "usage: python <script_name> <output>" )
    sys.exit( 1 )

remove_file = True

if len( sys.argv ) > 2 and sys.argv[ 2 ] == "--no-remove":
    remove_file = False

app_flags = ""
if ( remove_file and len( sys.argv ) > 3 ) or ( not remove_file and len( sys.argv ) > 2 ):
    for i in range( 3, len( sys.argv ) ):
        app_flags += sys.argv[ i ] + " "

cxx_flags = "-std=c++20 -O3 -fproc-stat-report  "
linker_flags = "-lglfw -lGLU -lGL -lpthread"

cxx_command = 'find . -name "*.cpp" | xargs -I{} -P$(nproc) clang++ -c {} ' + cxx_flags + '-o {}.o'

subprocess.run( 'find . -name "*.o" -delete', shell=True )
subprocess.run( cxx_command, shell=True )
subprocess.run( f'find . -name "*.o" | xargs clang++ -o {output_file} {linker_flags}', shell=True )

if os.path.exists( output_file ):
    lldb_command = f'lldb -o "settings set target.x86-disassembly-flavor intel" -o "run {app_flags}" {output_file}'
    print( lldb_command )
    subprocess.run( lldb_command, shell=True )
else:
    print( "failed to compile" )

if remove_file:
    subprocess.run( f"rm -f {output_file}", shell=True )

subprocess.run( 'find . -name "*.o" -delete', shell=True )
