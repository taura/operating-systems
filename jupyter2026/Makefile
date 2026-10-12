MAKEFLAGS += --warn-undefined-variables

md_files :=
md_files += os00_intro
md_files += os01_install_linux
md_files += os02_process
md_files += os03_thread
md_files += os04_sched
# md_files += os05_mutex
# md_files += os06_cond_var
# md_files += os07_atomic
# md_files += os08_addr
# md_files += os09_vm
# md_files += os10_cache
# md_files += os11_prefetch
# md_files += os12_mmap
# md_files += os13_mmap_app
# md_files += os14_shell
# md_files += os15_signal
# md_files += os16_security

aux_files := 
aux_files += $(wildcard nb/source/os00_intro/img/*.png)
aux_files += $(wildcard nb/source/os01_install_linux/img/*.png)

EVAL=$(eval $(1))
#EVAL=$(info $(1))

generate :
ipynb :

define dir_rule
assignments/source/$(asgn)/dir assignments/source/$(asgn)_ans/dir : % :
	mkdir -p $$@
endef

define generate_rule
generate : assignments/release/$(asgn)/generated
generate : assignments/release/$(asgn)_ans/generated
assignments/release/$(asgn)/generated : % :
	uv run nbgrader generate_assignment $(asgn) --force --CourseDirectory.root=assignments
	touch $$@
assignments/release/$(asgn)_ans/generated : % :
	uv run nbgrader generate_assignment $(asgn)_ans --force --CourseDirectory.root=assignments
	touch $$@
endef

#
# markdown -> ipynb
#
define nb_to_ipynb_rule
ipynb : assignments/source/$(asgn)/$(file).ipynb
ipynb : assignments/source/$(asgn)_ans/$(file).ipynb
assignments/release/$(asgn)/generated : assignments/source/$(asgn)/$(file).ipynb
assignments/release/$(asgn)/generated : assignments/source/$(asgn)_ans/$(file).ipynb
assignments/source/$(asgn)/$(file).ipynb : nb/source/$(asgn)/$(file).md assignments/source/$(asgn)/dir
	mkdir -p $$(dir $$@)
	uv run ./mk_nb.py --output $$@ --output-kernel miyabi $$<
assignments/source/$(asgn)_ans/$(file).ipynb : nb/source/$(asgn)/$(file).md assignments/source/$(asgn)_ans/dir
	mkdir -p $$(dir $$@)
	uv run ./mk_nb.py --output $$@ --output-kernel miyabi $$< --label ans
endef

#
# split path into assignment and file name, e.g.
# pd00_intro/img/a.png -> pd00_intro  img/a.png
#
aux_file_asgn = $(firstword $(subst /, ,$(patsubst nb/source/%,%,$(aux_file))))
aux_file_file = $(patsubst $(call aux_file_asgn)/%,%,$(patsubst nb/source/%,%,$(aux_file)))
md_file_asgn = $(firstword $(subst /, ,$(patsubst nb/source/%,%,$(asgn_md))))
md_file_file = $(patsubst $(call md_file_asgn)/%.md,%,$(patsubst nb/source/%,%,$(asgn_md)))

#
# copy aux files
#
define aux_file_rule
ipynb : assignments/source/$(asgn)/$(file)
ipynb : assignments/source/$(asgn)_ans/$(file)
assignments/release/$(asgn)/generated : assignments/source/$(asgn)/$(file)
assignments/release/$(asgn)/generated : assignments/source/$(asgn)_ans/$(file)
assignments/source/$(asgn)/$(file) : nb/source/$(asgn)/$(file) assignments/source/$(asgn)/dir
	install -D --mode 0644 $$< $$@
assignments/source/$(asgn)_ans/$(file) : nb/source/$(asgn)/$(file) assignments/source/$(asgn)_ans/dir
	install -D --mode 0644 $$< $$@
endef

assignments := $(sort $(foreach asgn_md,$(md_files),$(md_file_asgn)))
$(foreach asgn,$(assignments),\
  $(call EVAL,$(call dir_rule)))

$(foreach asgn_md,$(md_files),\
  $(let asgn file,$(md_file_asgn) $(md_file_file),\
    $(call EVAL,$(call nb_to_ipynb_rule))))

$(foreach aux_file,$(aux_files),\
  $(let asgn file,$(aux_file_asgn) $(aux_file_file),\
    $(call EVAL,$(call aux_file_rule))))

$(foreach asgn,$(assignments),\
  $(call EVAL,$(call generate_rule)))


.PHONY : ipynb generate 
