CFLAGS  = -g -ggdb -Wall
LFLAGS = 
SERVERDIR = server
OBJDIR = $(SERVERDIR)/obj
SRCDIR = $(SERVERDIR)/src
EXEC = serve
DEPS := $(wildcard server/include/*.h)
OBJS := $(patsubst  $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(wildcard $(SRCDIR)/*.c))

all: $(EXEC)

$(OBJDIR):
	[ -d $(OBJDIR) ] || mkdir $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS) | $(OBJDIR)
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)