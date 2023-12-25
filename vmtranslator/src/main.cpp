#include <filesystem>
#include <iostream>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/strip.h"

#include "commands.h"
#include "parser.h"

ABSL_FLAG(bool, v, false, "verbose output, print assembly output to console");
ABSL_FLAG(bool, d, false,
          "debug mode, write VM source lines as comments in assembly output");

class AssemblyFile {
 public:
  AssemblyFile(std::string_view path, bool source_is_multi_file)
      : file_(path.data()), source_is_multi_file_(source_is_multi_file) {
    QCHECK(file_.is_open()) << "Could not open output file: " << path;
    LOG(INFO) << "Output: " << path;
    LOG(INFO) << "Source is multi-file: " << source_is_multi_file;

    // Bootstrap code.
    file_ << "@256\n"
          << "D=A\n"
          << "@SP\n"
          << "M=D\n";

    if (source_is_multi_file_) {
      file_ << CallCommand("Sys.init", 0, "END").ToAssembly();
      // Although `Sys.init` is expected to enter an infinite loop, we still add
      // an infinite loop in case `Sys.init` returns.
      file_ << "@END\n"
            << "(END)\n"
            << "0;JMP\n";
    }
  }

  ~AssemblyFile() {
    if (!source_is_multi_file_) {
      file_ << "@END\n"
            << "(END)\n"
            << "0;JMP\n";
    }
    file_.close();
  }

  template <class T>
  AssemblyFile &operator<<(const T &value) {
    file_ << value;
    return *this;
  }

 private:
  std::ofstream file_;
  bool source_is_multi_file_ = false;
};

void Translate(VmFile &vm_file, AssemblyFile &asm_file) {
  while (vm_file.command()) {
    if (absl::GetFlag(FLAGS_v)) {
      LOG(INFO) << vm_file.line() << " ->\n" << vm_file.command()->ToAssembly();
    }
    if (absl::GetFlag(FLAGS_d)) {
      asm_file << "// " << vm_file.path() << ':' << vm_file.line_number()
               << ": " << vm_file.line() << '\n';
    }
    asm_file << vm_file.command()->ToAssembly();
    vm_file.Advance();
  }
}

int main(int argc, char *argv[]) {
  absl::SetProgramUsageMessage(
      absl::StrFormat("Usage: %s [-d] [-v] SOURCE", argv[0]));
  std::vector<char *> positional_args = absl::ParseCommandLine(argc, argv);
  QCHECK_EQ(positional_args.size(), 2) << absl::ProgramUsageMessage();

  std::filesystem::directory_entry source(positional_args[1]);
  QCHECK(source.exists()) << "File or directory does not exist: "
                          << positional_args[1];
  LOG(INFO) << "Source: " << source.path().string();

  std::string program_name;
  std::filesystem::path asm_path;
  if (source.is_directory()) {
    if (source.path().has_filename()) {
      program_name = source.path().filename().string();
    } else {
      program_name = source.path().parent_path().filename().string();
    }
    asm_path = source.path() / absl::StrCat(program_name, ".asm");
  } else {
    program_name = source.path().stem().string();
    asm_path = source.path().parent_path() / absl::StrCat(program_name, ".asm");
  }
  CHECK(!program_name.empty());
  LOG(INFO) << "Program: " << program_name;

  AssemblyFile asm_file(asm_path.string(), source.is_directory());
  if (source.is_directory()) {
    for (const std::filesystem::directory_entry &entry :
         std::filesystem::directory_iterator(source)) {
      if (entry.path().extension() == ".vm") {
        VmFile vm_file(entry.path().string());
        Translate(vm_file, asm_file);
      }
    }
  } else {
    VmFile vm_file(positional_args[1]);
    Translate(vm_file, asm_file);
  }
  return 0;
}
