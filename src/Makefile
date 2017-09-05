LIBSENT=../julius/libsent
#LIBSENT=../libsent

LIBJULIUS=../julius/libjulius
#LIBJULIUS=../libjulius

INCLUDE_PATH=-I./include -lcurl -std=c++11

ALL_FILE=TinyRetail_Julius.cpp TinyRetail_main.cpp Julius_Thread.cpp ThreadJob.cpp Post_curl.cpp JSON.cpp

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

tinyRetail: $(ALL_FILE)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE_PATH) -o TinyRetail $(ALL_FILE) $(LDFLAGS)

clean:
	$(RM) *.o *.bak *~ core TAGS

distclean:
	$(RM) *.o *.bak *~ core TAGS
	$(RM) TinyRetail
