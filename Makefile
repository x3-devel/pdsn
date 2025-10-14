CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Iinclude -Isrc -fsanitize=address -g -O1
LDFLAGS := 
AR      := ar
ARFLAGS := rcs

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := $(OBJ_DIR)/bin
LIBNAME := libpdsn.a
PROGNM  := pdsn

SRC_LIB := $(SRC_DIR)/pdsn.c $(SRC_DIR)/regex.c $(SRC_DIR)/format.c
OBJ_LIB := $(SRC_LIB:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_MAIN := $(OBJ_DIR)/main.o

TAG = $(shell git describe --abbrev=0 --tags)

GIT_DESCRIBE = $(shell git describe | sed 's/-/./g;s/^v//;')

VERSION = $(shell if test -f VERSION; then cat VERSION; else echo -n $(GIT_DESCRIBE) ; fi)

# program version name
PV_NAME := $(PROGNM)-$(TAG)

# hash format
HASH_ALGO := sha256

# releae file format
RF_FMT := tar.gz

# releae file
RELEASE_FILE := $(PV_NAME).$(RF_FMT)


.PHONY: all clean dirs

all: dirs $(LIBNAME) $(PROGNM)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(LIBNAME): $(OBJ_LIB)
	$(AR) $(ARFLAGS) $(BIN_DIR)/$@ $^

$(PROGNM): $(OBJ_MAIN) $(BIN_DIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: main.c include/pdsn.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(LIBNAME) $(PROGNM)

.PHONY:
release-verify:

# check for releases
ifeq ($(shell ls -A "releases/"),)
	$(error "No releases found.")
endif

# loop over release archives, check their hash and verify their signature
	@for f in releases/*.${RF_FMT}; do \
			cd releases/ && sha256sum -c "$$(basename $${f}).$(HASH_ALGO)" && cd ..; \
			gpg --verify "$$f.sig" "$$f" "$$f.$(HASH_ALGO)" 2> /dev/null; \
	done

release:

	@echo -n "$(GIT_DESCRIBE)" > VERSION
	@mkdir -p releases

# only generate release files if the do not exist
ifeq ($(wildcard releases/$(RELEASE_FILE)),)
	@echo "Generating release archive '$(RELEASE_FILE)'";
	@git archive --prefix=$(PV_NAME)/ --add-file=VERSION -o releases/$(RELEASE_FILE) $(TAG);
endif

ifeq ($(wildcard releases/$(RELEASE_FILE).$(HASH_ALGO)),)
	@echo "Generating release hashsum '$(RELEASE_FILE).${HASH_ALGO}'";
	@cd releases/ && $(HASH_ALGO)sum ${RELEASE_FILE} > ${RELEASE_FILE}.${HASH_ALGO} && cd ..;
endif

ifeq ($(wildcard releases/$(RELEASE_FILE).sig),)
	@echo "Generating release sigfile '$(RELEASE_FILE).sig'";
	@gpg --detach-sign -u CD32AC4A85BA80B6 -o releases/${RELEASE_FILE}.sig releases/${RELEASE_FILE} releases/${RELEASE_FILE}.${HASH_ALGO};
endif

	gh release create ${TAG} -t "pdsn ${TAG} release"
	gh release upload ${TAG} releases/$(RELEASE_FILE) releases/$(RELEASE_FILE).$(HASH_ALGO) releases/$(RELEASE_FILE).sig 
