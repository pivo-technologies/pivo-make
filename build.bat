echo Deleting old object files...
del /s /q *.o
for /r %%i in (*.cpp) do (
    echo Compiling %%i
    clang++ -c -std=c++20 -D _WIN32 -D UNICODE -D _CRT_SECURE_NO_WARNINGS "%%i" -o "%%~ni.o"
)
echo Linking...
del pivomake.exe
lld-link /libpath:"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64" /libpath:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\um\x64" /libpath:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\ucrt\x64" /defaultlib:LIBCMT.lib d3d11.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /DYNAMICBASE /NXCOMPAT *.o /subsystem:CONSOLE /out:pivomake.exe
echo Deleting object files...
del /s /q *.o
echo Compiled.

lldb -o "settings set target.x86-disassembly-flavor intel" -o run pivomake.exe