import platform
import os
import sys
import ctypes
import shutil
import subprocess
import time

def close( ):
    time.sleep( 3600 )
    exit( 0 )

windows = False

if platform.system( ) == 'Windows':
    windows = True
    is_admin = False
    try:
        is_admin = ctypes.windll.shell32.IsUserAnAdmin( )
    except:
        is_admin = False

    if not is_admin:
        ctypes.windll.shell32.ShellExecuteW( None, "runas", sys.executable, " ".join( sys.argv ), None, 1 )
        exit( 0 )
elif platform.system( ) == 'Linux':
    if os.getuid( ) != 0:
        print( 'Installation script must be run as root' )
        close( )
else:
    print( 'You are running installation script on unsupported OS' )
    close( )

intermediate_file = 'pivomake.exe' if windows else 'pivomake'
subprocess.run( f'python build.py -o {intermediate_file} --no-remove --do-not-run', shell=True )

if not os.path.exists( intermediate_file ):
    print( 'Failed to build pivo-make, aborting' )
    close( )

preset = 'build-release-windows' if windows else 'build-release-linux'
if not windows:
    intermediate_file = './' + intermediate_file
subprocess.run( f'{intermediate_file} -c {preset}', shell=True )

if not os.path.exists( 'pivo-make.exe' if windows else 'pivo-make' ):
    print( 'pivo-make is not found, aborting' )
    close( )

if not os.path.exists( 'pivo.config' ):
    print( 'Config file is not found, aborting' )
    close( )

os.makedirs( 'C:\\Program Files\\pivo-make' if windows else '/etc/pivo-make', exist_ok=True )

shutil.copy( 'pivo.config', 'C:\\Program Files\\pivo-make\\pivo.config' if windows else '/etc/pivo-make/pivo.config' )
shutil.copy( 'pivo-make.exe' if windows else 'pivo-make', 'C:\\Program Files\\pivo-make' if windows else '/usr/local/bin/pivo-make' )

if not os.path.exists( 'C:\\Program Files\\pivo-make\\pivo.config' if windows else '/etc/pivo-make/pivo.config' ) or not os.path.exists( 'C:\\Program Files\\pivo-make\\pivo-make.exe' if windows else '/usr/local/bin/pivo-make' ):
    print( 'Installation failed' )
else:
    if windows:
        import winreg
        with winreg.OpenKey( winreg.HKEY_LOCAL_MACHINE, r'SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment', 0, winreg.KEY_ALL_ACCESS ) as key:
            current_path = winreg.QueryValueEx( key, 'Path' )[ 0 ]
            if 'C:\\Program Files\\pivo-make\\' not in current_path:
                winreg.SetValueEx( key, 'Path', 0, winreg.REG_EXPAND_SZ, current_path + ';' + 'C:\\Program Files\\pivo-make\\' )
    print( 'pivo-make has been installed to', 'C:\\Program Files\\pivo-make\\' if windows else '/usr/local/bin/' )
    print( 'You can change the config file following documentation', 'C:\\Program Files\\pivo-make\\pivo.config' if windows else '/etc/pivo-make/pivo.config' )

print( 'This window will close in 1 hour. You may do it yourself' )
close( )