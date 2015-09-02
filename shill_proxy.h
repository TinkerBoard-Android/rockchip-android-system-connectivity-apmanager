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

#ifndef APMANAGER_SHILL_PROXY_H_
#define APMANAGER_SHILL_PROXY_H_

#include <set>
#include <string>

#include <base/macros.h>
#include <base/memory/scoped_ptr.h>

#include "shill/dbus-proxies.h"

// Proxy for shill "org.chromium.flimflam" DBus service.
namespace apmanager {

class ShillProxy {
 public:
  ShillProxy();
  virtual ~ShillProxy();

  void Init(const scoped_refptr<dbus::Bus>& bus);

  // Claim the given interface |interface_name| from shill.
  virtual void ClaimInterface(const std::string& interface_name);
  // Release the given interface |interface_name| to shill.
  virtual void ReleaseInterface(const std::string& interface_name);

 private:
  void OnServiceAvailable(bool service_available);
  void OnServiceNameChanged(const std::string& old_owner,
                            const std::string& new_owner);

  static const char kManagerPath[];

  // DBus proxy for shill manager.
  std::unique_ptr<org::chromium::flimflam::ManagerProxy> manager_proxy_;
  // List of interfaces apmanager have claimed.
  std::set<std::string> claimed_interfaces_;

  DISALLOW_COPY_AND_ASSIGN(ShillProxy);
};

}  // namespace apmanager

#endif  // APMANAGER_SHILL_PROXY_H_
