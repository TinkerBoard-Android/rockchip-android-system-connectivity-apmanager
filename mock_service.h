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

#ifndef APMANAGER_MOCK_SERVICE_H_
#define APMANAGER_MOCK_SERVICE_H_

#include <base/macros.h>
#include <gmock/gmock.h>

#include "apmanager/service.h"

namespace apmanager {

class MockService : public Service {
 public:
  MockService();
  ~MockService() override;

  MOCK_METHOD1(Start, bool(chromeos::ErrorPtr *error));
  MOCK_METHOD1(Stop, bool(chromeos::ErrorPtr *error));

 private:
  DISALLOW_COPY_AND_ASSIGN(MockService);
};

}  // namespace apmanager

#endif  // APMANAGER_MOCK_SERVICE_H_
