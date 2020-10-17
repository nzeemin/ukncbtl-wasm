@echo off

SET EMSDKPATH=C:\bin\emsdk

CALL %EMSDKPATH%\emsdk_env.bat 

SET SOURCE=Emulator.cpp ^
 %SOURCE% emubase\Disasm.cpp ^
 %SOURCE% emubase\Board.cpp ^
 %SOURCE% emubase\Processor.cpp ^
 %SOURCE% emubase\Memory.cpp ^
 %SOURCE% emubase\Floppy.cpp ^
 %SOURCE% emubase\Hard.cpp ^

@echo on

@rem emcc %SOURCE% -s WASM=1  2>emcc.log
@rem emcc %SOURCE% -s WASM=1 -s SAFE_HEAP=1 -o emul.html --shell-file shell_minimal.html
emcc %SOURCE% -s WASM=1 -O2 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" -s FORCE_FILESYSTEM=1 -s NO_EXIT_RUNTIME=1 -fno-exceptions -fno-rtti -o emul.html --shell-file shell_minimal.html

