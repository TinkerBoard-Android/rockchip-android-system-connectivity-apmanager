#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

# Definitions applying to all targets. Be sure to $(eval) this last.
define apmanager_common
  LOCAL_CPP_EXTENSION := .cc
  LOCAL_RTTI_FLAG := -frtti
  LOCAL_CLANG := true
  LOCAL_SHARED_LIBRARIES += \
      libchrome \
      libchrome-dbus \
      libchromeos \
      libchromeos-dbus \
      libchromeos-minijail \
      libdbus \
      libfirewalld-client \
      libminijail \
      libshill-net
  LOCAL_C_INCLUDES += \
      $(LOCAL_PATH)/.. \
      external/cros/system_api    # D-Bus service constants.
  LOCAL_CFLAGS += -Wall -Werror -Wno-unused-parameter
  # -Wno-non-virtual-dtor: for generated D-Bus adaptors.
  # -Wno-missing-field-initializers: for LAZY_INSTANCE_INITIALIZER.
  LOCAL_CPPFLAGS += \
      -Wno-sign-promo \
      -Wno-non-virtual-dtor \
      -Wno-missing-field-initializers
endef

ifeq ($(HOST_OS),linux)

# === libapmanager-client (shared library) ===
include $(CLEAR_VARS)
LOCAL_MODULE := libapmanager-client
LOCAL_SRC_FILES := \
    dbus_bindings/dbus-service-config.json \
    dbus_bindings/org.chromium.apmanager.Config.dbus-xml \
    dbus_bindings/org.chromium.apmanager.Device.dbus-xml \
    dbus_bindings/org.chromium.apmanager.Manager.dbus-xml \
    dbus_bindings/org.chromium.apmanager.Service.dbus-xml
LOCAL_DBUS_PROXY_PREFIX := apmanager
include $(BUILD_SHARED_LIBRARY)

# === libapmanager (static library) ===
include $(CLEAR_VARS)
LOCAL_MODULE := libapmanager
LOCAL_SRC_FILES := \
    dbus_bindings/dbus-service-config.json \
    dbus_bindings/org.chromium.apmanager.Config.dbus-xml \
    dbus_bindings/org.chromium.apmanager.Device.dbus-xml \
    dbus_bindings/org.chromium.apmanager.Manager.dbus-xml \
    dbus_bindings/org.chromium.apmanager.Service.dbus-xml \
    config.cc \
    daemon.cc \
    device.cc \
    device_info.cc \
    dhcp_server.cc \
    dhcp_server_factory.cc \
    event_dispatcher.cc \
    file_writer.cc \
    firewall_manager.cc \
    firewalld_dbus_proxy.cc \
    hostapd_monitor.cc \
    manager.cc \
    process_factory.cc \
    service.cc \
    shill_manager.cc \
    shill_stub_proxy.cc
$(eval $(apmanager_common))
include $(BUILD_STATIC_TEST_LIBRARY)

# === apmanager ===
include $(CLEAR_VARS)
LOCAL_MODULE := apmanager
#LOCAL_REQUIRED_MODULES := init.apmanager.rc
LOCAL_SRC_FILES := \
    main.cc
LOCAL_STATIC_LIBRARIES := libapmanager
LOCAL_C_INCLUDES += external/gtest/include
$(eval $(apmanager_common))
include $(BUILD_EXECUTABLE)

# === init.apmanager.rc (brillo only) ===
ifdef TARGET_COPY_OUT_INITRCD
include $(CLEAR_VARS)
LOCAL_MODULE := init.apmanager.rc
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/$(TARGET_COPY_OUT_INITRCD)
include $(BUILD_PREBUILT)
endif

# === unittest ===
include $(CLEAR_VARS)
LOCAL_MODULE := apmanager_unittest
LOCAL_SRC_FILES := \
    config_unittest.cc \
    device_info_unittest.cc \
    device_unittest.cc \
    dhcp_server_unittest.cc \
    hostapd_monitor_unittest.cc \
    manager_unittest.cc \
    mock_config.cc \
    mock_device.cc \
    mock_dhcp_server.cc \
    mock_dhcp_server_factory.cc \
    mock_event_dispatcher.cc \
    mock_file_writer.cc \
    mock_hostapd_monitor.cc \
    mock_manager.cc \
    mock_process_factory.cc \
    mock_service.cc \
    service_unittest.cc \
    run_all_tests.cc
LOCAL_STATIC_LIBRARIES := libapmanager libgmock
$(eval $(apmanager_common))
include $(BUILD_NATIVE_TEST)

endif # HOST_OS == linux