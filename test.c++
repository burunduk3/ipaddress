#include <cassert>
#include <cstdio>

#include "ipaddress.h"

int main() {
  printf ("cplusplus = %ld\n", __cplusplus);
  IPNetwork <IPv4Address> ipv4;
  IPNetwork <IPv6Address> ipv6;
  IPNetwork <> *net1 = IPNetwork <IPAddress>::parse ("95.142.200.0/21");
  IPNetwork <> *net2 = IPNetwork <IPAddress>::parse ("2a00:bdc0:e006::/48");
  IPNetwork <> *net3 = IPNetwork <IPAddress>::parse ("::1");
  IPNetwork <> *net4 = IPNetwork <IPAddress>::parse ("::1/0");
  assert (net1);
  assert (net2);
  assert (net3);
  assert (net4);
  printf ("net1 = %s\n", net1->string ().c_str ());
  printf ("net2 = %s\n", net2->string ().c_str ());
  printf ("net3 = %s\n", net3->string ().c_str ());
  printf ("net4 = %s\n", net4->string ().c_str ());
  const char *ips[] = {
    "95.142.200.34",
    "127.0.0.1",
    "2a00:bdc0:e006::ffff",
    "::1",
    "::2",
    0
  };
  for (const char **x = ips; *x; x = x + 1) {
    IPAddress *y = IPAddress::parse (*x);
    printf ("%s in net1: %s\n", *x, net1->contains (*y) ? "true" : "false");
    printf ("%s in net2: %s\n", *x, net2->contains (*y) ? "true" : "false");
    printf ("%s in net3: %s\n", *x, net3->contains (*y) ? "true" : "false");
    printf ("%s in net4: %s\n", *x, net4->contains (*y) ? "true" : "false");
    delete y;
  }
  // IPNetwork <unsigned int> ipvfail;
  return 0;
}

