#pragma once
#ifndef PICOHTTP_H_HEADERGUARD
#define PICOHTTP_H_HEADERGUARD

#include <stdlib.h>
#include <stdint.h>

#define PICOHTTP_MAJORVERSION(x) ( (x & 0x7f00) >> 8 )
#define PICOHTTP_MINORVERSION(x) ( (x & 0x007f) )

#define PICOHTTP_METHOD_GET  1
#define PICOHTTP_METHOD_HEAD 2
#define PICOHTTP_METHOD_POST 3

struct picohttpIoOps {
	int (*read)(size_t /*count*/, char* /*buf*/, void*);
	int (*write)(size_t /*count*/, char const* /*buf*/, void*);
	int16_t (*getch)(void*); // returns -1 on error
	int (*putch)(char, void*);
	void *data;
};

#define picohttpIoWrite(ioops,size,buf) (ioops->write(size, buf, ioops->data))
#define picohttpIoRead(ioops,size,buf)  (ioops->read(size, buf, ioops->data))
#define picohttpIoGetch(ioops)          (ioops->getch(ioops->data))
#define picohttpIoPutch(ioops,c)        (ioops->putch(c, ioops->data))

enum picohttpVarType {
	PICOHTTP_TYPE_UNDEFINED = 0,
	PICOHTTP_TYPE_INTEGER = 1,
	PICOHTTP_TYPE_REAL = 2,
	PICOHTTP_TYPE_BOOLEAN = 3,
	PICOHTTP_TYPE_TEXT = 4
};

struct picohttpVarSpec {
	char const * const name;
	enum picohttpVarType type;
	size_t max_len;
};

struct picohttpVar {
	struct picohttpVarSpec const *spec;
	union {
		char *text;
		float real;
		int integer;
		uint8_t boolean;
	} value;
	struct picohttpVar *next;
};

struct picohttpRequest;

typedef void (*picohttpHandler)(struct picohttpRequest*);

struct picohttpURLRoute {
	char const * urlhead;
	struct picohttpVarSpec const * get_vars;
	picohttpHandler handler;
	uint16_t max_urltail_len;
	int16_t allowed_methods;
};

struct picohttpRequest {
	struct picohttpIoOps const * ioops;
	struct picohttpURLRoute const * route;
	struct picohttpVar *get_vars;
	char const *url;
	char const *urltail;
	int16_t status;
	int16_t method;
	struct {
		uint8_t major;
		uint8_t minor;
	} httpversion;
	struct {
		uint8_t encoding;
		char const *contenttype;
		size_t contentlength;
	} queryheader;
	struct {
		uint8_t encoding;
		char const *contenttype;
		char const *date;
		char const *cachecontrol;
		char const *disposition;
		size_t contentlength;
	} responseheader;
};

void picohttpProcessRequest(
	struct picohttpIoOps const * const ioops,
	struct picohttpURLRoute const * const routes );

#endif/*PICOHTTP_H_HEADERGUARD*/