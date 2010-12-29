/* Copyright 2010 Dave Barnard (www.shoalresearch.com) */
#ifndef INC_PCAP_H
#define INC_PCAP_H

#include <stdint.h>

void open_pcap();

void write_pcap(const uint8_t *buffer, const uint16_t buffer_len);


#endif
