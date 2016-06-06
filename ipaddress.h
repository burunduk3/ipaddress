#pragma once

#include <string>
#include <type_traits>

class IPAddress;
class IPv4Address;
class IPv6Address;
template <class Address=IPAddress>
  requires std::is_base_of<IPAddress, Address>::value
  class IPNetwork;

class IPAddress {
  protected:
    IPAddress ( const IPAddress & );
    IPAddress& operator = ( const IPAddress & );
    IPAddress ();
  public:
    virtual ~IPAddress () = 0;
    virtual IPAddress * clone () const = 0;
    static IPAddress * parse ( const std::string & );
    virtual int length () const = 0;
    virtual std::string string () const = 0;
    virtual IPNetwork <> * network ( int prefix_length ) const = 0;
};

class IPv4Address : public IPAddress {
  private:
    union {
      unsigned int value;
      unsigned char octet[4];
    };
    IPv4Address ( unsigned int address );
  public:
    IPv4Address ( const IPv4Address & );
    IPv4Address & operator = ( const IPv4Address & );
    virtual ~IPv4Address ();
    IPv4Address ();
    virtual IPv4Address * clone () const;
    static IPv4Address * parse ( const std::string & );
    virtual int length () const;
    virtual std::string string () const;
    virtual IPNetwork <> * network ( int prefix_length ) const;
    IPv4Address mask ( int prefix_length ) const;
    IPv4Address shift ( int prefix_length ) const;
    bool operator < ( const IPv4Address &b ) const;
    bool operator <= ( const IPv4Address &b ) const;
    IPv4Address operator - ( int delta ) const;
};

class IPv6Address : public IPAddress {
  private:
    union {
      unsigned __int128 value;
      unsigned short block[8];
      unsigned char octet[16];
    };
    IPv6Address ( const unsigned short address[8] );
    IPv6Address ( unsigned __int128 address );
  public:
    IPv6Address ( const IPv6Address & );
    IPv6Address & operator = ( const IPv6Address & );
    virtual ~IPv6Address ();
    IPv6Address ();
    virtual IPv6Address * clone () const;
    static IPv6Address * parse ( const std::string & );
    virtual int length () const;
    virtual std::string string () const;
    virtual IPNetwork <> * network ( int prefix_length ) const;
    IPv6Address mask ( int prefix_length ) const;
    IPv6Address shift ( int prefix_length ) const;
    bool operator < ( const IPv6Address &b ) const;
    bool operator <= ( const IPv6Address &b ) const;
    IPv6Address operator - ( int delta ) const;
};


template <>
  class IPNetwork <IPAddress> {
    protected:
      int mPrefixLength;
    protected:
      IPNetwork ( const IPNetwork <IPAddress> & );
      IPNetwork& operator = ( const IPNetwork <IPAddress> & );
      IPNetwork ();
      IPNetwork ( int );
    public:
      virtual ~IPNetwork ();
      virtual IPNetwork <IPAddress> * clone () const = 0;
      static IPNetwork <IPAddress> * parse ( const std::string & );
      virtual std::string string () const = 0;
      virtual bool contains ( const IPAddress & ) const = 0;
      int prefix_length () const;
  };

template <class Address>
  requires std::is_base_of<IPAddress, Address>::value
  // class IPNetwork : private Address {
  class IPNetwork : public IPNetwork <IPAddress>, private Address {
    public:
      IPNetwork ( const IPNetwork <Address> & );
      IPNetwork& operator = ( const IPNetwork <Address> & );
      virtual ~IPNetwork ();
      IPNetwork ();
      IPNetwork ( const Address &, int );
      virtual IPNetwork <Address> * clone () const;
      static IPNetwork <Address> * parse ( const std::string & );
      virtual std::string string () const;
      virtual bool contains ( const IPAddress & ) const;
  };


template <class Address>
  IPNetwork <Address>::IPNetwork ( const IPNetwork <Address> &net )
    : IPNetwork <IPAddress> (net), Address (net) {
  }

template <class Address>
  IPNetwork <Address>::~IPNetwork () {
  }

template <class Address>
  IPNetwork <Address>::IPNetwork () : IPNetwork <IPAddress> (), Address () {
  }

template <class Address>
  IPNetwork <Address>::IPNetwork ( const Address &address, int prefix_length )
    : IPNetwork <IPAddress> (prefix_length), Address (address.mask (prefix_length)) {
  }

template <class Address>
  IPNetwork <Address> * IPNetwork <Address>::clone () const {
    return new IPNetwork <Address> (*this);
  }

template <class Address>
  std::string IPNetwork <Address>::string () const {
    return Address::string () + '/' + std::to_string (mPrefixLength);
  }

template <class Address>
  bool IPNetwork <Address>::contains ( const IPAddress &address ) const {
    const Address *x = dynamic_cast <const Address*> (&address);
    if (!x) {
      return false;
    }
    Address last = this->shift (mPrefixLength) - 1;
    // fprintf (stderr, "[debug] last = %s\n", last.string ().c_str ());
    return (*this) <= (*x) && (*x) <= last;
  }


