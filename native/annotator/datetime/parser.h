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

#ifndef LIBTEXTCLASSIFIER_ANNOTATOR_DATETIME_PARSER_H_
#define LIBTEXTCLASSIFIER_ANNOTATOR_DATETIME_PARSER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "annotator/datetime/extractor.h"
#include "annotator/model_generated.h"
#include "annotator/types.h"
#include "utils/base/integral_types.h"
#include "utils/calendar/calendar.h"
#include "utils/utf8/unicodetext.h"
#include "utils/utf8/unilib.h"
#include "utils/zlib/zlib.h"

namespace libtextclassifier3 {

// Parses datetime expressions in the input and resolves them to actual absolute
// time.
class DatetimeParser {
 public:
  static std::unique_ptr<DatetimeParser> Instance(
      const DatetimeModel* model, const UniLib* unilib,
      const CalendarLib* calendarlib, ZlibDecompressor* decompressor);

  // Parses the dates in 'input' and fills result. Makes sure that the results
  // do not overlap.
  // If 'anchor_start_end' is true the extracted results need to start at the
  // beginning of 'input' and end at the end of it.
  bool Parse(const std::string& input, int64 reference_time_ms_utc,
             const std::string& reference_timezone, const std::string& locales,
             ModeFlag mode, AnnotationUsecase annotation_usecase,
             bool anchor_start_end,
             std::vector<DatetimeParseResultSpan>* results) const;

  // Same as above but takes UnicodeText.
  bool Parse(const UnicodeText& input, int64 reference_time_ms_utc,
             const std::string& reference_timezone, const std::string& locales,
             ModeFlag mode, AnnotationUsecase annotation_usecase,
             bool anchor_start_end,
             std::vector<DatetimeParseResultSpan>* results) const;

 protected:
  explicit DatetimeParser(const DatetimeModel* model, const UniLib* unilib,
                          const CalendarLib* calendarlib,
                          ZlibDecompressor* decompressor);

  // Returns a list of locale ids for given locale spec string (comma-separated
  // locale names). Assigns the first parsed locale to reference_locale.
  std::vector<int> ParseAndExpandLocales(const std::string& locales,
                                         std::string* reference_locale) const;

  // Helper function that finds datetime spans, only using the rules associated
  // with the given locales.
  bool FindSpansUsingLocales(
      const std::vector<int>& locale_ids, const UnicodeText& input,
      const int64 reference_time_ms_utc, const std::string& reference_timezone,
      ModeFlag mode, AnnotationUsecase annotation_usecase,
      bool anchor_start_end, const std::string& reference_locale,
      std::unordered_set<int>* executed_rules,
      std::vector<DatetimeParseResultSpan>* found_spans) const;

  bool ParseWithRule(const CompiledRule& rule, const UnicodeText& input,
                     int64 reference_time_ms_utc,
                     const std::string& reference_timezone,
                     const std::string& reference_locale, const int locale_id,
                     bool anchor_start_end,
                     std::vector<DatetimeParseResultSpan>* result) const;

  // Converts the current match in 'matcher' into DatetimeParseResult.
  bool ExtractDatetime(const CompiledRule& rule,
                       const UniLib::RegexMatcher& matcher,
                       int64 reference_time_ms_utc,
                       const std::string& reference_timezone,
                       const std::string& reference_locale, int locale_id,
                       std::vector<DatetimeParseResult>* results,
                       CodepointSpan* result_span) const;

  // Parse and extract information from current match in 'matcher'.
  bool HandleParseMatch(const CompiledRule& rule,
                        const UniLib::RegexMatcher& matcher,
                        int64 reference_time_ms_utc,
                        const std::string& reference_timezone,
                        const std::string& reference_locale, int locale_id,
                        std::vector<DatetimeParseResultSpan>* result) const;

 private:
  bool initialized_;
  const UniLib& unilib_;
  const CalendarLib& calendarlib_;
  std::vector<CompiledRule> rules_;
  std::unordered_map<int, std::vector<int>> locale_to_rules_;
  std::vector<std::unique_ptr<const UniLib::RegexPattern>> extractor_rules_;
  std::unordered_map<DatetimeExtractorType, std::unordered_map<int, int>>
      type_and_locale_to_extractor_rule_;
  std::unordered_map<std::string, int> locale_string_to_id_;
  std::vector<int> default_locale_ids_;
  bool use_extractors_for_locating_;
  bool generate_alternative_interpretations_when_ambiguous_;
  bool prefer_future_for_unspecified_date_;
};

}  // namespace libtextclassifier3

#endif  // LIBTEXTCLASSIFIER_ANNOTATOR_DATETIME_PARSER_H_
