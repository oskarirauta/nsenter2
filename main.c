#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef MAX_PATH
#define MAX_PATH 30
#endif

//#define DEBUG 1

static int open_ns(char *pid, char *name) {

	char path[MAX_PATH];
	sprintf(path, "/proc/%s/%s", pid, name);
	int i = open(path, O_RDONLY);
	if ( i == -1 )
		printf("failed to open %s\n", path);
	return i;
}

extern char **environ;

int main(int argc, char **argv) {

	if ( argc < 2 ) {

		fprintf(stderr, "error: incorrect arguments\n\nusage: %s pid [command]\ncommand is optional, default is /bin/sh\n", argv[0]);
		exit(1);
	}

	struct stat st;
	if ( stat(argv[1], &st) == 0 && S_ISDIR(st.st_mode)) {

		fprintf(stderr, "error: cannot find /proc/%s - pid does not exist?\n", argv[1]);
		exit(1);
	}

	int ns_ipc = open_ns(argv[1], "ns/ipc");
	int ns_mnt = open_ns(argv[1], "ns/mnt");
	int ns_net = open_ns(argv[1], "ns/net");
	int ns_uts = open_ns(argv[1], "ns/uts");
	int ns_pid = open_ns(argv[1], "ns/pid");
	int ns_root = open_ns(argv[1], "root");

	if ( ns_ipc == -1 || ns_mnt == -1 || ns_net == -1 || ns_uts == -1 || ns_pid == -1 || ns_root == -1 ) {

		fprintf(stderr, "error: failed to open %s namespace\n", argv[1]);
		fprintf(stderr, "ipc: %d, mnt: %d, net: %d, uts: %d, pid: %d, root: %d\n",
			ns_ipc, ns_mnt, ns_net, ns_uts, ns_pid, ns_root);
		exit(1);
	}

	if ( setns(ns_ipc, 0) == -1 || setns(ns_mnt, 0) == -1 || setns(ns_net, 0) == -1 || setns(ns_uts, 0) == -1 || setns(ns_pid, 0) == -1 ) {

		fprintf(stderr, "error: failed to enter namespace of %s\n", argv[1]);
		exit(1);
	}

	if ( fchdir(ns_root) == -1 ) {

		fprintf(stderr, "error: failed to change working directory for %s\n", argv[1]);
		exit(1);
	}

	if ( chroot(".") == -1 ) {

		fprintf(stderr, "error: failed to chroot\n");
		exit(1);
	}

	int i;
	char **args;
	char *cmd = argc > 2 ? argv[2] : "/bin/sh";
	int cnt = argc > 3 ? ( argc - 2 ) : 1;
	args = (char **)malloc(cnt * sizeof(char*));

	if ( argc > 3 ) {

		for ( i = 0; i < cnt; i++ )
			args[i] = argv[i + 2];

	} else args[0] = cmd;

	args[cnt] = NULL;

#ifdef DEBUG
	printf("cmd: %s\n", cmd);

	for ( i = 0; i <= cnt; i++ )
		printf("arg %d: %s\n", i, args[i] == NULL ? "NULL" : args[i]);
#endif

	if ( execve(cmd, args, environ) == -1 ) {

		fprintf(stderr, "error: failed to execute %s in namespace of pid %s\n", cmd, argv[1]);
		return 1;
	}

	return 0;
}
