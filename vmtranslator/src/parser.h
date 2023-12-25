#ifndef NAND2TETRIS_VMTRANSLATOR_PARSER_H_
#define NAND2TETRIS_VMTRANSLATOR_PARSER_H_

#include <fstream>
#include <string_view>

#include "commands.h"

class VmFile {
 public:
  VmFile(std::string_view path);
  ~VmFile();

  void Advance();

  std::string path();
  std::string line();
  size_t line_number();
  Command *command();

 private:
  std::unique_ptr<Address> ParseAddress(std::string_view segment,
                                        std::string_view index_str);

  std::ifstream file_;
  std::string path_;
  std::string filename_;

  std::string line_;
  size_t line_number_ = 0;
  std::string function_;
  Command *command_ = nullptr;
};

#endif  // NAND2TETRIS_VMTRANSLATOR_PARSER_H_
