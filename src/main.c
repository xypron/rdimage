/* main.c
 *
 * Copyright (C) 2016, Heinrich Schuchardt <xypron.glpk@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "image.h"
#include "config.h"

#define BUFSIZE 0x100000

static void
copy(int fd, int fdo)
{
	ssize_t i;
	char *buf;

	buf = malloc(BUFSIZE);
	if (buf == NULL) {
		fprintf(stderr, "Out of memory");
		exit(EXIT_FAILURE);
	}
	for (;;) {
		i = read(fd, buf, BUFSIZE);
		if (i == -1) {
			perror("Failed to read");
			exit(EXIT_FAILURE);
		}
		if (i == 0) {
			break;
		}
		i = write(fdo, buf, i);
		if (i == -1) {
			perror("Failed to write");
			exit(EXIT_FAILURE);
		}
	}
	free(buf);
}

static const char *
comp2char(uint8_t comp)
{
	switch(comp) {
	case 0:
		return("No compression used");
	case 1:
		return("Gzip compression used");
	case 2:
		return("Bzip2 compression used");
	case 3:
		return("Lzma compression used");
	case 4:
		return("Lzo compression used");
	case 5:
		return("Lz4 compression used");
	default:
		return("Unknown compression");
	}
}

static const char *
type2char(uint8_t type)
{
	switch(type) {
	case 0:
		return("Invalid Image");
	case 1:
		return("Standalone Program");
	case 2:
		return("OS Kernel Image");
	case 3:
		return("RAMDisk Image");
	case 4:
		return("Multi-File Image");
	case 5:
		return("Firmware Image");
	case 6:
		return("Script file");
	case 7:
		return("Filesystem Image (any type)");
	case 8:
		return("Binary Flat Device Tree Blob");
	case 9:
		return("Kirkwood Boot Image");
	case 10:
		return("Freescale IMXBoot Image");
	case 11:
		return("Davinci UBL Image");
	case 12:
		return("TI OMAP Config Header Image");
	case 13:
		return("TI Davinci AIS Image");
	case 14:
		return("OS Kernel Image, can run from any load address");
	case 15:
		return("Freescale PBL Boot Image");
	case 16:
		return("Freescale MXSBoot Image");
	case 17:
		return("TI Keystone GPHeader Image");
	case 18:
		return("ATMEL ROM bootable Image");
	case 19:
		return("Altera SOCFPGA Preloader");
	case 20:
		return("x86 setup.bin Image");
	case 21:
		return("x86 setup.bin Image");
	case 22:
		return("A list of typeless images");
	case 23:
		return("Rockchip Boot Image");
	case 24:
		return("Rockchip SD card");
	case 25:
		return("Rockchip SPI image");
	case 26:
		return("Xilinx Zynq Boot Image");
	default:
		return("Unknown image type");
	}
}

static const char *
os2char(uint8_t os)
{
	switch(os) {
	case 0:
		return("Invalid OS");
	case 1:
		return("OpenBSD");
	case 2:
		return("NetBSD");
	case 3:
		return("FreeBSD");
	case 4:
		return("4.4BSD");
	case 5:
		return("Linux");
	case 6:
		return("SVR4");
	case 7:
		return("Esix");
	case 8:
		return("Solaris");
	case 9:
		return("Irix");
	case 10:
		return("SCO");
	case 11:
		return("Dell");
	case 12:
		return("NCR");
	case 13:
		return("LynxOS");
	case 14:
		return("VxWorks");
	case 15:
		return("pSOS");
	case 16:
		return("QNX");
	case 17:
		return("Firmware");
	case 18:
		return("RTEMS");
	case 19:
		return("ARTOS");
	case 20:
		return("Unity OS");
	case 21:
		return("INTEGRITY");
	case 22:
		return("OSE");
	case 23:
		return("Plan 9");
	case 24:
		return("OpenRTOS");
	default:
		return("Unknown operating system");
	}
}

static const char *
arch2char(uint8_t arch)
{
	switch (arch) {
	case 0:
		return "Invalid CPU";
	case 1:
		return "Alpha";
	case 2:
		return "ARM";
	case 3:
		return "Intel x86";
	case 4:
		return "IA64";
	case 5:
		return "MIPS";
	case 6:
		return "MIPS 64 Bit";
	case 7:
		return "PowerPC";
	case 8:
		return "IBM S390";
	case 9:
		return "SuperH";
	case 10:
		return "Sparc";
	case 11:
		return "Sparc 64 Bit";
	case 12:
		return "M68K";
	case 14:
		return "MicroBlaze";
	case 15:
		return "Nios-II";
	case 16:
		return "Blackfin";
	case 17:
		return "AVR32";
	case 18:
		return "STMicroelectronics ST200";
	case 19:
		return "Sandbox architecture (test only)";
	case 20:
		return "ANDES Technology - NDS32";
	case 21:
		return "OpenRISC 1000";
	case 22:
		return "ARM64";
	case 23:
		return "Synopsys DesignWare ARC";
	case 24:
		return "AMD x86_64, Intel and Via";
	default	:
		return "Unknown architecture";
	}
}

static uint32_t
swap32(be32_t in)
{
	return ((uint32_t) in.u[0] << 24)
	       | ((uint32_t) in.u[1] << 16)
	       | ((uint32_t) in.u[2] <<  8)
	       | ((uint32_t) in.u[3] <<  0);
}

static void
version ()
{
	printf("rdimage %s\n\n%s\n", VERSION,
	       "Copyright (C) 2016, Heinrich Schuchardt <xypron.glpk@gmx.de>\n"
	       "\nThis program has ABSOLUTELY NO WARRANTY.\n"
	       "This program is free software; you may re-distribute it under\n"
	       "the terms of the GNU General Public License version 2.\n");
}

static void
usage ()
{
	printf("Usage: rdimage [OPTION] ... FILENAME\n"
	       "Extracts a kernel image from an uImage file\n"
	       "\n"
	       "  -v           version information\n"
	       "  -x OUTFILE   extract image to OUTFILE\n"
	      );
	exit(EXIT_FAILURE);
}

static void
parse_header(int fd)
{
	image_header_t header;
	ssize_t ret;
	time_t time;
	char buffer[32];

	ret = read(fd, &header, sizeof(header));
	if (ret == -1) {
		fprintf(stderr, "Failed to read header.\n");
		exit(EXIT_FAILURE);
	}
	if (ret != sizeof(header)) {
		fprintf(stderr, "File too short.\n");
		exit(EXIT_FAILURE);
	}
	if (swap32(header.ih_magic) != IH_MAGIC) {
		fprintf(stderr, "Not an uImage file.\n");
		exit(EXIT_FAILURE);
	}
	header.ih_name[IH_NMLEN - 1] = 0;
	printf("Image name:       %s\n", header.ih_name);
	time = swap32(header.ih_time);
	strftime(buffer, sizeof(buffer), "%F %T UTC", gmtime(&time));
	printf("Creation date:    %s\n", buffer);
	printf("Load address:     %x\n", swap32(header.ih_load));
	printf("Entry address:    %x\n", swap32(header.ih_ep));
	printf("Operating system: %s\n", os2char(header.ih_os));
	printf("Architecture:     %s\n", arch2char(header.ih_arch));
	printf("Image type:       %s\n", type2char(header.ih_type));
	printf("Compression type: %s\n", comp2char(header.ih_comp));
	if (header.ih_comp == 0) {
		ret = lseek(fd, 0x24, SEEK_CUR);
		if (ret == -1) {
			fprintf(stderr, "Failed to read header.\n");
			exit(EXIT_FAILURE);
		}
		ret = read(fd, &buffer, 4);
		if (ret != 4) {
			fprintf(stderr, "Failed to read header.\n");
			exit(EXIT_FAILURE);
		}
		if (buffer[0] == 0x18 && buffer[1] == 0x28 &&
		    buffer[2] == 0x6F && buffer[3] == 0x01) {
			printf("Payload type:     zImage\n");
		} else {
			printf("Payload type:     Image\n");
		}
		ret = lseek(fd, sizeof(image_header_t), SEEK_SET);
		if (ret == -1) {
			fprintf(stderr, "Failed to read header.\n");
			exit(EXIT_FAILURE);
		}
	}
}

int main(int argc, char *argv[])
{
	int fd, fdo;
	int i;
	int ret;
	char *infile = NULL;
	char *outfile = NULL;

	for (i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'v':
				version();
				exit(EXIT_SUCCESS);
			case 'x':
				++i;
				if (i >= argc) {
					usage();
				}
				outfile = argv[i];
				break;
			default:
				usage();
			}
		} else {
			if (infile == NULL) {
				infile = argv[i];
			} else {
				usage();
			}
		}
	}
	if (infile == NULL) {
		usage();
	}

	fd = open(infile, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open '%s'.\n", infile);
		perror("");
		exit(EXIT_FAILURE);
	}

	parse_header(fd);

	if (outfile != NULL) {
		fdo = open(outfile, O_WRONLY | O_CREAT, 0664);
		if (fdo == -1) {
			fprintf(stderr, "Failed to open '%s'.\n", outfile);
			perror("");
			exit(EXIT_FAILURE);
		}
		ret = chmod(outfile, 0644);
		if (ret == -1) {
			fprintf(stderr, "Failed to chmod '%s'.\n", outfile);
			perror("");
			exit(EXIT_FAILURE);
		}
		copy(fd, fdo);
		close(fdo);
	}

	close(fd);
	exit(EXIT_SUCCESS);
}
