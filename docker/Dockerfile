FROM registry.gitlab.steamos.cloud/steamrt/sniper/sdk:latest

WORKDIR /app

RUN echo "deb http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-21 main" >> /etc/apt/sources.list.d/llvm.list
RUN echo "deb-src http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-21 main" >> /etc/apt/sources.list.d/llvm.list
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
RUN apt update && apt install -y clang-21
RUN ln -sf /usr/bin/clang-21 /usr/bin/clang && ln -sf /usr/bin/clang++-21 /usr/bin/clang++

RUN apt update && apt install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    zlib1g \
    zlib1g-dev \
    libssl-dev \
    libprotobuf-dev \
    protobuf-compiler \
    pkg-config \
    libcurl4-openssl-dev \
    libmaxminddb-dev \
    curl

RUN git config --system --add safe.directory '*'

COPY docker/docker-entrypoint.sh ./docker-entrypoint.sh
RUN sed -i 's/\r$//' ./docker-entrypoint.sh && chmod +x ./docker-entrypoint.sh

WORKDIR /app/source

CMD ["/bin/bash", "../docker-entrypoint.sh"]