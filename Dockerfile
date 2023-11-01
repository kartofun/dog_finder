FROM ubuntu:18.04

RUN apt update && apt install -y \
    git  \
    g++ \
    make  \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Install requirements
RUN apt-get update && apt-get install -y \
    # OpenCV dependencies
    build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev \
    python3-dev python3-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev \
    libcanberra-gtk-module libcanberra-gtk3-module

# Clone, build and install OpenCV
RUN git clone https://github.com/opencv/opencv.git && \
    cd /opencv && mkdir build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. && \
    make -j"$(nproc)" && \
    make install && \
    ls && cd .. && cd .. && \
    rm -rf /opencv

COPY main_repo .

RUN apt update && apt install -y \
    python-pip \
    python3-venv
    
#RUN python3 -m venv /venv

#ENV PATH=/venv/bin:$PATH



RUN apt update && apt install -y \
    python3.8-dev \
    python3-pip

RUN update-alternatives --install /usr/bin/python3 python /usr/bin/python3.8 1

RUN pip3 install --upgrade pip
RUN pip3 install -r ./requirements.txt

RUN pip3 uninstall multipart -y && pip3 install python-multipart

RUN         cd src && ls && cd .. && cd build && ls &&\
            cmake ../src && make && \
            cd .. && \
            ls && echo $LANG && echo $LC_LANG && python --version && python3 --version && python3.8 --version &&    \
            cd ./lib && ls && cd .. && ls

#RUN apt-get install -y python3.8-venv

#RUN python3.8 -m venv /venv

#ENV PATH=/venv/bin:$PATH

#RUN pip3 install -r ./requirements.txt

#RUN rm -f /usr/bin/python && ln -s /usr/bin/python /usr/bin/python3.8
#RUN ln -s /usr/bin/python3 /usr/bin/python3.8

RUN export LC_ALL=C.UTF-8
ENV LC_ALL=C.UTF-8

EXPOSE 8000


#CMD  uvicorn main:app --reload
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]