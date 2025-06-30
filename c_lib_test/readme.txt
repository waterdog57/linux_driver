gcc -shared -fPIC -o libmylib.so -lusb-1.0  ipc_tool.c
gcc main.c -o test -lusb-1.0
