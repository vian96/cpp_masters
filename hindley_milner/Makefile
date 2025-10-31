CC = clang++
INCLUDE = -I../common/
CFLAGS = -std=c++23 -Wall -Wextra -Wno-multichar $(INCLUDE) -MMD -MP
BUILDDIR = build
SRCDIR = .

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(addprefix $(BUILDDIR)/,$(SRCS:$(SRCDIR)/%.cpp=%.o))

all: mkdir main

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILDDIR)/main

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

mkdir:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

run: all
	$(BUILDDIR)/main

cleanrun: clean run


-include $(OBJS:.o=.d)
