#pragma once
#include <Arduino.h>
namespace str_utils {
  inline bool isEmpty(const String& str) {
    return str.length() == 0;
  }
  inline bool isBlank(const String& str) {
    for (unsigned int i = 0; i < str.length(); i++) {
      if (!isSpace(str[i])) return false;
    }
    return true;
  }
  inline void toUpperCase(String& str) {
    str.toUpperCase();
  }
