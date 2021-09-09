# Custom shared libraries
SHARED_LIBS = utils

# ===============================================================================================================

# All directories on workspace
DIRECTORIES = $(shell find . -maxdepth 1 -type d -name '[!.]*' | tr './\n' ' ')

# Projects (no shared libraries)
MODULES = $(filter-out $(SHARED_LIBS), $(DIRECTORIES))

# A base project use to create new projects with make project
BASE_PROJECT =$(strip $(word 1, $(MODULES)))

print:
	@echo $(DIRECTORIES)
	@echo $(MODULES)
	@echo $(BASE_PROJECT)

# Name of workspace file
WORKSPACE = $(word 1,$(shell find . -maxdepth 1 -name *.code-workspace | tr './' ' '))

# ===============================================================================================================

# Subdirectories of project
SRC=src
TESTS = src/tests
CONFIG=cfg
.VSCODE=.vscode

# Tasks and Debugg
UTILS_TASKS = utils/.vscode/tasks.json
TASKS = .vscode/tasks.json
DEBUG = .vscode/launch.json 

# SED References
SED_WORKSPACE_DIRS = workspace_folders:
SED_DEPENDANCIES = dependencies:
SED_TASK =build_tasks:

# ===============================================================================================================

all:
	@for dir in $(DIRECTORIES);		\
	do 								\
		cd $$dir; 					\
		make -s $@; 				\
		cd ..; 						\
	done; 							\
	echo "Make $@ was succesfull"

#p: project
project:
	@mkdir $(name) $(name)/$(SRC) $(name)/$(CONFIG)  $(name)/$(TESTS)
	
	@touch $(name)/$(CONFIG)/$(name).config  	\
	       $(name)/$(CONFIG)/$(name).log		\
		   $(name)/$(SRC)/main.c 				\
		   $(name)/$(SRC)/main.h 				\
		   $(name)/$(TESTS)/tests.c 			\
		   $(name)/$(TESTS)/tests.h

	@cp -R $(BASE_PROJECT)/.vscode $(name)
	@cp -r $(BASE_PROJECT)/makefile $(name)/makefile

	@sed -i 's/$(BASE_PROJECT)/$(name)/g' $(name)/$(TASKS)	
	@sed -i 's/$(BASE_PROJECT)/$(name)/g' $(name)/$(DEBUG)
	@sed -i '/$(SED_WORKSPACE_DIRS)/a \\t	{"path": "$(name)"},' $(WORKSPACE).code-workspace
	@sed -i '/$(SED_TASK)/a 													\
				//$(name)BuildTaskStart											\
			{ 																	\
				"label": "Build $(name) for build all", 						\
				"type": "shell", 												\
				"command": "make all", 											\
				"options": { 													\
					"cwd": "$${workspaceFolder:$(name)}" 	 					\
				}, 																\
				"isBackground" : true,											\
				"group": "build", 												\
				"problemMatcher": { 											\
					"base": "$$gcc", 											\
					"fileLocation": ["relative", "$${workspaceFolder:$(name)}"] \
				}, 																\
				"presentation": { 												\
					"echo": true, 												\
					"reveal": "never", 											\
					"focus": false, 											\
					"panel": "shared",											\
					"showReuseMessage": true,									\
					"clear": false 												\
				},																\
				"dependsOn": ["Build Utils for build all"],						\
			},																	\
				//$(name)BuildTaskEnd' $(UTILS_TASKS)
	@sed -i '/$(SED_DEPENDANCIES)/a \\t			"Build $(name) for build all",' $(UTILS_TASKS)
	
	@echo "$(name) was added"

del: delete
delete: $(BASE_PROJECT)
	@sed -i '/$(name)BuildTaskStart/,/$(name)BuildTaskEnd/d' $(UTILS_TASKS)
	@sed -i '/{"path": "$(name)"},/d' $(WORKSPACE).code-workspace
	@sed -i '/"Build $(name) for build all"/d' $(UTILS_TASKS)

	@rm -fr $(name)

	@echo "$(name) was removed"

clean: 
	@for dir in $(DIRECTORIES); 	\
	do 								\
		cd $$dir; 					\
		make -s $@; 				\
		cd ..; 						\
	done; 							\
	echo "Make $@ was succesfull"

help:
	@echo ""
	@echo "$(BOLDBLUE)COMMANDS: " 
	@echo "     $(BOLDGREEN)make$(NC) / $(BOLDGREEN)make all$(NC)    -- Compiles all projects in current directory"
	@echo "     $(BOLDGREEN)make clean$(NC)         -- Cleans all projects in current directory"
	@echo "   "                        	
	@echo "     $(BOLDGREEN)make project$(NC)       -- Creates a new project with specified name"
	@echo "                           $(BOLDYELLOW)NEEDS ARGUMENT$(NC) - $(MAGENTA)$"name$"$(NC):"
	@echo "                           - Example: $(LIGHTCYAN)make project name=Client$(NC)"
	@echo "  "                         
	@echo "     $(BOLDGREEN)make delete$(NC) /     -- Deletes specified project and all its"
	@echo "     $(BOLDGREEN)make del$(NC)             non necesary estructures in our workspace"
	@echo "                          $(BOLDYELLOW)NEEDS ARGUMENT$(NC) - $(MAGENTA)$"name$"$(NC):"	
	@echo "                           - Example: $(LIGHTCYAN)make delete name=project$(NC)"
	@echo " "


.PHONY: project clean all delete del p

# ===============================================================================================================

NC=\033[0m
RED=\033[1;31m
BOLDGREEN=\033[1;32m
MAGENTA=\033[0;95m
BOLDYELLOW=\033[1;93m
BOLDBLUE=\033[1;34m
NORMAL=\033[0m
BOLD=\033[1m
LIGHTCYAN=\033[0;96m

# ===============================================================================================================