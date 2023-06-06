FROM ubuntu:latest
ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update
RUN apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools -y
RUN apt-get install build-essential -y
RUN apt-get install -y sqlite3 libsqlite3-dev
EXPOSE 33333
WORKDIR /root/
RUN mkdir Gunin_221_353
WORKDIR /root/Gunin_221_353/
COPY *.cpp /root/Gunin_221_353/
COPY *.h /root/Gunin_221_353/
COPY *.pro.user /root/Gunin_221_353/
COPY . /root/Gunin_221_353
RUN qmake echoserver.pro
RUN make
ENTRYPOINT ["./echoserver"]