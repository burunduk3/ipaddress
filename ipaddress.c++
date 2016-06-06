#include "ipaddress.h"

IPAddress::IPAddress ( const IPAddress &address __attribute__((unused)) ) {
}

IPAddress::~IPAddress () {
}

IPAddress::IPAddress () {
}

IPAddress * IPAddress::parse ( const std::string &value ) {
  IPAddress *result = IPv4Address::parse (value);
  if (result)
    return result;
  result = IPv6Address::parse (value);
  if (result)
    return result;
  return 0;
}


IPv4Address::IPv4Address ( unsigned int address ) : IPAddress (), value (address) {
}

IPv4Address::IPv4Address () : IPAddress (), value (0) {
}

IPv4Address::IPv4Address ( const IPv4Address &address )
  : IPAddress (address), value (address.value) {
}

IPv4Address::~IPv4Address () {
}

IPv4Address * IPv4Address::clone () const {
  return new IPv4Address (*this);
}

IPv4Address * IPv4Address::parse ( const std::string &value ) {
  int octet[4], l;
  int r = sscanf (value.c_str(), "%d.%d.%d.%d%n", octet, octet + 1, octet + 2, octet + 3, &l);
  if (r != 4 || l != (int)value.length ()) {
    return 0;
  }
  unsigned int address = 0;
  for (int i = 0; i < 4; i++) {
    if (octet[i] < 0 || octet[i] >= 256) {
      return 0;
    }
    address = (address << 8) + octet[i];
  }
  return new IPv4Address (address);
}

int IPv4Address::length () const {
  return 32;
}

std::string IPv4Address::string () const {
  static char buffer[16];
  snprintf (buffer, 16, "%d.%d.%d.%d", octet[3], octet[2], octet[1], octet[0]);
  return std::string (buffer);
}

IPNetwork <> * IPv4Address::network ( int prefix_length ) const {
  return new IPNetwork <IPv4Address> (*this, prefix_length);
}

IPv4Address IPv4Address::mask ( int prefix_length ) const {
  unsigned int mask = 0;
  if (prefix_length) {
    mask = 0xffffffff << (32 - prefix_length);
  }
  return IPv4Address (value & mask);
}

IPv4Address IPv4Address::shift ( int prefix_length) const {
  return IPv4Address (value + (1 << (32 - prefix_length)));
}

bool IPv4Address::operator < ( const IPv4Address &b ) const {
  return value < b.value;
}

bool IPv4Address::operator <= ( const IPv4Address &b ) const {
  return value <= b.value;
}

IPv4Address IPv4Address::operator - ( int delta ) const {
  return IPv4Address (value - (unsigned int)delta);
}


IPv6Address::IPv6Address ( const IPv6Address &address )
  : IPAddress (address), value (address.value) {
}

IPv6Address::IPv6Address ( const unsigned short address[8] ) : IPAddress () {
  for (size_t i = 0; i < 8; i++) {
    block[7 - i] = address[i];
  }
}

IPv6Address::IPv6Address ( unsigned __int128 address ) : IPAddress (), value (address) {
}

IPv6Address::IPv6Address () : IPAddress (), value (0) {
}

IPv6Address::~IPv6Address () {
}

IPv6Address * IPv6Address::clone () const {
  return new IPv6Address (*this);
}

IPv6Address * IPv6Address::parse ( const std::string &value ) {
  unsigned short block[8];
  for (int i = 0; i < 8; i++) {
    block[i] = 0;
  }
  int k = 1;
  for (size_t i = 0; i < value.length (); i++) {
    k += value[i] == ':';
  }
  int data[k], current = 0, index = 0;
  bool digit = false;
  if (k > 8) {
    return 0;
  }
  for (size_t i = 0; i < value.length (); i++) {
    if (value[i] == ':') {
      data[index++] = digit ? current : -1;
      digit = false;
      current = 0;
      continue;
    }
    digit = true;
    if ('0' <= value[i] && value[i] <= '9') {
      current = current * 16 + value[i] - '0';
    } else if ('a' <= value[i] && value[i] <= 'f') {
      current = current * 16 + value[i] - 'a' + 10;
    } else if ('A' <= value[i] && value[i] <= 'F') {
      current = current * 16 + value[i] - 'A' + 10;
    } else {
      return 0;
    }
    if (current >= 0x10000) {
      return 0;
    }
  }
  data[index++] = digit ? current : -1;
  /*
  for (int i = 0; i < k; i++) {
    fprintf (stderr, "[debug] IPv6Address::parse, value = '%s', data[i=%d] = %04x\n", value.c_str (), i, data[i]);
  }
   //*/
  if (k > 1 && data[0] == -1 && data[1] != -1) {
    return 0;
  }
  if (k > 1 && data[k - 1] == -1 && data[k - 2] != -1) {
    return 0;
  }
  int start = data[0] == -1, finish = k - (data[k - 1] == -1);
  int skc = 0;
  for (int i = start; i < finish; i++) {
    if (data[i] == -1) {
      skc++;
      continue;
    }
    if (skc) {
      block[8 - (finish - i)] = data[i];
    } else {
      block[i] = data[i];
    }
  }
  if (skc != 1 && (skc != 0 || finish - start != 8 || k != 8)) {
    return 0;
  }
  return new IPv6Address (block);
}

