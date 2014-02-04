#!/usr/bin/env python

import os.path
import re
import subprocess
import sys

from in_file import InFile
import in_generator
import license


HEADER_TEMPLATE = """
%(license)s

#ifndef %(class_name)s_h
#define %(class_name)s_h

#include "core/css/CSSParserMode.h"
#include <string.h>

namespace WebCore {

enum CSSValueID {
%(value_keyword_enums)s
};

const int numCSSValueKeywords = %(value_keywords_count)d;
const size_t maxCSSValueKeywordLength = %(max_value_keyword_length)d;

const char* getValueName(unsigned short id);
bool isValueAllowedInMode(unsigned short id, CSSParserMode mode);

} // namespace WebCore

#endif // %(class_name)s_h
"""

GPERF_TEMPLATE = """
%%{
%(license)s

#include "config.h"
#include "%(class_name)s.h"
#include "core/platform/HashTools.h"
#include <string.h>

namespace WebCore {
const char* const valueList[] = {
"",
%(value_keyword_strings)s
0
};

%%}
%%struct-type
struct Value;
%%omit-struct-type
%%language=C++
%%readonly-tables
%%compare-strncmp
%%define class-name %(class_name)sHash
%%define lookup-function-name findValueImpl
%%define hash-function-name value_hash_function
%%define word-array-name value_word_list
%%enum
%%%%
%(value_keyword_to_enum_map)s
%%%%
const Value* findValue(register const char* str, register unsigned int len)
{
    return CSSValueKeywordsHash::findValueImpl(str, len);
}

const char* getValueName(unsigned short id)
{
    if (id >= numCSSValueKeywords || id <= 0)
        return 0;
    return valueList[id];
}

bool isValueAllowedInMode(unsigned short id, CSSParserMode mode)
{
    switch (id) {
        %(ua_sheet_mode_values_keywords)s
            return mode == UASheetMode;
        %(quirks_mode_values_keywords)s
            return mode == CSSQuirksMode;
        %(quirks_mode_or_ua_sheet_mode_values_keywords)s
            return mode == UASheetMode || mode == CSSQuirksMode;
        default:
            return true;
    }
}

} // namespace WebCore
"""


class CSSValueKeywordsWriter(in_generator.Writer):
    class_name = "CSSValueKeywords"
    defaults = {
        'condition': None,
        'mode': None,
    }

    def __init__(self, file_paths, enabled_conditions):
        in_generator.Writer.__init__(self, file_paths, enabled_conditions)
        self._outputs = {(self.class_name + ".h"): self.generate_header,
                         (self.class_name + ".cpp"): self.generate_implementation,
                        }

        all_properties = self.in_file.name_dictionaries
        self._value_keywords = filter(lambda property: not property['condition'] or property['condition'] in self._enabled_conditions, all_properties)
        first_property_id = 1
        for offset, property in enumerate(self._value_keywords):
            property['name'] = property['name'].lower()
            property['enum_name'] = self._enum_name_from_value_keyword(property['name'])
            property['enum_value'] = first_property_id + offset
            if property['name'].startswith('-internal-'):
                assert property['mode'] is None, 'Can\'t specify mode for value keywords with the prefix "-internal-".'
                property['mode'] = 'UASheet'
            else:
                assert property['mode'] != 'UASheet', 'UASheet mode only value keywords should have the prefix "-internal-".'

    def _enum_name_from_value_keyword(self, value_keyword):
        return "CSSValue" + "".join(w.capitalize() for w in value_keyword.split("-"))

    def _enum_declaration(self, property):
        return "    %(enum_name)s = %(enum_value)s," % property

    def _case_value_keyword(self, property):
        return "case %(enum_name)s:" % property

    def generate_header(self):
        enum_enties = map(self._enum_declaration, [{'enum_name': 'CSSValueInvalid', 'enum_value': 0}] + self._value_keywords)
        return HEADER_TEMPLATE % {
            'license': license.license_for_generated_cpp(),
            'class_name': self.class_name,
            'value_keyword_enums': "\n".join(enum_enties),
            'value_keywords_count': len(enum_enties),
            'max_value_keyword_length': reduce(max, map(len, map(lambda property: property['name'], self._value_keywords))),
        }

    def _value_keywords_with_mode(self, mode):
        return filter(lambda property: property['mode'] == mode, self._value_keywords)

    def generate_implementation(self):
        gperf_input = GPERF_TEMPLATE % {
            'license': license.license_for_generated_cpp(),
            'class_name': self.class_name,
            'value_keyword_strings': '\n'.join(map(lambda property: '    "%(name)s",' % property, self._value_keywords)),
            'value_keyword_to_enum_map': '\n'.join(map(lambda property: '%(name)s, %(enum_name)s' % property, self._value_keywords)),
            'ua_sheet_mode_values_keywords': '\n'.join(map(self._case_value_keyword, self._value_keywords_with_mode('UASheet'))),
            'quirks_mode_values_keywords': '\n'.join(map(self._case_value_keyword, self._value_keywords_with_mode('Quirks'))),
            'quirks_mode_or_ua_sheet_mode_values_keywords': '\n'.join(map(self._case_value_keyword, self._value_keywords_with_mode('QuirksOrUASheet'))),
        }
        # FIXME: If we could depend on Python 2.7, we would use subprocess.check_output
        gperf_args = ['gperf', '--key-positions=*', '-D', '-n', '-s', '2']
        gperf = subprocess.Popen(gperf_args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        return gperf.communicate(gperf_input)[0]


if __name__ == "__main__":
    in_generator.Maker(CSSValueKeywordsWriter).main(sys.argv)