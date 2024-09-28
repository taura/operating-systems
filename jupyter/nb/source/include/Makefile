
c:=$(wildcard os03_*/*.c)

versioned_c:=\
$(foreach vc,$(patsubst %.c,%,$(c)),\
$(foreach v,$(shell seq 1 $(shell ./mk_version.py --mode comment -DVER=0 $(vc).c)),\
versioned/$(vc)_$(v).c))

versioned_dir:=$(sort $(foreach x,$(c),versioned/$(dir $(x))))

c_exe:=$(patsubst versioned/%.c,versioned/%.exe,$(versioned_c))

# $(error $(versioned_c))
# $(error $(versioned_dir))
# $(error $(c_exe))

targets:=
targets+=$(c_exe)

all : $(targets)

define rule
versioned/$(vc)_$(v).exe : versioned/%.exe : versioned/%.c
	gcc -Wall $$< -o $$@
versioned/$(vc)_$(v).c : versioned/%_$(v).c : %.c $$(dir versioned/$(vc)_$(v).c)
	./mk_version.py -D VER=$(v) $$< > $$@
endef

$(foreach vc,$(patsubst %.c,%,$(c)),\
$(foreach v,$(shell seq 1 $(shell ./mk_version.py --mode comment -DVER=0 $(vc).c)),\
$(eval $(call rule))))

$(versioned_dir) : % :
	mkdir -p $@
