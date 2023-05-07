PROJECTNAME=mist-lib

PLATFORM?=native
BUILDTYPE?=debug
CC?=
CFLAGS?=-Wall -DWINDOW_WIDTH=1280 -DWINDOW_HEIGHT=720 -DWINDOW_TITLE=\""Mist Lib\"" -std=gnu17 -I include -I third-party/glad/include -I third-party/stb/include -I third-party/cglm/include -I third-party/cJSON -I third-party/hashmap.c
LDFLAGS?=-lglfw3
ASSETSDIR=assets
EXECUTABLE=$(BINDIR)/$(PROJECTNAME)
BUILDDIR?=bin
BINDIR=$(BUILDDIR)/$(PLATFORM)

ifeq ($(PLATFORM),native)
	CC=clang
	CFLAGS+=-D_PLATFORM_NATIVE
else ifeq ($(PLATFORM),web)
	CC=emcc
	CFLAGS+=-D_PLATFORM_WEB
	EXECUTABLE=$(BINDIR)/$(PROJECTNAME).html
	LDFLAGS+=-sUSE_GLFW=3 -sWASM=1 -sMAX_WEBGL_VERSION=2 -sALLOW_MEMORY_GROWTH=1 -sNO_EXIT_RUNTIME=0 -sASSERTIONS=1 --preload-file $(ASSETSDIR) --shell-file $(ASSETSDIR)/index.html
endif

ifeq ($(BUILDTYPE),debug)
	CFLAGS += -g
else ifeq ($(BUILDTYPE),release)
	CFLAGS += -O1
endif

SRCDIRS=src third-party/glad/src third-party/stb/src third-party/cglm/src
SOURCES+=third-party/cJSON/cJSON.c third-party/hashmap.c/hashmap.c
SOURCES+=$(shell find $(SRCDIRS) -name '*.c')

OBJDIR=$(BINDIR)/obj
OBJECTS=$(foreach file,$(SOURCES),$(patsubst %.c,$(OBJDIR)/%.o,$(file)))

all: buildinfo prebuild $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(info Linking $@)
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.c
	$(info $<)
	@mkdir -p $(shell dirname $@)
	@$(CC) -c $(CFLAGS) $< -o $@

buildinfo:
	$(info === Building $(PROJECTNAME) ($(BUILDTYPE)) ===)
	$(info C Compiler: $(CC))
	$(info Platform: $(PLATFORM))

prebuild:
	$(info === Running Prebuild Commands ===)
	mkdir -p $(BUILDDIR) $(BINDIR) $(OBJDIR)
	cp -r $(ASSETSDIR) $(BINDIR)

run: all
	./$(EXECUTABLE)

clean:
	$(info === Removing $(BUILDDIR) ===)
	rm -rf $(BUILDDIR)

host:
	python -m http.server --directory $(BINDIR)

.PHONY: buildinfo host prebuild run clean
