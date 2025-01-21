# Makefile for IPC server
PROG=	cond
SRCS=	main.c func.c structures.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys

MAN=

BINDIR?= /usr/sbin
FILES=cond.conf
FILESNAME=cond
FILESDIR= /etc/system.conf.d

.include <minix.service.mk>
