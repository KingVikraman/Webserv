# Webserv

A custom HTTP server written in C++98.

## Requirements

- `c++` with C++98 support
- `make`

## Project Layout

- `src/` - server source files
- `include/` - header files
- `config/` - server configuration files
- `public/` - static files served by the server
- `uploads/` - upload target directory

## Build

```bash
make
```

This generates the executable:

```bash
./webserv
```

## Run

The server requires a config file argument:

```bash
./webserv config/server.conf
```

If you run without arguments, usage is:

```text
Usage: ./webserv [config_file]
```

## Quick Test

After starting the server, open a browser and visit:

- `http://localhost:8080/`

You can also test with curl:

```bash
curl -i http://localhost:8080/
```

## Rebuild / Clean

```bash
make clean     # remove object files
make fclean    # remove object files + binary
make re        # full rebuild
```
