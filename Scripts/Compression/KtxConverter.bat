@echo off
for %%f in (*.png *.jpg *.jpeg) do (
    echo Converting %%f to KTX2...
    "./img2ktx.exe" -f BC7 -o "%%~nf.ktx2" "%%f"
)
echo Conversion completed.
pause
