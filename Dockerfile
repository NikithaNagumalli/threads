FROM gcc:latest

COPY main.cpp /app/

WORKDIR /app/

RUN g++ -o main main.cpp

CMD ["./main"]
