
# Compile with GCC
CC	:=gcc
# Use GNU Assembly, this also
# allows usage of macro, compared to GNU as
AS	:=gcc
# Link with gcc
LD	:=gcc

CC_DEBUG_FLAGS	:=-g

__CDEPS	=\
echo -n $(@D)/ > $@ ;\
$(CC) -MM $< \
$(CF_ALL) \
$(CF_$(patsubst %.c,$(DIR_OBJS)/%.o,$<)) \
$(CF_$(patsubst %/,%,$(dir $<))) \
>> $@

CDEPS =$(call style,$(__CDEPS),[DP] $@)