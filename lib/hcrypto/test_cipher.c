/*
 * Copyright (c) 2006 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef RCSID
RCSID("$Id$");
#endif

#define HC_DEPRECATED_CRYPTO

#include <sys/types.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getarg.h>
#include <roken.h>

#include <evp.h>
#include <hex.h>
#include <err.h>

struct tests {
    const char *name;
    void *key;
    size_t keysize;
    void *iv;
    size_t datasize;
    void *indata;
    void *outdata;
    void *outiv;
};

struct tests aes_tests[] = {
    { "aes-256",
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      32,
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      16,
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      "\xdc\x95\xc0\x78\xa2\x40\x89\x89\xad\x48\xa2\x14\x92\x84\x20\x87"
    }
};

struct tests aes_128_cts_tests[] = {
    {
	"aes-128-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69",
	16,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	17,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20",
	"\xc6\x35\x35\x68\xf2\xbf\x8c\xb4\xd8\xa5\x80\x36\x2d\xa7\xff\x7f"
	"\x97",
	"\xc6\x35\x35\x68\xf2\xbf\x8c\xb4\xd8\xa5\x80\x36\x2d\xa7\xff\x7f"
    },
    {
	"aes-128-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69",
	16,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	31,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20",
	"\xfc\x00\x78\x3e\x0e\xfd\xb2\xc1\xd4\x45\xd4\xc8\xef\xf7\xed\x22"
	"\x97\x68\x72\x68\xd6\xec\xcc\xc0\xc0\x7b\x25\xe2\x5e\xcf\xe5",
	"\xfc\x00\x78\x3e\x0e\xfd\xb2\xc1\xd4\x45\xd4\xc8\xef\xf7\xed\x22"
    },
    {
	"aes-128-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69",
	16,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	32,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43",
	"\x39\x31\x25\x23\xa7\x86\x62\xd5\xbe\x7f\xcb\xcc\x98\xeb\xf5\xa8"
	"\x97\x68\x72\x68\xd6\xec\xcc\xc0\xc0\x7b\x25\xe2\x5e\xcf\xe5\x84",
	"\x39\x31\x25\x23\xa7\x86\x62\xd5\xbe\x7f\xcb\xcc\x98\xeb\xf5\xa8"
    },
    {
	"aes-128-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69",
	16,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	47,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c",
	"\x97\x68\x72\x68\xd6\xec\xcc\xc0\xc0\x7b\x25\xe2\x5e\xcf\xe5\x84"
	"\xb3\xff\xfd\x94\x0c\x16\xa1\x8c\x1b\x55\x49\xd2\xf8\x38\x02\x9e"
	"\x39\x31\x25\x23\xa7\x86\x62\xd5\xbe\x7f\xcb\xcc\x98\xeb\xf5",
	"\xb3\xff\xfd\x94\x0c\x16\xa1\x8c\x1b\x55\x49\xd2\xf8\x38\x02\x9e"
    },
    {
	"aes-128-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69",
	16,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	48,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c\x20",
	"\x97\x68\x72\x68\xd6\xec\xcc\xc0\xc0\x7b\x25\xe2\x5e\xcf\xe5\x84"
	"\x9d\xad\x8b\xbb\x96\xc4\xcd\xc0\x3b\xc1\x03\xe1\xa1\x94\xbb\xd8"
	"\x39\x31\x25\x23\xa7\x86\x62\xd5\xbe\x7f\xcb\xcc\x98\xeb\xf5\xa8",
	"\x9d\xad\x8b\xbb\x96\xc4\xcd\xc0\x3b\xc1\x03\xe1\xa1\x94\xbb\xd8"
    },
    {
	"aes-128-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69",
	16,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	64,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c\x20"
	"\x61\x6e\x64\x20\x77\x6f\x6e\x74\x6f\x6e\x20\x73\x6f\x75\x70\x2e",
	"\x97\x68\x72\x68\xd6\xec\xcc\xc0\xc0\x7b\x25\xe2\x5e\xcf\xe5\x84"
	"\x39\x31\x25\x23\xa7\x86\x62\xd5\xbe\x7f\xcb\xcc\x98\xeb\xf5\xa8"
	"\x48\x07\xef\xe8\x36\xee\x89\xa5\x26\x73\x0d\xbc\x2f\x7b\xc8\x40"
	"\x9d\xad\x8b\xbb\x96\xc4\xcd\xc0\x3b\xc1\x03\xe1\xa1\x94\xbb\xd8",
	"\x48\x07\xef\xe8\x36\xee\x89\xa5\x26\x73\x0d\xbc\x2f\x7b\xc8\x40"
    }
};


struct tests aes_256_cts_tests[] = {
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	17,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20",
	"\x5c\x13\x26\x27\xc4\xcb\xca\x04\x14\x43\x8a\xb5\x97\x97\x7c\x10"
	"\x16"
    },
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	31,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20",
	"\x16\xb3\xd8\xe5\xcd\x93\xe6\x2c\x28\x70\xa0\x36\x6e\x9a\xb9\x74"
	"\x16\xc1\xee\xdf\x39\xc8\x3f\xfb\xc5\xf6\x72\xe9\xc1\x6e\x53"
    },
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	32,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43",
	"\x69\xde\xce\x59\x83\x6a\x82\xe1\xcd\x21\x93\xd0\x9e\x2a\xff\xc8"
	"\x16\xc1\xee\xdf\x39\xc8\x3f\xfb\xc5\xf6\x72\xe9\xc1\x6e\x53\x0c"
    },
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	47,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c",
	"\x16\xc1\xee\xdf\x39\xc8\x3f\xfb\xc5\xf6\x72\xe9\xc1\x6e\x53\x0c"
	"\xe5\x56\xb4\x88\x41\xb9\xde\x27\xf0\x07\xa1\x6e\x89\x94\x47\xf1"
	"\x69\xde\xce\x59\x83\x6a\x82\xe1\xcd\x21\x93\xd0\x9e\x2a\xff"
    },
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	48,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c\x20",
	"\x16\xc1\xee\xdf\x39\xc8\x3f\xfb\xc5\xf6\x72\xe9\xc1\x6e\x53\x0c"
	"\xfd\x68\xd1\x56\x32\x23\x7b\xfa\xb0\x09\x86\x3b\x17\x53\xfa\x30"
	"\x69\xde\xce\x59\x83\x6a\x82\xe1\xcd\x21\x93\xd0\x9e\x2a\xff\xc8"
    },
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	64,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c\x20"
	"\x61\x6e\x64\x20\x77\x6f\x6e\x74\x6f\x6e\x20\x73\x6f\x75\x70\x2e",
	"\x16\xc1\xee\xdf\x39\xc8\x3f\xfb\xc5\xf6\x72\xe9\xc1\x6e\x53\x0c"
	"\x69\xde\xce\x59\x83\x6a\x82\xe1\xcd\x21\x93\xd0\x9e\x2a\xff\xc8"
	"\x70\x29\xf2\x6f\x7c\x79\xc1\x77\x91\xad\x94\xb0\x78\x62\x27\x67"
	"\xfd\x68\xd1\x56\x32\x23\x7b\xfa\xb0\x09\x86\x3b\x17\x53\xfa\x30"
    },
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	78,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c\x20"
	"\x61\x6e\x64\x20\x77\x6f\x6e\x74\x6f\x6e\x20\x73\x6f\x75\x70\x2e"
	"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41",
	"\x16\xc1\xee\xdf\x39\xc8\x3f\xfb\xc5\xf6\x72\xe9\xc1\x6e\x53\x0c"
	"\x69\xde\xce\x59\x83\x6a\x82\xe1\xcd\x21\x93\xd0\x9e\x2a\xff\xc8"
	"\xfd\x68\xd1\x56\x32\x23\x7b\xfa\xb0\x09\x86\x3b\x17\x53\xfa\x30"
	"\x73\xfb\x2c\x36\x76\xaf\xcf\x31\xff\xe3\x8a\x89\x0c\x7e\x99\x3f"
	"\x70\x29\xf2\x6f\x7c\x79\xc1\x77\x91\xad\x94\xb0\x78\x62"
    },
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	83,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c\x20"
	"\x61\x6e\x64\x20\x77\x6f\x6e\x74\x6f\x6e\x20\x73\x6f\x75\x70\x2e"
	"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
	"\x41\x41\x41",
	"\x16\xc1\xee\xdf\x39\xc8\x3f\xfb\xc5\xf6\x72\xe9\xc1\x6e\x53\x0c"
	"\x69\xde\xce\x59\x83\x6a\x82\xe1\xcd\x21\x93\xd0\x9e\x2a\xff\xc8"
	"\xfd\x68\xd1\x56\x32\x23\x7b\xfa\xb0\x09\x86\x3b\x17\x53\xfa\x30"
	"\x70\x29\xf2\x6f\x7c\x79\xc1\x77\x91\xad\x94\xb0\x78\x62\x27\x67"
	"\x65\x39\x3a\xdb\x92\x05\x4d\x4f\x08\xa1\xfa\x59\xda\x56\x58\x0e"
	"\x3b\xac\x12"
    },
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	92,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c\x20"
	"\x61\x6e\x64\x20\x77\x6f\x6e\x74\x6f\x6e\x20\x73\x6f\x75\x70\x2e"
	"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
	"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41",
	"\x16\xc1\xee\xdf\x39\xc8\x3f\xfb\xc5\xf6\x72\xe9\xc1\x6e\x53\x0c"
	"\x69\xde\xce\x59\x83\x6a\x82\xe1\xcd\x21\x93\xd0\x9e\x2a\xff\xc8"
	"\xfd\x68\xd1\x56\x32\x23\x7b\xfa\xb0\x09\x86\x3b\x17\x53\xfa\x30"
	"\x70\x29\xf2\x6f\x7c\x79\xc1\x77\x91\xad\x94\xb0\x78\x62\x27\x67"
	"\x0c\xff\xd7\x63\x50\xf8\x4e\xf9\xec\x56\x1c\x79\xc5\xc8\xfe\x50"
	"\x3b\xac\x12\x6e\xd3\x2d\x02\xc4\xe5\x06\x43\x5f"
    },
    {
	"aes-256-cts",
	"\x63\x68\x69\x63\x6b\x65\x6e\x20\x74\x65\x72\x69\x79\x61\x6b\x69"
	"\x2c\x20\x79\x75\x6d\x6d\x79\x20\x79\x75\x6d\x6d\x79\x21\x21\x21",
	32,
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
	96,
	"\x49\x20\x77\x6f\x75\x6c\x64\x20\x6c\x69\x6b\x65\x20\x74\x68\x65"
	"\x20\x47\x65\x6e\x65\x72\x61\x6c\x20\x47\x61\x75\x27\x73\x20\x43"
	"\x68\x69\x63\x6b\x65\x6e\x2c\x20\x70\x6c\x65\x61\x73\x65\x2c\x20"
	"\x61\x6e\x64\x20\x77\x6f\x6e\x74\x6f\x6e\x20\x73\x6f\x75\x70\x2e"
	"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41"
	"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41",
	"\x16\xc1\xee\xdf\x39\xc8\x3f\xfb\xc5\xf6\x72\xe9\xc1\x6e\x53\x0c"
	"\x69\xde\xce\x59\x83\x6a\x82\xe1\xcd\x21\x93\xd0\x9e\x2a\xff\xc8"
	"\xfd\x68\xd1\x56\x32\x23\x7b\xfa\xb0\x09\x86\x3b\x17\x53\xfa\x30"
	"\x70\x29\xf2\x6f\x7c\x79\xc1\x77\x91\xad\x94\xb0\x78\x62\x27\x67"
	"\x08\x28\x49\xad\xfc\x2d\x8e\x86\xae\x69\xa5\xa8\xd9\x29\x9e\xe4"
	"\x3b\xac\x12\x6e\xd3\x2d\x02\xc4\xe5\x06\x43\x5f\x4c\x41\xd1\xb8"
    }
};


struct tests rc2_40_tests[] = {
    { "rc2-40",
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      16,
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      16,
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      "\xc0\xb8\xff\xa5\xd6\xeb\xc9\x62\xcc\x52\x5f\xfe\x9a\x3c\x97\xe6"
    }
};

struct tests des_ede3_tests[] = {
    { "des-ede3",
      "\x19\x17\xff\xe6\xbb\x77\x2e\xfc"
      "\x29\x76\x43\xbc\x63\x56\x7e\x9a"
      "\x00\x2e\x4d\x43\x1d\x5f\xfd\x58",
      24,
      "\xbf\x9a\x12\xb7\x26\x69\xfd\x05",
      16,
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      "\x55\x95\x97\x76\xa9\x6c\x66\x40\x64\xc7\xf4\x1c\x21\xb7\x14\x1b"
    }
};

struct tests camellia128_tests[] = {
    { "camellia128",
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      16,
      "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      16,
      "\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
      "\x07\x92\x3A\x39\xEB\x0A\x81\x7D\x1C\x4D\x87\xBD\xB8\x2D\x1F\x1C"
    }
};


static int
test_cipher(int i, const EVP_CIPHER *c, struct tests *t)
{
    EVP_CIPHER_CTX ectx;
    EVP_CIPHER_CTX dctx;
    void *d;

    EVP_CIPHER_CTX_init(&ectx);
    EVP_CIPHER_CTX_init(&dctx);

    if (EVP_CipherInit_ex(&ectx, c, NULL, t->key, t->iv, 1) != 1)
	errx(1, "%s: %d EVP_CipherInit_ex encrypt", t->name, i);
    if (EVP_CipherInit_ex(&dctx, c, NULL, t->key, t->iv, 0) != 1)
	errx(1, "%s: %d EVP_CipherInit_ex decrypt", t->name, i);

    d = emalloc(t->datasize);

    if (!EVP_Cipher(&ectx, d, t->indata, t->datasize))
	return 1;

    if (memcmp(d, t->outdata, t->datasize) != 0) {
	char *s, *s2;
	hex_encode(d, t->datasize, &s);
	hex_encode(t->outdata, t->datasize, &s2);
	errx(1, "%s: %d encrypt not the same: %s != %s", t->name, i, s, s2);
    }

    if (!EVP_Cipher(&dctx, d, d, t->datasize))
	return 1;

    if (memcmp(d, t->indata, t->datasize) != 0) {
	char *s;
	hex_encode(d, t->datasize, &s);
	errx(1, "%s: %d decrypt not the same: %s", t->name, i, s);
    }
    if (t->outiv)
	/* XXXX check  */;

    EVP_CIPHER_CTX_cleanup(&ectx);
    EVP_CIPHER_CTX_cleanup(&dctx);
    free(d);

    return 0;
}

