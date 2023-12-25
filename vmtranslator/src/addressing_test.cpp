#include "addressing.h"

#include "absl/strings/str_format.h"
#include "gtest/gtest.h"

TEST(DestinationTest, DestinationString) {
  EXPECT_EQ(DestinationString(Destination::kA), "A");
  EXPECT_EQ(DestinationString(Destination::kD), "D");
  EXPECT_EQ(DestinationString(Destination::kM), "M");
  EXPECT_EQ(DestinationString(Destination::kA | Destination::kD), "AD");
  EXPECT_EQ(DestinationString(Destination::kA | Destination::kM), "AM");
  EXPECT_EQ(DestinationString(Destination::kD | Destination::kM), "DM");
  EXPECT_EQ(
      DestinationString(Destination::kA | Destination::kD | Destination::kM),
      "ADM");
}

TEST(AddressingTest, ArgumentAddress) {
  ArgumentAddress address(2);
  EXPECT_EQ(address.AddressingAssembly(Destination::kA),
            "@2\n"
            "D=A\n"
            "@ARG\n"
            "A=D+M\n");
  EXPECT_EQ(address.value_register(), 'M');
}

TEST(AddressingTest, LocalAddress) {
  LocalAddress address(2);
  EXPECT_EQ(address.AddressingAssembly(Destination::kA),
            "@2\n"
            "D=A\n"
            "@LCL\n"
            "A=D+M\n");
  EXPECT_EQ(address.value_register(), 'M');
}

TEST(AddressingTest, ThisAddress) {
  ThisAddress address(2);
  EXPECT_EQ(address.AddressingAssembly(Destination::kA),
            "@2\n"
            "D=A\n"
            "@THIS\n"
            "A=D+M\n");
  EXPECT_EQ(address.value_register(), 'M');
}

TEST(AddressingTest, ThatAddress) {
  ThatAddress address(2);
  EXPECT_EQ(address.AddressingAssembly(Destination::kA),
            "@2\n"
            "D=A\n"
            "@THAT\n"
            "A=D+M\n");
  EXPECT_EQ(address.value_register(), 'M');
}

TEST(AddressingTest, StaticAddress) {
  StaticAddress address("Foo", 2);
  EXPECT_EQ(address.AddressingAssembly(Destination::kA), "@Foo.2\n");
  EXPECT_EQ(address.AddressingAssembly(Destination::kD),
            "@Foo.2\n"
            "D=A\n");
  EXPECT_EQ(address.value_register(), 'M');
}

TEST(AddressingTest, ConstantAddress) {
  ConstantAddress address(2);
  EXPECT_EQ(address.AddressingAssembly(Destination::kA), "@2\n");
  EXPECT_EQ(address.AddressingAssembly(Destination::kD),
            "@2\n"
            "D=A\n");
  EXPECT_EQ(address.value_register(), 'A');
}

TEST(AddressingTest, PointerAddress) {
  for (int i = 0; i < 2; ++i) {
    PointerAddress address(i);
    EXPECT_EQ(address.AddressingAssembly(Destination::kA),
              absl::StrFormat("@%d\n", 3 + i));
    EXPECT_EQ(address.AddressingAssembly(Destination::kD),
              absl::StrFormat("@%d\n"
                              "D=A\n",
                              3 + i));
    EXPECT_EQ(address.value_register(), 'M');
  }
}

TEST(AddressingTest, TempAddress) {
  for (int i = 0; i < 8; ++i) {
    TempAddress address(i);
    EXPECT_EQ(address.AddressingAssembly(Destination::kA),
              absl::StrFormat("@%d\n", 5 + i));
    EXPECT_EQ(address.AddressingAssembly(Destination::kD),
              absl::StrFormat("@%d\n"
                              "D=A\n",
                              5 + i));
    EXPECT_EQ(address.value_register(), 'M');
  }
}
