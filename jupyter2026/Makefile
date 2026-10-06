assignments :=
assignments += os00_intro
assignments += os01_install_linux
assignments += os02_process
# assignments += os03_thread
# assignments += os04_sched
# assignments += os05_mutex
# assignments += os06_cond_var
# assignments += os07_atomic
# assignments += os08_addr
# assignments += os09_vm
# assignments += os10_cache
# assignments += os11_prefetch
# assignments += os12_mmap
# assignments += os13_mmap_app
# assignments += os14_shell
# assignments += os15_signal
# assignments += os16_security

aux_files := 
aux_files += $(wildcard nb/source/os00_intro/img/*.png)
aux_files += $(wildcard nb/source/os01_install_linux/img/*.png)

ipynb :

# os00_intro/img/a.png -> os00_intro  img/a.png
aux_file_asgn = $(firstword $(subst /, ,$(patsubst nb/source/%,%,$(aux_file))))
aux_file_file = $(patsubst $(call aux_file_asgn)/%,%,$(patsubst nb/source/%,%,$(aux_file)))

define aux_file_rule
ipynb : notebooks/source/$(asgn)/$(file)
ipynb : notebooks/source/$(asgn)_ans/$(file)
notebooks/source/$(asgn)/$(file) : nb/source/$(asgn)/$(file) notebooks/source/$(asgn)/dir
	install -D --mode 0644 $$< $$@
notebooks/source/$(asgn)_ans/$(file) : nb/source/$(asgn)/$(file) notebooks/source/$(asgn)/dir
	install -D --mode 0644 $$< $$@
endef

define nb_to_ipynb_rule
ipynb : notebooks/source/$(asgn)/$(asgn).ipynb
ipynb : notebooks/source/$(asgn)_ans/$(asgn)_ans.ipynb
notebooks/source/$(asgn)/dir notebooks/source/$(asgn)_ans/dir : % :
	mkdir -p $$@
notebooks/source/$(asgn)/$(asgn).ipynb : nb/source/$(asgn)/$(asgn).md notebooks/source/$(asgn)/dir
	uv run ./mk_nb.py --output $$@ --output-kernel py $$<
notebooks/source/$(asgn)_ans/$(asgn)_ans.ipynb : nb/source/$(asgn)/$(asgn).md notebooks/source/$(asgn)_ans/dir
	uv run ./mk_nb.py --output $$@ --output-kernel py $$< --label ans
endef

$(foreach asgn,$(assignments),\
  $(eval $(call nb_to_ipynb_rule)))

$(foreach aux_file,$(aux_files),\
  $(let asgn file,$(aux_file_asgn) $(aux_file_file),\
    $(eval $(call aux_file_rule))))

