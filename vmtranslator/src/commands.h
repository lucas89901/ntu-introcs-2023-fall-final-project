#ifndef NAND2TETRIS_VMTRANSLATOR_COMMANDS_H_
#define NAND2TETRIS_VMTRANSLATOR_COMMANDS_H_

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "addressing.h"

class Command {
 public:
  virtual std::string ToAssembly() const = 0;
};
std::ostream &operator<<(std::ostream &os, const Command &command);

class BinaryArithmeticCommand : public Command {
 public:
  BinaryArithmeticCommand(std::string_view write_command);
  std::string ToAssembly() const override;

 private:
  std::string write_command_;
};

class AddCommand : public BinaryArithmeticCommand {
 public:
  AddCommand();
};

class SubCommand : public BinaryArithmeticCommand {
 public:
  SubCommand();
};

class AndCommand : public BinaryArithmeticCommand {
 public:
  AndCommand();
};

class OrCommand : public BinaryArithmeticCommand {
 public:
  OrCommand();
};

class UnaryArithmeticCommand : public Command {
 public:
  UnaryArithmeticCommand(std::string_view write_command);
  std::string ToAssembly() const;

 private:
  std::string write_command_;
};

class NegCommand : public UnaryArithmeticCommand {
 public:
  NegCommand();
};

class NotCommand : public UnaryArithmeticCommand {
 public:
  NotCommand();
};

class BinaryComparisonCommand : public Command {
 public:
  BinaryComparisonCommand(std::string_view jump_condition,
                          std::string_view else_label,
                          std::string_view end_label);
  std::string ToAssembly() const;

 private:
  std::string jump_condition_;
  std::string else_label_;
  std::string end_label_;
};

class EqCommand : public BinaryComparisonCommand {
 public:
  EqCommand(std::string_view label);
};

class GtCommand : public BinaryComparisonCommand {
 public:
  GtCommand(std::string_view label);
};

class LtCommand : public BinaryComparisonCommand {
 public:
  LtCommand(std::string_view label);
};

class PushCommand : public Command {
 public:
  PushCommand(std::unique_ptr<Address> address);
  std::string ToAssembly() const override;

 private:
  std::unique_ptr<Address> address_;
};

class PopCommand : public Command {
 public:
  PopCommand(std::unique_ptr<Address> address);
  std::string ToAssembly() const override;

 private:
  std::unique_ptr<Address> address_;
};

class LabelCommand : public Command {
 public:
  LabelCommand(std::string_view label);
  std::string ToAssembly() const override;

 private:
  std::string label_;
};

class GotoCommand : public Command {
 public:
  GotoCommand(std::string_view label);
  std::string ToAssembly() const override;

 private:
  std::string label_;
};

class IfGotoCommand : public Command {
 public:
  IfGotoCommand(std::string_view label);
  std::string ToAssembly() const override;

 private:
  std::string label_;
};

class CallCommand : public Command {
 public:
  CallCommand(std::string_view function, int argument_count,
              std::string_view return_label);
  std::string ToAssembly() const override;

 private:
  std::string function_;
  int argument_count_;
  std::string return_label_;
};

class FunctionCommand : public Command {
 public:
  FunctionCommand(std::string_view identifier, int variable_count);
  std::string ToAssembly() const override;

 private:
  std::string identifier_;
  int variable_count_;
};

class ReturnCommand : public Command {
 public:
  std::string ToAssembly() const override;
};

#endif  // NAND2TETRIS_VMTRANSLATOR_COMMANDS_H_