int IPv6Address::length () const {
  return 128;
}

std::string IPv6Address::string () const {
  static char buffer[41];
  for (int i = 0; i < 8; i++)
    snprintf (buffer + i * 5, 6, "%04x:", block[7 - i]);
  buffer[39] = '\0';
  return std::string (buffer);
}

IPNetwork <> * IPv6Address::network ( int prefix_length ) const {
  return new IPNetwork <IPv6Address> (*this, prefix_length);
}

IPv6Address IPv6Address::mask ( int prefix_length ) const {
  unsigned short address[8];
  unsigned short mask[8];
  for (int i = 0; i < 8; i++) {
    mask[i] = 0;
    if (i * 16 < prefix_length) {
      mask[i] = 0xffff;
      mask[i] <<= 16 - (prefix_length - i * 16);
    }
    address[i] = block[7 - i] & mask[i];
  }
  return IPv6Address (address);
}

IPv6Address IPv6Address::shift ( int prefix_length ) const {
  unsigned short address[8];
  int add = 0;
  prefix_length = 128 - prefix_length;
  for (int i = 0; i < 8; i++) {
    add = add + block[i];
    if (prefix_length >= 0 && prefix_length < 16)
      add += (1 << prefix_length);
    address[7 - i] = add & 0xffff;
    add >>= 16;
    prefix_length -= 16;
  }
  return IPv6Address (address);
}

bool IPv6Address::operator < ( const IPv6Address &b ) const {
  for (int i = 7; i >= 0; i--) {
    if (block[i] != b.block[i]) {
      return block[i] < b.block[i];
    }
  }
  return false;
}

bool IPv6Address::operator <= ( const IPv6Address &b ) const {
  for (int i = 7; i >= 0; i--) {
    if (block[i] != b.block[i]) {
      return block[i] <= b.block[i];
    }
  }
  return true;
}

IPv6Address IPv6Address::operator - ( int delta ) const {
  return IPv6Address (value - (unsigned __int128)delta);
}


IPNetwork <IPAddress>::IPNetwork ( const IPNetwork <IPAddress> &net )
  : mPrefixLength (net.mPrefixLength) {
}

IPNetwork <IPAddress>::~IPNetwork () {
}

IPNetwork <IPAddress>::IPNetwork () : mPrefixLength (0) {
}

IPNetwork <IPAddress>::IPNetwork ( int prefix_length ) : mPrefixLength (prefix_length) {
}

IPNetwork <IPAddress> * IPNetwork <IPAddress>::parse ( const std::string &value ) {
  size_t slash = value.find ('/'), length = value.length ();
  int prefix_length = -1;
  if (slash != std::string::npos) {
    size_t pos;
    prefix_length = std::stoi (value.substr (slash + 1), &pos);
    // TODO: catch c++ exceptions?
    if (slash + 1 + pos != value.length ()) {
      return 0;
    }
    length = slash;
  }
  IPAddress *address = IPAddress::parse (value.substr (0, length));
  if (!address || prefix_length > address->length ()) {
    return 0;
  }
  if (prefix_length == -1) {
    prefix_length = address->length ();
  }
  IPNetwork <IPAddress> *result = address->network (prefix_length);
  delete address;
  return result;
}

int IPNetwork <IPAddress>::prefix_length() const {
  return this->mPrefixLength;
}


