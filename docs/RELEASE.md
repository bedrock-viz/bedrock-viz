# Incomplete notes on how to release a new version of bedrock-viz

## Windows

Do the steps in [the build docs](BUILD.md), but adjust the last two steps as follows:
```
cmake .. -DCMAKE_TOOLCHAIN_FILE=..\..\..\vcpkg\scripts\buildsystems\vcpkg.cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_CONFIGURATION_TYPES=Release
msbuild bedrock-viz.sln -m -p:Configuration=Release
```
then copy the output into a folder with the previous version, replace the files that we generated above and zip it up.
