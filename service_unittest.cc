//
// Copyright (C) 2014 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "apmanager/service.h"

#include <string>

#include <base/strings/string_util.h>
#include <base/strings/stringprintf.h>
#include <chromeos/process_mock.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#if !defined(__ANDROID__)
#include <chromeos/dbus/service_constants.h>
#else
#include "dbus/apmanager/dbus-constants.h"
#endif  // __ANDROID__

#include "apmanager/mock_config.h"
#include "apmanager/mock_dhcp_server.h"
#include "apmanager/mock_dhcp_server_factory.h"
#include "apmanager/mock_file_writer.h"
#include "apmanager/mock_hostapd_monitor.h"
#include "apmanager/mock_manager.h"
#include "apmanager/mock_process_factory.h"

using chromeos::ProcessMock;
using ::testing::_;
using ::testing::Mock;
using ::testing::Return;
using ::testing::SetArgPointee;

namespace {
  const int kServiceIdentifier = 1;
  const char kHostapdConfig[] = "ssid=test\n";
  const char kBinSleep[] = "/bin/sleep";
  const char kHostapdConfigFilePath[] =
#if !defined(__ANDROID__)
      "/var/run/apmanager/hostapd/hostapd-1.conf";
#else
      "/data/misc/apmanager/hostapd/hostapd-1.conf";
#endif  // __ANDROID__
}  // namespace

namespace apmanager {

class ServiceTest : public testing::Test {
 public:
  ServiceTest()
      : dhcp_server_factory_(MockDHCPServerFactory::GetInstance()),
        file_writer_(MockFileWriter::GetInstance()),
        process_factory_(MockProcessFactory::GetInstance()),
        hostapd_monitor_(new MockHostapdMonitor()),
        service_(&manager_, kServiceIdentifier) {}

  virtual void SetUp() {
    service_.dhcp_server_factory_ = dhcp_server_factory_;
    service_.file_writer_ = file_writer_;
    service_.process_factory_ = process_factory_;
    service_.hostapd_monitor_.reset(hostapd_monitor_);
  }

  void StartDummyProcess() {
    service_.hostapd_process_.reset(new chromeos::ProcessImpl);
    service_.hostapd_process_->AddArg(kBinSleep);
    service_.hostapd_process_->AddArg("12345");
    CHECK(service_.hostapd_process_->Start());
    LOG(INFO) << "DummyProcess: " << service_.hostapd_process_->pid();
  }

  void SetConfig(Config* config) {
    service_.config_.reset(config);
  }

 protected:
  MockManager manager_;
  MockDHCPServerFactory* dhcp_server_factory_;
  MockFileWriter* file_writer_;
  MockProcessFactory* process_factory_;
  MockHostapdMonitor* hostapd_monitor_;
  Service service_;
};

MATCHER_P(IsServiceErrorStartingWith, message, "") {
  return arg != nullptr &&
         arg->GetDomain() == chromeos::errors::dbus::kDomain &&
         arg->GetCode() == kServiceError &&
         base::EndsWith(arg->GetMessage(), message, false);
}

TEST_F(ServiceTest, StartWhenServiceAlreadyRunning) {
  StartDummyProcess();

  chromeos::ErrorPtr error;
  EXPECT_FALSE(service_.Start(&error));
  EXPECT_THAT(error, IsServiceErrorStartingWith("Service already running"));
}

TEST_F(ServiceTest, StartWhenConfigFileFailed) {
  MockConfig* config = new MockConfig();
  SetConfig(config);

  chromeos::ErrorPtr error;
  EXPECT_CALL(*config, GenerateConfigFile(_, _)).WillOnce(Return(false));
  EXPECT_FALSE(service_.Start(&error));
  EXPECT_THAT(error, IsServiceErrorStartingWith(
      "Failed to generate config file"));
}

TEST_F(ServiceTest, StartSuccess) {
  MockConfig* config = new MockConfig();
  SetConfig(config);

  // Setup mock DHCP server.
  MockDHCPServer* dhcp_server = new MockDHCPServer();
  // Setup mock process.
  ProcessMock* process = new ProcessMock();

  std::string config_str(kHostapdConfig);
  chromeos::ErrorPtr error;
  EXPECT_CALL(*config, GenerateConfigFile(_, _)).WillOnce(
      DoAll(SetArgPointee<1>(config_str), Return(true)));
  EXPECT_CALL(*file_writer_, Write(kHostapdConfigFilePath, kHostapdConfig))
      .WillOnce(Return(true));
  EXPECT_CALL(*config, ClaimDevice()).WillOnce(Return(true));
  EXPECT_CALL(*process_factory_, CreateProcess()).WillOnce(Return(process));
  EXPECT_CALL(*process, Start()).WillOnce(Return(true));
  EXPECT_CALL(*dhcp_server_factory_, CreateDHCPServer(_, _))
      .WillOnce(Return(dhcp_server));
  EXPECT_CALL(*dhcp_server, Start()).WillOnce(Return(true));
  EXPECT_CALL(manager_, RequestDHCPPortAccess(_));
  EXPECT_CALL(*hostapd_monitor_, Start());
  EXPECT_TRUE(service_.Start(&error));
  EXPECT_EQ(nullptr, error);
}

TEST_F(ServiceTest, StopWhenServiceNotRunning) {
  chromeos::ErrorPtr error;
  EXPECT_FALSE(service_.Stop(&error));
  EXPECT_THAT(error, IsServiceErrorStartingWith(
      "Service is not currently running"));
}

TEST_F(ServiceTest, StopSuccess) {
  StartDummyProcess();

  MockConfig* config = new MockConfig();
  SetConfig(config);
  chromeos::ErrorPtr error;
  EXPECT_CALL(*config, ReleaseDevice()).Times(1);
  EXPECT_TRUE(service_.Stop(&error));
}

}  // namespace apmanager
