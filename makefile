SrcDir = src
BinDir = bin
LibAeds = $(SrcDir)/libaeds

OutputFileName   = restaurant
OutputFile       = $(BinDir)/$(OutputFileName)
CompilationUnits = $(LibAeds)/*.c			  \
									 $(LibAeds)/adt/*.c   \
									 $(SrcDir)/util/*.c   \
									 $(SrcDir)/entity/*.c \
									 $(SrcDir)/*.c

Build       = clang
Standard    = c99
Optimize    = -flto -O2
Debug       = -g
BuildFlags  = -Wall             \
              -std=$(Standard)	\
							$(Optimize)       \
              -I $(SrcDir)      \
              -o $(OutputFile)


directories: $(SrcDir)
	@[ -d "$(BinDir)" ] || mkdir "$(BinDir)";

build: directories
	@$(Build) $(BuildFlags) $(CompilationUnits)

debug: directories
	@$(Build) $(Debug) $(BuildFlags) $(CompilationUnits)
	@gdb $(OutputFile)

run: directories build
	@cd $(BinDir) && ./$(OutputFile)

valgrind: directories build
	@valgrind --leak-check=full $(OutputFile)

valgrindv: directories build
	@valgrind --leak-check=full -v $(OutputFile)


clean:
	rm -f $(OutputFile)
