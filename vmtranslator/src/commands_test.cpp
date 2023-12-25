#include "commands.h"

#include <memory>

#include "gtest/gtest.h"

#include "addressing.h"

TEST(ArithmeticsCommandTest, AddCommand) {
  EXPECT_EQ(AddCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D+M\n");
}

TEST(ArithmeticsCommandTest, SubCommand) {
  EXPECT_EQ(SubCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=M-D\n");
}

TEST(ArithmeticsCommandTest, AndCommand) {
  EXPECT_EQ(AndCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D&M\n");
}

TEST(ArithmeticsCommandTest, OrCommand) {
  EXPECT_EQ(OrCommand().ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D|M\n");
}

TEST(ArithmeticsCommandTest, NegCommand) {
  EXPECT_EQ(NegCommand().ToAssembly(),
            "@SP\n"
            "A=M-1\n"
            "M=-M\n");
}

TEST(ArithmeticsCommandTest, NotCommand) {
  EXPECT_EQ(NotCommand().ToAssembly(),
            "@SP\n"
            "A=M-1\n"
            "M=!M\n");
}

TEST(LogicalCommandTest, EqCommand) {
  EXPECT_EQ(EqCommand("Foo_123").ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@Foo_123$eq_else\n"
            "D;JNE\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "@Foo_123$eq_end\n"
            "0;JMP\n"
            "(Foo_123$eq_else)\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "(Foo_123$eq_end)\n");
}

TEST(LogicalCommandTest, GtCommand) {
  EXPECT_EQ(GtCommand("Foo_123").ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@Foo_123$gt_else\n"
            "D;JLE\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "@Foo_123$gt_end\n"
            "0;JMP\n"
            "(Foo_123$gt_else)\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "(Foo_123$gt_end)\n");
}

TEST(LogicalCommandTest, LtCommand) {
  EXPECT_EQ(LtCommand("Foo_123").ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@Foo_123$lt_else\n"
            "D;JGE\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "@Foo_123$lt_end\n"
            "0;JMP\n"
            "(Foo_123$lt_else)\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "(Foo_123$lt_end)\n");
}

TEST(PushCommandTest, ArgumentAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<ArgumentAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@ARG\n"
            "A=D+M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, LocalAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<LocalAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@LCL\n"
            "A=D+M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, ThisAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<ThisAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@THIS\n"
            "A=D+M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, ThatAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<ThatAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@THAT\n"
            "A=D+M\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, StaticAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<StaticAddress>("Foo", 5)).ToAssembly(),
            "@Foo.5\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, ConstantAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<ConstantAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, PointerAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<PointerAddress>(0)).ToAssembly(),
            "@3\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PushCommandTest, TempAddress) {
  EXPECT_EQ(PushCommand(std::make_unique<TempAddress>(5)).ToAssembly(),
            "@10\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(PopCommandTest, ArgumentAddress) {
  EXPECT_EQ(PopCommand(std::make_unique<ArgumentAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@ARG\n"
            "D=D+M\n"
            "@R15\n"
            "M=D\n"
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@R15\n"
            "A=M\n"
            "M=D\n");
}

TEST(PopCommandTest, LocalAddress) {
  EXPECT_EQ(PopCommand(std::make_unique<LocalAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@LCL\n"
            "D=D+M\n"
            "@R15\n"
            "M=D\n"
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@R15\n"
            "A=M\n"
            "M=D\n");
}

TEST(PopCommandTest, ThisAddress) {
  EXPECT_EQ(PopCommand(std::make_unique<ThisAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@THIS\n"
            "D=D+M\n"
            "@R15\n"
            "M=D\n"
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@R15\n"
            "A=M\n"
            "M=D\n");
}

TEST(PopCommandTest, ThatAddress) {
  EXPECT_EQ(PopCommand(std::make_unique<ThatAddress>(5)).ToAssembly(),
            "@5\n"
            "D=A\n"
            "@THAT\n"
            "D=D+M\n"
            "@R15\n"
            "M=D\n"
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@R15\n"
            "A=M\n"
            "M=D\n");
}

TEST(PopCommandTest, StaticAddress) {
  EXPECT_EQ(PopCommand(std::make_unique<StaticAddress>("Foo", 5)).ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@Foo.5\n"
            "M=D\n");
}

TEST(PopCommandTest, ConstantAddress) {
  EXPECT_EQ(PopCommand(std::make_unique<ConstantAddress>(5)).ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@5\n"
            "M=D\n");
}

TEST(PopCommandTest, PointerAddress) {
  EXPECT_EQ(PopCommand(std::make_unique<PointerAddress>(0)).ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@3\n"
            "M=D\n");
}

TEST(PopCommandTest, TempAddress) {
  EXPECT_EQ(PopCommand(std::make_unique<TempAddress>(5)).ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@10\n"
            "M=D\n");
}

TEST(LabelCommandTest, LabelCommand) {
  EXPECT_EQ(LabelCommand("Foo.f$L1").ToAssembly(), "(Foo.f$L1)\n");
}

TEST(GotoCommandTest, GotoCommand) {
  EXPECT_EQ(GotoCommand("Foo.f$L1").ToAssembly(),
            "@Foo.f$L1\n"
            "0;JMP\n");
}

TEST(IfGotoCommandTest, IfGotoCommand) {
  EXPECT_EQ(IfGotoCommand("Foo.f$L1").ToAssembly(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@Foo.f$L1\n"
            "D;JNE\n");
}

TEST(CallCommandTest, CallCommand) {
  EXPECT_EQ(CallCommand("Callee.callee", 0, "Caller_123$ret").ToAssembly(),
            "@Caller_123$ret\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@LCL\n"
            "D=M\n"
            "@SP\n"
            "AM=M+1\n"
            "M=D\n"
            "@ARG\n"
            "D=M\n"
            "@SP\n"
            "AM=M+1\n"
            "M=D\n"
            "@THIS\n"
            "D=M\n"
            "@SP\n"
            "AM=M+1\n"
            "M=D\n"
            "@THAT\n"
            "D=M\n"
            "@SP\n"
            "AM=M+1\n"
            "M=D\n"
            "@SP\n"
            "MD=M+1\n"
            "@5\n"
            "D=D-A\n"
            "@ARG\n"
            "M=D\n"
            "@SP\n"
            "D=M\n"
            "@LCL\n"
            "M=D\n"
            "@Callee.callee\n"
            "0;JMP\n"
            "(Caller_123$ret)\n");
  EXPECT_EQ(CallCommand("Callee.callee", 2, "Caller_123$ret").ToAssembly(),
            "@Caller_123$ret\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@LCL\n"
            "D=M\n"
            "@SP\n"
            "AM=M+1\n"
            "M=D\n"
            "@ARG\n"
            "D=M\n"
            "@SP\n"
            "AM=M+1\n"
            "M=D\n"
            "@THIS\n"
            "D=M\n"
            "@SP\n"
            "AM=M+1\n"
            "M=D\n"
            "@THAT\n"
            "D=M\n"
            "@SP\n"
            "AM=M+1\n"
            "M=D\n"
            "@SP\n"
            "MD=M+1\n"
            "@7\n"
            "D=D-A\n"
            "@ARG\n"
            "M=D\n"
            "@SP\n"
            "D=M\n"
            "@LCL\n"
            "M=D\n"
            "@Callee.callee\n"
            "0;JMP\n"
            "(Caller_123$ret)\n");
}

TEST(FunctionCommandTest, FunctionCommand) {
  EXPECT_EQ(FunctionCommand("Foo.f", 0).ToAssembly(), "(Foo.f)\n");
  EXPECT_EQ(FunctionCommand("Foo.f", 1).ToAssembly(),
            "(Foo.f)\n"
            "@SP\n"
            "A=M\n"
            "M=0\n"
            "@SP\n"
            "M=M+1\n");
  EXPECT_EQ(FunctionCommand("Foo.f", 2).ToAssembly(),
            "(Foo.f)\n"
            "@SP\n"
            "A=M\n"
            "M=0\n"
            "@SP\n"
            "AM=M+1\n"
            "M=0\n"
            "@SP\n"
            "M=M+1\n");
  EXPECT_EQ(FunctionCommand("Foo.f", 3).ToAssembly(),
            "(Foo.f)\n"
            "@SP\n"
            "A=M\n"
            "M=0\n"
            "@SP\n"
            "AM=M+1\n"
            "M=0\n"
            "@SP\n"
            "AM=M+1\n"
            "M=0\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(ReturnCommandTest, ReturnCommand) {
  EXPECT_EQ(ReturnCommand().ToAssembly(),
            "@5\n"
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
            "0;JMP\n");
}
