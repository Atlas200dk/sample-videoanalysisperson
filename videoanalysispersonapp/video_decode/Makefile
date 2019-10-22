all : libvideo_decode.so
#HOST COMPILER		
ifndef DDK_HOME
$(error "Can not find DDK_HOME env, please set it in environment!.")
endif

LOCAL_DIR  := ./

SRC_DIR = $(LOCAL_DIR)
BUILD_DIR = tmp
OUT_DIR = ../out
OBJ_DIR = $(BUILD_DIR)/obj
DEPS_DIR  = $(BUILD_DIR)/deps

INC_DIR = \
	-I$(SRC_DIR) \
	-I$(DDK_HOME)/include/inc \
	-I$(DDK_HOME)/include/inc/custom \
	-I$(DDK_HOME)/include/third_party/opencv/include \
	-I$(DDK_HOME)/include/third_party/protobuf/include \
	-I$(DDK_HOME)/include/third_party/cereal/include \
	-I$(DDK_HOME)/include/libc_sec/include \
	-I$(HOME)/ascend_ddk/include/third_party/ffmpeg \
	-I../common/include \
	-I/usr/include/python2.7

CC_FLAGS := $(INC_DIR) -g -std=c++11 -fPIC

ifeq ($(mode),)
mode=AtlasDK
endif

ifeq ($(mode), AtlasDK)
CC := aarch64-linux-gnu-g++
LNK_FLAGS := \
	-L$(HOME)/ascend_ddk/host/lib -L$(DDK_HOME)/host/lib \
	-lmedia_mini \
	-lavcodec \
	-lavformat \
	-lavdevice \
	-lavutil \
	-lswresample \
	-lavfilter \
	-lswscale \
	-shared
else ifeq ($(mode), ASIC)
CC := g++
LNK_FLAGS := \
	-L$(HOME)/ascend_ddk/host/lib	-L$(DDK_HOME)/host/lib/ \
	-lavcodec \
	-lavformat \
	-lavdevice \
	-lavutil \
	-lswresample \
	-lavfilter \
	-lswscale \
	-shared
else
$(error "Unsupported mode: "$(mode)", please input: AtlasDK or ASIC.")
endif



DIRS := $(shell find $(SRC_DIR) -maxdepth 3 -type d)
CUSTOM_DIRS := $(shell find $(SRC_DIR) -maxdepth 3 -type d)

VPATH = $(DIRS)

SOURCES  = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
CUSTOM_SOURCES  = $(foreach dir, $(CUSTOM_DIRS), $(wildcard $(dir)/*.cpp))
OBJS   = $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%.o,$(notdir $(SOURCES))))
OBJS_customop = $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%.o,$(notdir $(CUSTOM_SOURCES))))
OBJS_no_customop := $(filter-out $(OBJS_customop), $(OBJS))
DEPS  = $(addprefix $(DEPS_DIR)/, $(patsubst %.cpp,%.d,$(notdir $(SOURCES))))

libvideo_decode.so: $(OBJS_customop)
	$(CC) $^ $(LNK_FLAGS) -o $@
	rm -rf $(BUILD_DIR)

$(OBJ_DIR)/%.o:%.cpp
	@if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi;
	$(CC) -c $(CC_FLAGS) -o $@ $<


$(DEPS_DIR)/%.d:%.cpp
	@if [ ! -d $(DEPS_DIR) ]; then mkdir -p $(DEPS_DIR); fi;
	set -e; rm -f $@;
	$(CC) -MM $(CC_FLAGS) $< > $@.$$$$;
	sed 's,\($*\)\.o[ :]*,$(OBJ_DIR)/\1.o $@ : ,g' < $@.$$$$ > $@;
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS), clean)
	-include $(DEPS)
endif

.PHONY : clean install
clean:
	rm -rf $(BUILD_DIR) lib*.so *.o
install: libvideo_decode.so
	mv *.so $(OUT_DIR)
