import platform
import os
import ctypes
import shutil
import subprocess

windows = False

if platform.system( ) == 'Windows':
    windows = True
    is_admin = False
    try:
        is_admin = ctypes.windll.shell32.IsUserAnAdmin( )
    except:
        is_admin = False

    if not is_admin:
        print( 'Installation script must be run as administrator' )
        exit( 0 )
elif platform.system( ) == 'Linux':
    if os.getuid( ) != 0:
        print( 'Installation script must be run as root' )
        exit( 0 )
else:
    print( 'You are running installation script on unsupported OS' )
    exit( 0 )

intermediate_file = 'pivomake.exe' if windows else 'pivomake'
subprocess.run( f'python build.py -o {intermediate_file} --no-remove --do-not-run', shell=True )

if not os.path.exists( intermediate_file ):
    print( 'Failed to build pivo-make, aborting' )
    exit( 0 )

preset = 'build-release-windows' if windows else 'build-release-linux'
if not windows:
    intermediate_file = './' + intermediate_file
subprocess.run( f'{intermediate_file} -c {preset}', shell=True )

if not os.path.exists( 'pivo-make.exe' if windows else 'pivo-make' ):
    print( 'pivo-make is not found, aborting' )
    exit( 0 )

if not os.path.exists( 'pivo.config' ):
    print( 'Config file is not found, aborting' )
    exit( 0 )

os.makedirs( 'C:\\Program Files\\pivo-make' if windows else '/etc/pivo-make', exist_ok=True )

shutil.copy( 'pivo.config', 'C:\\Program Files\\pivo-make\\pivo.config' if windows else '/etc/pivo-make/pivo.config' )
shutil.copy( 'pivo-make.exe' if windows else 'pivo-make', 'C:\\Program Files\\pivo-make' if windows else '/usr/local/bin/pivo-make' )

if not os.path.exists( 'C:\\Program Files\\pivo-make\\pivo.config' if windows else '/etc/pivo-make/pivo.config' ) or not os.path.exists( 'C:\\Program Files\\pivo-make\\pivo-make.exe' if windows else '/usr/local/bin/pivo-make' ):
    print( 'Installation failed' )
else:
    print( 'pivo-make has been installed to', 'C:\\Program Files\\pivo-make\\' if windows else '/usr/local/bin/' )
    print( 'You can change the config file following documentation', 'C:\\Program Files\\pivo-make\\pivo.config' if windows else '/etc/pivo-make/pivo.config' )