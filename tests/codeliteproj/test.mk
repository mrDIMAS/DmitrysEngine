##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Linux_Debug
ProjectName            :=test
ConfigurationName      :=Linux_Debug
WorkspacePath          :=C:/DmitrysEngine/DmitrysEngine/tests/codeliteproj
ProjectPath            :=C:/DmitrysEngine/DmitrysEngine/tests/codeliteproj
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=mrDIMAS
Date                   :=09/03/2019
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/MinGW-8.1.0/bin/g++.exe
SharedObjectLinkerName :=C:/MinGW-8.1.0/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=../bin/test.exe
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="test.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/MinGW-8.1.0/bin/windres.exe
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../../. $(IncludeSwitch)../../external/ 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)GL $(LibrarySwitch)pthread $(LibrarySwitch)asound $(LibrarySwitch)X11 $(LibrarySwitch)Xrandr 
ArLibs                 :=  "GL" "pthread" "asound" "X11" "Xrandr" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/MinGW-8.1.0/bin/ar.exe rcu
CXX      := C:/MinGW-8.1.0/bin/g++.exe
CC       := C:/MinGW-8.1.0/bin/gcc.exe
CXXFLAGS :=   $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall -std=c99 -pedantic -Werror  $(Preprocessors)
ASFLAGS  := 
AS       := C:/MinGW-8.1.0/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/up_src_main.c$(ObjectSuffix) $(IntermediateDirectory)/up_up_de_main.c$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "./Debug"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_src_main.c$(ObjectSuffix): ../src/main.c $(IntermediateDirectory)/up_src_main.c$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/DmitrysEngine/DmitrysEngine/tests/src/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_src_main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_src_main.c$(DependSuffix): ../src/main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_src_main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/up_src_main.c$(DependSuffix) -MM ../src/main.c

$(IntermediateDirectory)/up_src_main.c$(PreprocessSuffix): ../src/main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_src_main.c$(PreprocessSuffix) ../src/main.c

$(IntermediateDirectory)/up_up_de_main.c$(ObjectSuffix): ../../de_main.c $(IntermediateDirectory)/up_up_de_main.c$(DependSuffix)
	$(CC) $(SourceSwitch) "C:/DmitrysEngine/DmitrysEngine/de_main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_up_de_main.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_up_de_main.c$(DependSuffix): ../../de_main.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_up_de_main.c$(ObjectSuffix) -MF$(IntermediateDirectory)/up_up_de_main.c$(DependSuffix) -MM ../../de_main.c

$(IntermediateDirectory)/up_up_de_main.c$(PreprocessSuffix): ../../de_main.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_up_de_main.c$(PreprocessSuffix) ../../de_main.c


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


