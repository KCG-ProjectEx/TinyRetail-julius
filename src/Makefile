LIBSENT=../julius/libsent
#LIBSENT=../libsent

LIBJULIUS=../julius/libjulius
#LIBJULIUS=../libjulius

INCLUDE_PATH=-I./include

OPTION=-lcurl -std=c++11

OBJS=TinyRetail_Julius.o TinyRetail_main.o Julius_Thread.o ThreadJob.o Curl.o JSON.o split.o negaPosi.o

SRCS=$(OBJS:%.o=%.cpp)

CC=g++
CFLAGS=-g -O2 

####
#### When using system-installed libraries
####
# CPPFLAGS=`libjulius-config --cflags` `libsent-config --cflags`
# LDFLAGS=`libjulius-config --libs` `libsent-config --libs`

####
#### When using within-package libraries
####
CPPFLAGS=-I$(LIBJULIUS)/include -I$(LIBSENT)/include  `$(LIBSENT)/libsent-config --cflags` `$(LIBJULIUS)/libjulius-config --cflags`
LDFLAGS= -L$(LIBJULIUS) `$(LIBJULIUS)/libjulius-config --libs` -L$(LIBSENT) `$(LIBSENT)/libsent-config --libs`

############################################################

# all: julius-simple

# julius-simple: julius-simple.cpp
# 	$(CC) $(CFLAGS) $(CPPFLAGS) -o julius-simple julius-simple.cpp $(LDFLAGS)

# clean:
# 	$(RM) *.o *.bak *~ core TAGS

# distclean:
# 	$(RM) *.o *.bak *~ core TAGS
# 	$(RM) julius-simple

all: tinyRetail
tinyRetail: $(CBJS) $(SRCS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE_PATH) -o TinyRetail $(SRCS) $(LDFLAGS) $(OPTION)

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) TinyRetail
