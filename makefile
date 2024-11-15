#defines a search function.
define search #(1 path, 2 pattern) -> list
    $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call search,$d/,$2))
endef

SCM-DIR := ./scm
BLD-DIR := ./build

#builds a list of files that looks like `a.scm b.scm c.scm` and `a.so b.so c.so`
SCM-LST := $(call search,$(SCM-DIR),*.scm)
SSO-LST := $(SCM-LST:$(SCM-DIR)%.scm=$(BLD-DIR)%.so)

SOURCES = main.cpp ResourceManager.cpp UboManager.cpp Camera.cpp

game.boot: $(SSO-LST)
  #compile .so files into .boot file
  
$(BLD-DIR)/%.so: $(SCM-DIR)/%.scm
  #compile scm files into so files
  