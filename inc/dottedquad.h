#ifndef DOTTEDQUAD_H
#define DOTTEDQUAD_H

char* __fastcall__ dotted_quad(unsigned long quad);

// Convert a string representing a dotted quad (IP address, netmask) into 4 octets
//
// Inputs: quad: Zero terminated string containing dotted quad (e.g. "192.168.1.0"),
//               to simplify URL parsing, a ':' or '/' can also terminate the string.
// Output: IP address, 0 on error
//
unsigned long __fastcall__ parse_dotted_quad(char* quad);

// Minimal DHCP client implementation
//
// IP addresses are requested from a DHCP server (aka 'leased') but are not renewed
// or released. Although this is not correct behaviour according to  the DHCP RFC,
// this works fine in practice in a typical home network environment.
//
// Inputs: None (although ip65_init should be called first)
// Output: 0 if IP config has been sucesfully obtained and cfg_ip, cfg_netmask,
//           cfg_gateway and cfg_dns will be set per response from dhcp server.
//           dhcp_server will be set to address of server that provided configuration.
//         1 if there was an error

#endif