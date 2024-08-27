LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := test_app_native_renderer
LOCAL_SRC_FILES := test_gl_native.cpp
LOCAL_LDLIBS := -llog -lEGL -landroid -lGLESv3 -lmediandk

include $(BUILD_SHARED_LIBRARY)