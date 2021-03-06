//
// Created by mike on 29.11.18.
//

#ifndef DED_DIFFERENTIATOR_COMMON_CLASSES_H
#define DED_DIFFERENTIATOR_COMMON_CLASSES_H

#include <iostream>
#include <vector>
#include <string>


class SmartFile {
 private:
  FILE* file_{nullptr};

  void release() {
    if (file_ != nullptr) {
      fclose(file_);
    }
    file_ = nullptr;
  }

 public:
  SmartFile() {}

  SmartFile(const char* filename, const char* mode = "r") {
    file_ = fopen(filename, mode);
  }

  SmartFile(FILE* file) {
    file_ = file;
  }

  FILE* getFile() const {
    return file_;
  }

  void setFile(const char* filename, const char* mode = "r") {
    release();
    file_ = fopen(filename, mode);
  }

  ~SmartFile() {
    release();
  }
};

template<class T>
struct Command {
  size_t cmd_id;
  std::string cmd_name;
  size_t arg_cnt;
  std::vector<std::pair<T, int>> args;
};

bool isJump(const std::string& cmd_name) {
  return cmd_name == "jmp" || cmd_name == "call" || cmd_name == "je" || cmd_name == "jne" ||
    cmd_name == "jl" || cmd_name == "jle";
}

#endif //DED_DIFFERENTIATOR_COMMON_CLASSES_H