static int version_flag;
static int help_flag;

static struct getargs args[] = {
    { "version",	0,	arg_flag,	&version_flag,
      "print version", NULL },
    { "help",		0,	arg_flag,	&help_flag,
      NULL, 	NULL }
};

static void
usage (int ret)
{
    arg_printusage (args,
		    sizeof(args)/sizeof(*args),
		    NULL,
		    "");
    exit (ret);
}

int
main(int argc, char **argv)
{
    int ret = 0;
    int i, idx = 0;

    setprogname(argv[0]);

    if(getarg(args, sizeof(args) / sizeof(args[0]), argc, argv, &idx))
	usage(1);
    
    if (help_flag)
	usage(0);

    if(version_flag){
	print_version(NULL);
	exit(0);
    }

    argc -= idx;
    argv += idx;

    for (i = 0; i < sizeof(aes_128_cts_tests)/sizeof(aes_128_cts_tests[0]); i++)
	ret += test_cipher(i, EVP_hcrypto_aes_128_cts(), &aes_128_cts_tests[i]);
    for (i = 0; i < sizeof(aes_256_cts_tests)/sizeof(aes_256_cts_tests[0]); i++)
	ret += test_cipher(i, EVP_hcrypto_aes_256_cts(), &aes_256_cts_tests[i]);
    for (i = 0; i < sizeof(aes_tests)/sizeof(aes_tests[0]); i++)
	ret += test_cipher(i, EVP_aes_256_cbc(), &aes_tests[i]);
    for (i = 0; i < sizeof(rc2_40_tests)/sizeof(rc2_40_tests[0]); i++)
	ret += test_cipher(i, EVP_rc2_40_cbc(), &rc2_40_tests[i]);
    for (i = 0; i < sizeof(des_ede3_tests)/sizeof(des_ede3_tests[0]); i++)
	ret += test_cipher(i, EVP_des_ede3_cbc(), &des_ede3_tests[i]);
    for (i = 0; i < sizeof(camellia128_tests)/sizeof(camellia128_tests[0]); i++)
	ret += test_cipher(i, EVP_camellia_128_cbc(), &camellia128_tests[i]);

    return ret;
}
