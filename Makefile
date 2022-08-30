c_compiler := gcc
linker := gcc
linker_options :=
libraries := 
library_paths := 
user_cflags :=

target_executable := air_protocol

makefile_name := $(lastword $(MAKEFILE_LIST))
makefile_path := $(dir $(abspath $(makefile_name)))
# Get path where the Application is
build_root_path := $(abspath $(makefile_path))

object_output_path := $(build_root_path)/obj
binary_output_path := $(build_root_path)/bin

# Finds files that matches the specified pattern. The directory list
# is searched recursively. It is safe to invoke this function with an
# empty list of directories.
#
# Param $(1): List of directories to search
# Param $(2): The file pattern to search for
define find
	$(foreach dir,$(1),$(foreach d,$(wildcard $(dir)/*),\
		$(call find,$(d),$(2))) $(wildcard $(dir)/$(strip $(2))))
endef
unexport find

source_paths := $(build_root_path)
include_paths := $(source_paths)
c_source_files := $(call find, $(source_paths),*.c)
object_files := $(c_source_files:%.c=$(object_output_path)/%.o)

$(binary_output_path)/$(target_executable): $(object_files)
	@echo Linking $(@)
	@mkdir -p $(@D)		
	@$(shell mkdir -p $(object_output_path))
	@$(shell echo "$(object_files)">$(object_output_path)/objects.tmp)
	@$(linker) \
		$(linker_options) \
		$(patsubst %,-L%,$(library_paths)) \
		@$(object_output_path)/objects.tmp -o $@ \
		-Wl,--start-group $(patsubst %,-l%,$(libraries)) -Wl,--end-group
	@rm -f $(object_output_path)/objects.tmp

$(object_output_path)/%.o: %.c
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(c_compiler) $(user_cflags) $(patsubst %,-I%,$(include_paths)) -c $< -o $@
		
.PHONY: $(binary_output_path)/$(target_executable) clean

clean:
	@echo Cleaning
	@rm -rf $(binary_output_path)
	@rm -rf $(object_output_path)