clang --% -g -O0 text.c main.c -ISDL2-2.26.0-VC\include -I. -LSDL2-2.26.0-VC\lib\x64 -L. -w -Xlinker /subsystem:console -lSDL2 -lSDL2main -lShell32 -lSDL2_ttf -o scriba.exe
if($LASTEXITCODE -eq 0) {
    & .\scriba.exe
}