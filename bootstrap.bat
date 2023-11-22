@echo off
if exist "vcpkg" (
	cd vcpkg
) else (
	git clone https://github.com/microsoft/vcpkg.git
	cd vcpkg
	.\bootstrap-vcpkg.bat
)

.\vcpkg.exe install entt:x64-windows-release
.\vcpkg.exe install glm:x64-windows-release
.\vcpkg.exe install mimalloc:x64-windows-release
.\vcpkg.exe install spdlog:x64-windows-release
.\vcpkg.exe install fmt:x64-windows-release
.\vcpkg.exe install eigen3:x64-windows-release
.\vcpkg.exe install rttr:x64-windows-release
.\vcpkg.exe install taskflow:x64-windows-release
.\vcpkg.exe install tbb:x64-windows-release
.\vcpkg.exe install boost:x64-windows-release
.\vcpkg.exe install glslang:x64-windows-release
.\vcpkg.exe install pkgconf:x64-windows-release
.\vcpkg.exe install clipper2:x64-windows-release
.\vcpkg.exe install parallel-hashmap:x64-windows-release
.\vcpkg.exe install assimp:x64-windows-release
.\vcpkg.exe install spirv-cross:x64-windows-release
.\vcpkg.exe install lmdb:x64-windows-release