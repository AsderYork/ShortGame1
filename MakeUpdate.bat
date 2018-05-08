@echo off
echo Compressing client's release
powershell "Compress-Archive -Path ./build/Release -DestinationPath ./ClientRelease.zip -force"

echo Compressing server's release
powershell "Compress-Archive -Path ./ServerBuild/x64/Release -DestinationPath ./ServerRelease.zip -force"

echo Archives are ready! Uploading
