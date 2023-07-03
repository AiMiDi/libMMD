git submodule update --init --recursive
for /f "delims=" %%i in ('"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -products * -requires Microsoft.Component.MSBuild -property installationPath -latest') do set VSPath=%%i
call "%VSPath%\VC\Auxiliary\Build\vcvars64.bat" x64
cd libMMD/dependency/libiconv
mkdir build 
cd build
cmake ..  -G "Ninja Multi-Config"
cmake --build . --config Debug -j
cmake --install . --config Debug --prefix ../../install/
cmake --build . --config Release -j
cmake --install . --config Release --prefix ../../install/
cd ..
rmdir build /s /q
cd ..
cd yalantinglibs
mkdir build 
cd build
cmake ..  -G "Ninja Multi-Config" -DBUILD_BENCHMARK=OFF -DBUILD_EXAMPLES=OFF
cmake --build . --config Debug -j
ctest .
cmake --install . --config Debug --prefix ../../install/
cmake --build . --config Release -j
ctest .
cmake --install . --config Release --prefix ../../install/
cd ..
rmdir build /s /q
cd ..
pause
