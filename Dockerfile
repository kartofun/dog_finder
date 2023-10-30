FROM python:3.8

RUN apt update && apt install -y \
    git  \
    g++  \
    make  \
    cmake  \
    python3-dev  \
    && rm -rf /var/lib/apt/lists/*



FROM gcc:9.2
RUN set -ex;              \
    cd ./main_repo/build;  \
    cmake ..; make;

CMD uvicorn main:app --reload