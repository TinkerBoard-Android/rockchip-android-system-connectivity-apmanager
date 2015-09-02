//
// Copyright (C) 2015 The Android Open Source Project
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

#ifndef APMANAGER_MOCK_FILE_WRITER_H_
#define APMANAGER_MOCK_FILE_WRITER_H_

#include <string>

#include <base/lazy_instance.h>
#include <gmock/gmock.h>

#include "apmanager/file_writer.h"

namespace apmanager {

class MockFileWriter : public FileWriter {
 public:
  ~MockFileWriter() override;

  // This is a singleton. Use MockFileWriter::GetInstance()->Foo().
  static MockFileWriter* GetInstance();

  MOCK_METHOD2(Write, bool(const std::string& file_name,
                           const std::string& content));

 protected:
  MockFileWriter();

 private:
  friend struct base::DefaultLazyInstanceTraits<MockFileWriter>;

  DISALLOW_COPY_AND_ASSIGN(MockFileWriter);
};

}  // namespace apmanager

#endif  // APMANAGER_MOCK_FILE_WRITER_H_
