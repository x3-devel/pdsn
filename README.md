# pdsn - Simple Data Source Name (DNS) parser

`pdsn` follows the Unix philosophy providing data source name parsing and formatting of its elements for common file formats.
There is also a public API so it can be easily integrated into existing projects. 


---

## Features
- DSN parsing
- Common output formats
- Fast 

---

## Requirements
- gcc
- make

---

## Build
Binary destination directory is `build/bin`.
```bash
make
```

## Usage
Syntax is simple:
```bash
pdsn mysql://username:password@localhost/db1

Format: bash
schema="mysql"
username="username"
password="password"
hostname="localhost"
port="3306"
path="db1"
query=""
```

## Development

### Regex Capture Groups
```
0	Full match	postgres://test:pass@localhost:22/db?param=1&param2=2
1	Schema	postgres
2	Full userinfo (test:pass@)	test:pass@
3	Username	test
4	:password portion	:pass
5	Password	pass
6	Hostname	localhost
7	Full :port portion	:22
8	Port	22
9	Full /path portion	/db
10	Path (no slash)	db
11	Full ?query portion	?param=1&param2=2
12	Query string	param=1&param2=2
```
