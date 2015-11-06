program_NAME := myprogram

program_C_SRCS := $(wildcard *.cpp)
program_C_SRCS += $(wildcard App42/App42HTTP/*.cpp)
program_C_SRCS += $(wildcard App42/cJSON/*.cpp)
program_C_SRCS += $(wildcard App42/Common/*.cpp)
program_C_SRCS += $(wildcard App42/Crypto/*.cpp)
program_C_SRCS += $(wildcard App42/Util/*.cpp)
program_C_SRCS += $(wildcard App42/PushNotificationService/*.cpp)
program_C_SRCS += $(wildcard App42/EventService/*.cpp)
program_C_SRCS += $(wildcard App42/StorageService/*.cpp)

program_C_OBJS := ${program_C_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) 

program_INCLUDE_DIRS := App42/App42HTTP
program_INCLUDE_DIRS += App42/cJSON
program_INCLUDE_DIRS += App42/Common
program_INCLUDE_DIRS += App42/Crypto
program_INCLUDE_DIRS += App42/Util
program_INCLUDE_DIRS += App42/PushNotificationService
program_INCLUDE_DIRS += App42/EventService
program_INCLUDE_DIRS += App42/StorageService

program_LIBRARY_DIRS := 
program_LIBRARIES := curl

CPPFLAGS := -frtti -std=c++11 -fsigned-char
CPPFLAGS += -fexceptions
CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all clean distclean

all: $(program_NAME)

$(program_NAME): $(program_C_OBJS)
	$(LINK.cc) $(program_C_OBJS) -o $(program_NAME)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)
	echo Clean done
distclean: clean

