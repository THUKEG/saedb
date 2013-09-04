// Copyright (c) 2013 The SAE Authors. All rights reserved.
// Derived from LevelDB's testharness code.
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LEVELDB_LICENSE file. See the LEVELDB_AUTHORS file for names of contributors.

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "testharness.hpp"

namespace saedb {
namespace test {

namespace {
  struct Test {
    const char* base;
    const char* name;
    void (*func)();
  };
  std::vector<Test>* tests;
}

bool RegisterTest(const char* base, const char* name, void (*func)()) {
  if (tests == NULL) {
    tests = new std::vector<Test>;
  }
  Test t;
  t.base = base;
  t.name = name;
  t.func = func;
  tests->push_back(t);
  return true;
}

int RunAllTests() {
  const char* matcher = std::getenv("SAE_TESTS");

  int num = 0;
  if (tests != NULL) {
    for (int i = 0; i < tests->size(); i++) {
      const Test& t = (*tests)[i];
      if (matcher != NULL) {
        std::string name = t.base;
        name.push_back('.');
        name.append(t.name);
        if (name.find(matcher) == std::string::npos) {
          continue;
        }
      }
      std::cerr << "==== Test " << t.base << "."  << t.name << std::endl;
      (*t.func)();
      ++num;
    }
  }
  std::cerr << "==== PASSED " << num << " tests" << std::endl;
  return 0;
}

std::string TempDir() {
  const char * dir;

  dir = std::getenv("SAE_TMP_DIR");
  if (dir) return dir;

  dir = std::getenv("TMP");
  if (dir) return dir;

  dir = std::getenv("TEMP");
  if (dir) return dir;

  // default to current directory
  // TODO use "/tmp" for posix
  return ".";
}

std::string TempFileName() {
  char tmp[L_tmpnam];
  tmpnam(tmp);
  // Windows quirk: it returns a '\\' prefixed name in current dir
  if (tmp[0] == '\\') return tmp + 1;
  else return tmp;
}

int RandomSeed() {
  const char* env = getenv("TEST_RANDOM_SEED");
  int result = (env != NULL ? atoi(env) : 301);
  if (result <= 0) {
    result = 301;
  }
  return result;
}

}  // namespace test
}  // namespace saedb
