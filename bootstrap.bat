@echo off
if exist "vcpkg" (
	cd vcpkg
) else (
	git clone https://github.com/microsoft/vcpkg.git
	cd vcpkg
	.\bootstrap-vcpkg.bat
)

.\vcpkg.exe install glm:x64-windows
.\vcpkg.exe install spdlog:x64-windows
.\vcpkg.exe install eigen3:x64-windows
.\vcpkg.exe install glslang:x64-windows
.\vcpkg.exe install assimp:x64-windows