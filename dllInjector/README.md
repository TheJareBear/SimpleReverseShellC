# DLL Injector

This is some c code for a DLL injector. This program will create a handle to a remote process, find the address of LoadLibrary, load your DLL into the mapped memory of the remote process, and execute a thread at the DLLMain of in the remote process
