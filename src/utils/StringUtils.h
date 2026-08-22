#pragma once
#include <Arduino.h>
namespace str_utils {
  inline bool isEmpty(const String& str) {
    return str.length() == 0;
  }
  inline bool isBlank(const String& str) {
