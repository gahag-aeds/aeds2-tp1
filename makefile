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
BuildFlags  = -Wall             \
              -std=$(Standard)	\
							$(Optimize)       \
              -I $(SrcDir)      \
              -o $(OutputFile)


directories: $(SrcDir)
	@[ -d "$(BinDir)" ] || mkdir "$(BinDir)";

build: directories
	@$(Build) $(BuildFlags) $(CompilationUnits)

run: directories build
	@cd $(BinDir) && ./$(OutputFileName)

valgrind: directories build
	@valgrind --leak-check=full $(BinDir)/$(OutputFileName)

valgrindv: directories build
	@valgrind --leak-check=full -v $(BinDir)/$(OutputFileName)


clean:
	rm -f $(OutputFile)
