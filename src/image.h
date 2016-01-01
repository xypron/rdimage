/*
 * (C) Copyright 2008 Semihalf
 *
 * (C) Copyright 2000-2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdint.h>

#define IH_MAGIC        0x27051956      /* Image Magic Number           */
#define IH_NMLEN                32      /* Image Name Length            */

/*
 * Big endian 32bit value
 */
typedef struct {
	uint8_t u[4];
} be32_t;

typedef struct image_header {
	be32_t		ih_magic;	/* Image Header Magic Number	*/
	be32_t		ih_hcrc;	/* Image Header CRC Checksum	*/
	be32_t		ih_time;	/* Image Creation Timestamp	*/
	be32_t		ih_size;	/* Image Data Size		*/
	be32_t		ih_load;	/* Data	 Load  Address		*/
	be32_t		ih_ep;		/* Entry Point Address		*/
	be32_t		ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		ih_name[IH_NMLEN];	/* Image Name		*/
} image_header_t;

#endif	/* __IMAGE_H__ */
