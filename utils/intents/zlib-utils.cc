/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utils/intents/zlib-utils.h"

#include <memory>

#include "utils/zlib/buffer_generated.h"
#include "utils/zlib/zlib.h"

namespace libtextclassifier3 {

bool CompressIntentModel(IntentFactoryModelT* intent_model) {
  std::unique_ptr<ZlibCompressor> intent_zlib_compressor =
      ZlibCompressor::Instance();
  for (auto& generator : intent_model->generator) {
    generator->compressed_lua_template_generator.reset(new CompressedBufferT);
    intent_zlib_compressor->Compress(
        std::string(reinterpret_cast<const char*>(
                        generator->lua_template_generator.data()),
                    generator->lua_template_generator.size()),
        generator->compressed_lua_template_generator.get());
    generator->lua_template_generator.clear();
  }
  return true;
}

}  // namespace libtextclassifier3
