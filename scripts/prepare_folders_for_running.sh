# Please make sure working directory is "scripts"
rm -rf ../sample_runtime
mkdir -p ../sample_runtime
cp -r ../build/datafiles ../sample_runtime
cp -r ../external/cegui/datafiles ../sample_runtime
cp -r ../build/external/cegui/lib/* ../sample_runtime
cp -r ../build/external/cegui/bin/* ../sample_runtime
cp -r ../build/*.so ../sample_runtime
cp -r ../debug_data ../sample_runtime
echo "Sample runtime folder is set up"