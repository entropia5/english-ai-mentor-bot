FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        libcurl4-openssl-dev \
        libpqxx-dev \
        nlohmann-json3-dev \
        pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B /tmp/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DENGLISH_MENTOR_BUILD_TESTS=OFF \
        -DENGLISH_MENTOR_WARNINGS_AS_ERRORS=ON \
    && cmake --build /tmp/build --parallel 2

FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive \
    MIGRATIONS_DIR=/app/migrations \
    RENDER_RESOURCES_DIR=/app/resources/rendering

RUN apt-get update \
    && apt-get install --yes --no-install-recommends \
        ca-certificates \
        libcurl4 \
        libpqxx-dev \
        wkhtmltopdf \
    && rm -rf /var/lib/apt/lists/* \
    && groupadd --system mentor \
    && useradd --system --gid mentor --home-dir /app mentor

WORKDIR /app

COPY --from=builder /tmp/build/english_mentor /app/english_mentor
COPY resources /app/resources
COPY migrations /app/migrations

RUN mkdir -p /app/data \
    && chown -R mentor:mentor /app

USER mentor

ENTRYPOINT ["/app/english_mentor"]
