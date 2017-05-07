SrcDir = src
BinDir = bin
LibAeds = $(SrcDir)/libaeds
SettingsFile = settings.cfg

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
	@gdb --args $(OutputFile) $(SettingsFile)

run: directories build
	@$(OutputFile) $(SettingsFile)

valgrind: directories build
	@valgrind --leak-check=full --show-leak-kinds=all $(OutputFile) $(SettingsFile)

valgrindv: directories build
	@valgrind --leak-check=full --show-leak-kinds=all -v $(OutputFile) $(SettingsFile)


clean:
	rm -f $(OutputFile)
