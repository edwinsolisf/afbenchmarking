echo %1 %2
set "conf=Release"

if not exist ./bin/%conf%/aftests.exe (
    rmdir /S /Q build
    cmake -B build -DCMAKE_BUILD_TYPE=%conf% -G "Visual Studio 17 2022"
    cmake --build build --config %conf%
)
cd bin/%conf%
powershell ".\aftests.exe %1 %2 | tee -filepath .\af_results.txt -append"