# Chip-8_Emulator
This is a simple implementation of a Chip-8 system, which serves as an introduction to hardware emulation.

It was tested and run using a [Chip-8 Test ROM](https://github.com/corax89/chip8-test-rom/blob/master/README.md?plain=1) as well as several game ROMs. 

Links for some of the original games created for this system can be found [here](https://github.com/JamesGriffin/CHIP-8-Emulator/tree/master/roms) (they're all public domain, I just didn't upload them), while a list of playable gamejam submissions for this system can be found [here](https://johnearnest.github.io/chip8Archive/).

Files can be run using:
```
./chip8 <ROM file>
```
The cmake-compiled file is included in the build folder, but you can also make your own by creating a new directory in the repo, setting it to the current directory, and running:
```
$ cmake ...
$ make
```


## Sources and References
- [Chip-8 Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
- [James Griffin's C++ Chip-8 Implementation](https://github.com/JamesGriffin/CHIP-8-Emulator)
- [Laurence Muller's "How to write an emulator"](https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
- [Austin Morlan's "Building A Chip-8 Emulator"](https://austinmorlan.com/posts/chip8_emulator/)
- [codeslinger.co.uk's Chip-8 Emulation Guide](http://www.codeslinger.co.uk/pages/projects/chip8.html)
  
Working through this project taught me a lot about low-level system emulation and structure as well as the various ways it can be done with a mix of modern and older practices. I can't say my implementation is perfect, even if I referenced the source materials pretty often when I got stuck, but the end result is functional and I had fun putting it together. 

