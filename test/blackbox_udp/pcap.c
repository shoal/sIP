/* Copyright 2010 Dave Barnard (www.shoalresearch.com) */
#include "pcap.h"

#include <stdint.h>
#include <stdio.h>
FILE *m_fp;
static m_secs = 0;
static m_usecs = 0;


// pcap header
typedef struct pcap_hdr_s {
        uint32_t magic_number;   /* magic number */
        uint16_t version_major;  /* major version number */
        uint16_t version_minor;  /* minor version number */
        int32_t  thiszone;       /* GMT to local correction */
        uint32_t sigfigs;        /* accuracy of timestamps */
        uint32_t snaplen;        /* max length of captured packets, in octets */
        uint32_t network;        /* data link type */
} pcap_hdr_t;


//packet header
typedef struct pcaprec_hdr_s {
        uint32_t ts_sec;         /* timestamp seconds */
        uint32_t ts_usec;        /* timestamp microseconds */
        uint32_t incl_len;       /* number of octets of packet saved in file */
        uint32_t orig_len;       /* actual length of packet */
} pcaprec_hdr_t;


void open_pcap()
{
	// Create pcap file.
	m_fp = fopen("capture.pcap", "w+");
	if(m_fp)
	{
		// pcap header;
		struct pcap_hdr_s pcap_h;
		pcap_h.magic_number = 0xa1b2c3d4;
		pcap_h.version_major = 2;
		pcap_h.version_minor = 4;
		pcap_h.thiszone = 0;
		pcap_h.sigfigs = 0;
		pcap_h.snaplen = 65535;
		pcap_h.network = 1;

		fwrite(&pcap_h, 1, sizeof(pcap_h), m_fp);
	}
}


void write_pcap(const uint8_t *buffer, const uint16_t buffer_len)
{
	// packet header;
	struct pcaprec_hdr_s pac_h;
	pac_h.ts_sec = m_secs++;
	pac_h.ts_usec = ++m_usecs;
	pac_h.incl_len = buffer_len;
	pac_h.orig_len = buffer_len;

	fwrite(&pac_h, 1, sizeof(pac_h), m_fp);

	
	// packet
	fwrite(&buffer[0], 1, buffer_len, m_fp);

}
