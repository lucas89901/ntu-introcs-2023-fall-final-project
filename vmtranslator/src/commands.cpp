#include "commands.h"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

#include "addressing.h"

std::ostream &operator<<(std::ostream &os, const Command &command) {
  return os << command.ToAssembly();
}

BinaryArithmeticCommand::BinaryArithmeticCommand(std::string_view write_command)
    : write_command_(write_command) {}

std::string BinaryArithmeticCommand::ToAssembly() const {
  return absl::StrFormat(
      "@SP\n"
      "AM=M-1\n"
      "D=M\n"
      "A=A-1\n"
      "%s\n",
      write_command_);
}

AddCommand::AddCommand() : BinaryArithmeticCommand("M=D+M") {}
SubCommand::SubCommand() : BinaryArithmeticCommand("M=M-D") {}
AndCommand::AndCommand() : BinaryArithmeticCommand("M=D&M") {}
OrCommand::OrCommand() : BinaryArithmeticCommand("M=D|M") {}

UnaryArithmeticCommand::UnaryArithmeticCommand(std::string_view write_command)
    : write_command_(write_command) {}

std::string UnaryArithmeticCommand::ToAssembly() const {
  return absl::StrFormat(
      "@SP\n"
      "A=M-1\n"
      "%s\n",
      write_command_);
}

NegCommand::NegCommand() : UnaryArithmeticCommand("M=-M") {}
NotCommand::NotCommand() : UnaryArithmeticCommand("M=!M") {}

std::string BinaryComparisonCommand::ToAssembly() const {
  return absl::StrFormat(
      "@SP\n"
      "AM=M-1\n"
      "D=M\n"
      "A=A-1\n"
      "D=M-D\n"
      "@%s\n"
      "D;%s\n"
      "@SP\n"
      "A=M-1\n"
      "M=-1\n"
      "@%s\n"
      "0;JMP\n"
      "(%s)\n"
      "@SP\n"
      "A=M-1\n"
      "M=0\n"
      "(%s)\n",
      else_label_, jump_condition_, end_label_, else_label_, end_label_);
}

BinaryComparisonCommand::BinaryComparisonCommand(
    std::string_view jump_condition, std::string_view else_label,
    std::string_view end_label)
    : jump_condition_(jump_condition),
      else_label_(else_label),
      end_label_(end_label) {}

EqCommand::EqCommand(std::string_view label)
    : BinaryComparisonCommand("JNE", absl::StrCat(label, "$eq_else"),
                              absl::StrCat(label, "$eq_end")) {}
GtCommand::GtCommand(std::string_view label)
    : BinaryComparisonCommand("JLE", absl::StrCat(label, "$gt_else"),
                              absl::StrCat(label, "$gt_end")) {}
LtCommand::LtCommand(std::string_view label)
    : BinaryComparisonCommand("JGE", absl::StrCat(label, "$lt_else"),
                              absl::StrCat(label, "$lt_end")) {}

PushCommand::PushCommand(std::unique_ptr<Address> address)
    : address_(std::move(address)) {}

std::string PushCommand::ToAssembly() const {
  return absl::StrFormat(
      "%s"
      "D=%c\n"
      "@SP\n"
      "A=M\n"
      "M=D\n"
      "@SP\n"
      "M=M+1\n",
      address_->AddressingAssembly(Destination::kA),
      address_->value_register());
}

PopCommand::PopCommand(std::unique_ptr<Address> address)
    : address_(std::move(address)) {}

std::string PopCommand::ToAssembly() const {
  if (dynamic_cast<PointerAddressedAddress *>(address_.get())) {
    return absl::StrFormat(
        "%s"
        "@R15\n"
        "M=D\n"
        "@SP\n"
        "AM=M-1\n"
        "D=M\n"
        "@R15\n"
        "A=M\n"
        "M=D\n",
        address_->AddressingAssembly(Destination::kD));
  }
  return absl::StrFormat(
      "@SP\n"
      "AM=M-1\n"
      "D=M\n"
      "%s"
      "M=D\n",
      address_->AddressingAssembly(Destination::kA));
}

