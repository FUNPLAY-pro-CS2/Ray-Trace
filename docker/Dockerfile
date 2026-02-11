FROM registry.gitlab.steamos.cloud/steamrt/sniper/sdk:latest

WORKDIR /app

RUN apt update && apt install -y \
    build-essential \
    g++ \
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
    curl \
    && curl -sSL https://packages.microsoft.com/config/debian/11/packages-microsoft-prod.deb -o packages-microsoft-prod.deb \
        && dpkg -i packages-microsoft-prod.deb \
        && rm packages-microsoft-prod.deb \
        && apt update && apt install -y dotnet-sdk-8.0

RUN git config --system --add safe.directory '*'

COPY docker/docker-entrypoint.sh ./docker-entrypoint.sh
RUN sed -i 's/\r$//' ./docker-entrypoint.sh && chmod +x ./docker-entrypoint.sh

WORKDIR /app/source

CMD ["/bin/bash", "../docker-entrypoint.sh"]