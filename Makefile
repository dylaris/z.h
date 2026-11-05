BIN_DIR := output
SRC_DIR := examples
TEST_DIR := tests
TEST_BIN_DIR := $(BIN_DIR)/tests
CC      := cc
CFLAGS  := -Wall -Wextra -I. -g

ifeq ($(OS),Windows_NT)
SHELL := cmd.exe
MKDIR = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
RMDIR = if exist "$(subst /,\,$(1))" rmdir /S /Q "$(subst /,\,$(1))"
RUN = "$(subst /,\,$(1))"
EXE_EXT := .exe
else
MKDIR = mkdir -p "$(1)"
RMDIR = rm -rf "$(1)"
RUN = ./$(1)
EXE_EXT :=
endif

TARGETS := $(BIN_DIR)/array$(EXE_EXT) $(BIN_DIR)/deque$(EXE_EXT) \
           $(BIN_DIR)/string$(EXE_EXT) $(BIN_DIR)/log$(EXE_EXT) \
           $(BIN_DIR)/lsdot$(EXE_EXT)
TEST_TARGETS := $(TEST_BIN_DIR)/test_array$(EXE_EXT) \
                $(TEST_BIN_DIR)/test_deque$(EXE_EXT) \
                $(TEST_BIN_DIR)/test_string$(EXE_EXT) \
                $(TEST_BIN_DIR)/test_temp$(EXE_EXT) \
                $(TEST_BIN_DIR)/test_log$(EXE_EXT) \
                $(TEST_BIN_DIR)/test_misc$(EXE_EXT)

all: $(TARGETS)

$(BIN_DIR):
	@$(call MKDIR,$@)

$(TEST_BIN_DIR):
	@$(call MKDIR,$@)

$(BIN_DIR)/%$(EXE_EXT): $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(TEST_BIN_DIR)/%$(EXE_EXT): $(TEST_DIR)/%.c $(TEST_DIR)/test.h z.h | $(TEST_BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

test: $(TEST_TARGETS)
	@echo RUN array tests
	@$(call RUN,$(TEST_BIN_DIR)/test_array$(EXE_EXT))
	@echo RUN deque tests
	@$(call RUN,$(TEST_BIN_DIR)/test_deque$(EXE_EXT))
	@echo RUN string tests
	@$(call RUN,$(TEST_BIN_DIR)/test_string$(EXE_EXT))
	@echo RUN temp allocator tests
	@$(call RUN,$(TEST_BIN_DIR)/test_temp$(EXE_EXT))
	@echo RUN log tests
	@$(call RUN,$(TEST_BIN_DIR)/test_log$(EXE_EXT))
	@echo RUN misc tests
	@$(call RUN,$(TEST_BIN_DIR)/test_misc$(EXE_EXT))

ifeq ($(OS),Windows_NT)
help:
	@echo Usage: make ^<target^>
	@echo.
	@echo Targets:
	@echo   all   Build all examples
	@echo   test  Build and run all tests
	@echo   clean Remove generated output
	@echo   help  Show this help
else
help:
	@echo "Usage: make <target>"
	@echo ""
	@echo "Targets:"
	@echo "  all   Build all examples"
	@echo "  test  Build and run all tests"
	@echo "  clean Remove generated output"
	@echo "  help  Show this help"
endif

clean:
	@$(call RMDIR,$(BIN_DIR))

.PHONY: all clean help test
