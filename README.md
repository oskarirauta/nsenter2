### nsenter2
Minimal nsenter command designed mostly for container usage

[![CI build](https://github.com/oskarirauta/nsenter2/actions/workflows/build.yml/badge.svg)](https://github.com/oskarirauta/nsenter2/actions/workflows/build.yml)

### usage
nsenter PID [command]

PID should be container's PID and command is optional (defaults to /bin/sh)
command must exist in container.

### build
make

### debugging
pass DEBUG=1 environment variable for make command

### license
MIT
