// bitfield demo
// REFERENCE: https://stackoverflow.com/questions/11903820/casting-struct-into-int

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


typedef union {
	struct {
		uint16_t messagetype:7; // first bits
		uint16_t prio:4;
		uint16_t forbidden:5; // not to be set, if in case a
				      // can identifier only takes 11
				      // bit

	} __attribute__((__packed__)) fields; // advice the compile to
					      // not align and/or
					      // reorder the fields
	uint16_t bits;
} protocol_id_t;


typedef union {
	struct {
		uint8_t  byte0_audio:1;
		uint8_t  byte0_video:1;
		uint8_t  byte0_light:1;
		uint8_t  byte0_unused:5; // not to be used
		uint16_t byte12;
		uint8_t  byte3a:4;
		uint8_t  byte3b:4;
		uint32_t byte4567;
	} __attribute__((__packed__)) fields;
	uint64_t bits;
} protocol_payload_t;


// NB: globals or statics will be initialized with 0!
protocol_id_t my_id;
protocol_payload_t my_payload;


int main(int argc, char **argv)
{
	uint16_t header = 0x0;
	uint8_t data[8];
	memset(data, 0, sizeof(data));



	// initialization
	my_id.fields.messagetype = 0x7f;
	my_id.fields.prio = 0x01;

	my_payload.fields.byte0_video = 1;
	my_payload.fields.byte3b = 0xb; // NB: the half of the byte it appears to be
	my_payload.fields.byte4567 = 0x12345678; // NB: ordering - big endian / little endian



	fprintf(stderr, "frame: %03x#%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x\n",
		header, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);


	// copy in case of a field
	protocol_id_t transfer = { .bits = *(uint16_t*) &my_id }; // should work w/o cast
	header = transfer.bits;


	// copy in case of an array
	memcpy(data, (uint8_t*) &my_payload.fields, sizeof(my_payload.fields));



	fprintf(stderr, "frame: %03x#%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x\n",
		header, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

	fprintf(stderr, "sizeof(my_payload) = %d, sizeof(my_id) = %d\n", sizeof(my_payload), sizeof(my_id));


	fprintf(stderr, "READY.\n");
	return EXIT_SUCCESS;
};
