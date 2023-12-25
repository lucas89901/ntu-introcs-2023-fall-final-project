#include <algorithm>
#include <bitset>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

class Instruction {
 public:
  virtual std::string ToMachine() const = 0;
  virtual std::string ToAssembly() const = 0;
};

class AInstruction : public Instruction {
 public:
  AInstruction(uint16_t value) : value_(value) {}

  std::string ToMachine() const override {
    return absl::StrFormat("0%s", value_.to_string());
  }

  std::string ToAssembly() const override {
    return absl::StrFormat("@%u", value_.to_ulong());
  }

 private:
  std::bitset<15> value_;
};

class CInstruction : public Instruction {
 public:
  std::string ToMachine() const override {
    return absl::StrFormat("111%s%s%s", computation_.to_string(),
                           destination_.to_string(), jump_.to_string());
  }

  std::string ToAssembly() const override {
    std::string assembly;
    if (destination_.any()) {
      if (destination_[2]) {
        assembly.push_back('A');
      }
      if (destination_[1]) {
        assembly.push_back('D');
      }
      if (destination_[0]) {
        assembly.push_back('M');
      }
      assembly.push_back('=');
    }

    for (const auto &pair : kComputationCodes) {
      if (pair.second == computation_) {
        absl::StrAppend(&assembly, pair.first);
        break;
      }
    }

    if (jump_.any()) {
      assembly.push_back(';');
      absl::StrAppend(&assembly, kJumpCodes[jump_.to_ulong()]);
    }
    return assembly;
  }

  bool SetComputation(std::string_view computation_str) {
    for (const auto &pair : kComputationCodes) {
      if (pair.first == computation_str) {
        computation_ = pair.second;
        return true;
      }
    }
    return false;
  }

  bool SetDestination(std::string_view destination_str) {
    for (char c : destination_str) {
      switch (c) {
        case 'A':
          destination_.set(2);
          break;
        case 'D':
          destination_.set(1);
          break;
        case 'M':
          destination_.set(0);
          break;
        default:
          LOG(ERROR) << "Unknown destination " << c;
          return false;
      }
    }
    return true;
  }

  bool SetJump(std::string_view jump_str) {
    size_t found = std::find(kJumpCodes, kJumpCodes + 8, jump_str) - kJumpCodes;
    if (found == 8) {
      return false;
    }
    jump_ = found;
    return true;
  }

 private:
  static constexpr std::pair<std::string_view, std::bitset<7>>
      kComputationCodes[] = {
          {"0", 0b0101010},   {"1", 0b0111111},   {"-1", 0b0111010},
          {"D", 0b0001100},   {"A", 0b0110000},   {"!D", 0b0001101},
          {"!A", 0b0110001},  {"-D", 0b0001111},  {"-A", 0b0110011},
          {"D+1", 0b0011111}, {"A+1", 0b0110111}, {"D-1", 0b0001110},
          {"A-1", 0b0110010}, {"D+A", 0b0000010}, {"D-A", 0b0010011},
          {"A-D", 0b0000111}, {"D&A", 0b0000000}, {"D|A", 0b0010101},
          {"M", 0b1110000},   {"!M", 0b1110001},  {"-M", 0b1110011},
          {"M+1", 0b1110111}, {"M-1", 0b1110010}, {"D+M", 0b1000010},
          {"D-M", 0b1010011}, {"M-D", 0b1000111}, {"D&M", 0b1000000},
          {"D|M", 0b1010101}};
  static constexpr std::string_view kJumpCodes[8] = {
      "", "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"};

  std::bitset<7> computation_;
  std::bitset<3> destination_;
  std::bitset<3> jump_;
};

bool IsNumber(std::string_view str) {
  for (char c : str) {
    if (!isdigit(c)) {
      return false;
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  CHECK_GE(argc, 2) << "No input file";
  std::ifstream asm_file(argv[1]);
  CHECK(asm_file.is_open()) << "Failed to open input file '" << argv[1] << "'";

  std::string buffer;
  std::vector<std::string> trimmed_lines;
  while (std::getline(asm_file, buffer)) {
    std::string line;
    for (size_t i = 0; i < buffer.size(); ++i) {
      if (buffer[i] == ' ' || buffer[i] == '\t') {
        continue;
      }
      if (buffer.substr(i, 2) == "//") {
        break;
      }
      line.push_back(buffer[i]);
    }
    if (!line.empty()) {
      trimmed_lines.push_back(std::move(line));
    }
  }
  asm_file.close();

  std::unordered_map<std::string, uint16_t> symbol_table = {
      {"R0", 0},   {"R1", 1},         {"R2", 2},      {"R3", 3},   {"R4", 4},
      {"R5", 5},   {"R6", 6},         {"R7", 7},      {"R8", 8},   {"R9", 9},
      {"R10", 10}, {"R11", 11},       {"R12", 12},    {"R13", 13}, {"R14", 14},
      {"R15", 15}, {"SCREEN", 16384}, {"KBD", 24576}, {"SP", 0},   {"LCL", 1},
      {"ARG", 2},  {"THIS", 3},       {"THAT", 4}};
  uint16_t instruction_counter = 0;
  for (const std::string &line : trimmed_lines) {
    if (line.front() == '(' && line.back() == ')') {  // Is label.
      symbol_table[line.substr(1, line.size() - 2)] = instruction_counter;
    } else {
      ++instruction_counter;
    }
  }

  std::string hack_filename = std::filesystem::path(argv[1]).stem().string();
  absl::StrAppend(&hack_filename, ".hack");
  std::ofstream hack_file(hack_filename);
  CHECK(hack_file.is_open())
      << "Failed to open output file '" << hack_filename << "'";

  uint16_t variable_address = 16;
  for (const std::string &line : trimmed_lines) {
    if (line.front() == '(' && line.back() == ')') {  // Label
      continue;
    }
    if (line.front() == '@') {  // A-instruction
      std::string value_str = line.substr(1);
      if (IsNumber(value_str)) {
        uint32_t value;
        if (absl::SimpleAtoi(value_str, &value) && value <= UINT16_MAX) {
          hack_file << AInstruction(static_cast<uint16_t>(value)).ToMachine()
                    << std::endl;
        } else {
          LOG(ERROR) << "Cannot convert " << value_str << " to uint16_t";
        }
      } else {
        if (!symbol_table.count(value_str)) {
          symbol_table[value_str] = variable_address++;
        }
        hack_file << AInstruction(symbol_table[value_str]).ToMachine()
                  << std::endl;
      }
    } else {  // C-instruction
      CInstruction instruction;
      std::string_view line_view(line);
      size_t found = line_view.find('=');
      if (found != std::string_view::npos) {
        instruction.SetDestination(line_view.substr(0, found));
        line_view.remove_prefix(found + 1);
      }

      found = line_view.find(';');
      instruction.SetComputation(line_view.substr(0, found));
      if (found != std::string_view::npos) {
        line_view.remove_prefix(found + 1);
        instruction.SetJump(line_view);
      }
      hack_file << instruction.ToMachine() << std::endl;
    }
  }
  hack_file.close();
  return 0;
}
