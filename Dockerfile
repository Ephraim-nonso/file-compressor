FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Toolchain + deps for FetchContent(Catch2) + pthread + CTest
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    git \
    cmake \
    ninja-build \
    g++ \
    make \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source
COPY . .

# Build (Release) and keep build dir for subsequent runs
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build -j

# For the HTTP API server (optional at runtime; compose maps this).
EXPOSE 8081

# Default: run the full test suite
CMD ["ctest", "--test-dir", "build", "--output-on-failure"]