LabelCommand::LabelCommand(std::string_view label) : label_(label) {}

std::string LabelCommand::ToAssembly() const {
  return absl::StrFormat("(%s)\n", label_);
}

GotoCommand::GotoCommand(std::string_view label) : label_(label) {}

std::string GotoCommand::ToAssembly() const {
  return absl::StrFormat(
      "@%s\n"
      "0;JMP\n",
      label_);
}

IfGotoCommand::IfGotoCommand(std::string_view label) : label_(label) {}

std::string IfGotoCommand::ToAssembly() const {
  return absl::StrFormat(
      "@SP\n"
      "AM=M-1\n"
      "D=M\n"
      "@%s\n"
      "D;JNE\n",
      label_);
}

CallCommand::CallCommand(std::string_view function, int argument_count,
                         std::string_view return_label)
    : function_(function),
      argument_count_(argument_count),
      return_label_(return_label) {}

std::string CallCommand::ToAssembly() const {
  std::string assembly = absl::StrFormat(
      "@%s\n"
      "D=A\n"
      "@SP\n"
      "A=M\n"
      "M=D\n",
      return_label_);
  for (std::string_view pointer : {"LCL", "ARG", "THIS", "THAT"}) {
    absl::StrAppend(&assembly, "@", pointer,
                    "\n"
                    "D=M\n"
                    "@SP\n"
                    "AM=M+1\n"
                    "M=D\n");
  }
  absl::StrAppend(&assembly,
                  "@SP\n"
                  "MD=M+1\n");

  absl::StrAppendFormat(&assembly,
                        // ARG = SP - 5 - argument_count
                        "@%d\n"
                        "D=D-A\n"
                        "@ARG\n"
                        "M=D\n"
                        // LCL = SP
                        "@SP\n"
                        "D=M\n"
                        "@LCL\n"
                        "M=D\n"
                        // goto function
                        "@%s\n"
                        "0;JMP\n"
                        "(%s)\n",
                        5 + argument_count_, function_, return_label_);
  return assembly;
}

FunctionCommand::FunctionCommand(std::string_view identifier,
                                 int variable_count)
    : identifier_(identifier), variable_count_(variable_count) {}

std::string FunctionCommand::ToAssembly() const {
  std::string assembly = absl::StrFormat("(%s)\n", identifier_);
  if (variable_count_ == 0) {
    return assembly;
  }

  absl::StrAppend(&assembly,
                  "@SP\n"
                  "A=M\n"
                  "M=0\n");
  for (int i = 0; i < variable_count_ - 1; ++i) {
    absl::StrAppend(&assembly,
                    "@SP\n"
                    "AM=M+1\n"
                    "M=0\n");
  }
  absl::StrAppend(&assembly,
                  "@SP\n"
                  "M=M+1\n");
  return assembly;
}

std::string ReturnCommand::ToAssembly() const {
  return "@5\n"
         "D=A\n"
         "@LCL\n"
         "A=M-D\n"
         "D=M\n"
         "@R15\n"
         "M=D\n"
         "@SP\n"
         "A=M-1\n"
         "D=M\n"
         "@ARG\n"
         "A=M\n"
         "M=D\n"
         "@ARG\n"
         "D=M+1\n"
         "@SP\n"
         "M=D\n"
         "@LCL\n"
         "A=M-1\n"
         "D=M\n"
         "@THAT\n"
         "M=D\n"
         "@2\n"
         "D=A\n"
         "@LCL\n"
         "A=M-D\n"
         "D=M\n"
         "@THIS\n"
         "M=D\n"
         "@3\n"
         "D=A\n"
         "@LCL\n"
         "A=M-D\n"
         "D=M\n"
         "@ARG\n"
         "M=D\n"
         "@4\n"
         "D=A\n"
         "@LCL\n"
         "A=M-D\n"
         "D=M\n"
         "@LCL\n"
         "M=D\n"
         "@R15\n"
         "A=M\n"
         "0;JMP\n";
}
