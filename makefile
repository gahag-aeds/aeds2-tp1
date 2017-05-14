SrcDir = src
ScriptDir = scripts
BinDir = bin

LibAeds = $(SrcDir)/libaeds

SettingsFile = default_settings.cfg
SettingsCombinationsFile = settings_combinations.txt
CombinationsResultsFile = combinations_results.txt
SortedResultsFile = sorted_results.txt

CombinationsFile = settings_combinations.txt
GenCombinationsScript = $(ScriptDir)/gen_combinations.sh
RunCombinationsScript = $(ScriptDir)/run_combinations.sh
SortResultsScript = $(ScriptDir)/sort_results.sh


OutputFileName   = restaurant
OutputFile       = $(BinDir)/$(OutputFileName)
CompilationUnits = $(LibAeds)/*.c					\
									 $(LibAeds)/adt/*.c			\
									 $(LibAeds)/io/*.c      \
									 $(LibAeds)/memory/*.c  \
									 $(SrcDir)/util/*.c			\
									 $(SrcDir)/*.c


Build       = clang
Standard    = c99
Warnings    = -Wall -Wextra
Optimize    = -flto -O2
Debug       = -g

BuildFlags  = $(Warnings)		    \
              -std=$(Standard)	\
							$(Optimize)       \
              -I $(SrcDir)      \
              -o $(OutputFile)

ReleaseFlags = -Wall            \
               -std=$(Standard)	\
							 -DNDEBUG -s			\
							 $(Optimize)      \
               -I $(SrcDir)     \
               -o $(OutputFile)



directories: $(SrcDir)
	@[ -d "$(BinDir)" ] || mkdir "$(BinDir)";


build: directories
	@$(Build) $(BuildFlags) $(CompilationUnits)

release: directories
	@$(Build) $(ReleaseFlags) $(CompilationUnits)


run: directories build
	@$(OutputFile) $(SettingsFile)


valgrind: directories build
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(OutputFile) $(SettingsFile)

debug: directories
	@$(Build) $(Debug) $(BuildFlags) $(CompilationUnits)
	@gdb --args $(OutputFile) $(SettingsFile)


gencomb: $(ScriptDir)
	@$(GenCombinationsScript) > $(SettingsCombinationsFile)

runcomb: $(ScriptDir) gencomb release
	@$(RunCombinationsScript) $(OutputFile) $(CombinationsFile) > $(CombinationsResultsFile)

sortres: $(ScriptDir)
	@$(SortResultsScript) $(CombinationsResultsFile) > $(SortedResultsFile)


clean:
	rm -f $(OutputFile)
