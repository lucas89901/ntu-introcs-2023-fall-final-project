#include "addressing.h"

#include <cstdint>
#include <string>
#include <string_view>

#include "absl/log/log.h"
#include "absl/strings/str_format.h"

std::string DestinationString(uint16_t destination) {
  std::string result;
  if (destination & Destination::kA) {
    result.push_back('A');
  }
  if (destination & Destination::kD) {
    result.push_back('D');
  }
  if (destination & Destination::kM) {
    result.push_back('M');
  }
  return result;
};

Address::Address(char value_register) : value_register_(value_register) {}

char Address::value_register() const { return value_register_; }

PointerAddressedAddress::PointerAddressedAddress(std::string_view pointer,
                                                 uint16_t index)
    : Address('M'), pointer_(pointer), index_(index) {}

std::string PointerAddressedAddress::AddressingAssembly(
    uint16_t destination) const {
  return absl::StrFormat(
      "@%u\n"
      "D=A\n"
      "@%s\n"
      "%s=D+M\n",
      index_, pointer_, DestinationString(destination));
}

ArgumentAddress::ArgumentAddress(uint16_t index)
    : PointerAddressedAddress("ARG", index) {}
LocalAddress::LocalAddress(uint16_t index)
    : PointerAddressedAddress("LCL", index) {}
ThisAddress::ThisAddress(uint16_t index)
    : PointerAddressedAddress("THIS", index) {}
ThatAddress::ThatAddress(uint16_t index)
    : PointerAddressedAddress("THAT", index) {}

StaticAddress::StaticAddress(std::string_view class_name, uint16_t index)
    : Address('M'), class_name_(class_name), index_(index) {}

std::string StaticAddress::AddressingAssembly(uint16_t destination) const {
  std::string result = absl::StrFormat("@%s.%u\n", class_name_, index_);
  destination = destination & ~Destination::kA;
  if (destination) {
    absl::StrAppendFormat(&result, "%s=A\n", DestinationString(destination));
  }
  return result;
}

DirectlyAddressedAddress::DirectlyAddressedAddress(uint16_t address,
                                                   char value_register)
    : Address(value_register), address_(address) {}

std::string DirectlyAddressedAddress::AddressingAssembly(
    uint16_t destination) const {
  std::string result = absl::StrFormat("@%u\n", address_);
  destination = destination & ~Destination::kA;
  if (destination) {
    absl::StrAppendFormat(&result, "%s=A\n", DestinationString(destination));
  }
  return result;
}

ConstantAddress::ConstantAddress(uint16_t index)
    : DirectlyAddressedAddress(index, 'A') {}

PointerAddress::PointerAddress(uint16_t index)
    : DirectlyAddressedAddress(3 + index, 'M') {
  if (index >= 2) {
    LOG(WARNING) << "Index out of range: " << index
                 << " (pointer segment is 2 words long)";
  }
}

TempAddress::TempAddress(uint16_t index)
    : DirectlyAddressedAddress(5 + index, 'M') {
  if (index >= 8) {
    LOG(WARNING) << "Index out of range: " << index
                 << " (temp segment is 8 words long)";
  }
}
