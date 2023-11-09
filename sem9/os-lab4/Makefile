.ONESHELL:

CFLAGS = -g -Wall -Wpointer-arith -Werror
LDFLAGS = -lpthread

EXERCISE = 1


build/%.o: src/%.c
	@
	mkdir -p $(@D)
	gcc -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@
.PHONY: build/%.o

build/%.run: build/%.o
	@
	gcc $(LDFLAGS) $^ -o $@
.PHONY: build/%.run


pnb: build/peru_and_bolivia_e$(EXERCISE).run
	@
	./$^
.PHONY: pnb

po: build/passing_objects_e$(EXERCISE).run
	@
	./$^
.PHONY: pnb

b: build/barrier_e$(EXERCISE).run
	@
	./$^
.PHONY: b


clean:
	@
	rm -rf build
.PHONY: clean
