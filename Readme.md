# Distributed File Compression System

A distributed file compression and decompression system implemented in C++ with a modern web frontend. The system provides lossless compression using Run-Length Encoding (RLE) with adaptive algorithm selection, ensuring optimal compression ratios by automatically choosing between RLE and identity (no-op) compression.

The backend implements a client-server architecture supporting both raw TCP and HTTP/1.1 protocols, with multi-threaded concurrent request handling. The design demonstrates object-oriented programming principles including inheritance, polymorphism, and encapsulation through an extensible compression algorithm interface.

Key features include binary-safe file I/O, cross-platform socket networking, thread-per-connection concurrency, and a RESTful HTTP API. The system is containerized with Docker and deployable to cloud platforms. A React-based frontend provides an intuitive web interface for file compression operations.

**Ref: University of Lincoln - CMP9133-2526 Assessment**

