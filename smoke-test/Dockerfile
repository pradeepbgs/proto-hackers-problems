FROM gcc:14

WORKDIR /app

COPY main.c .

RUN gcc -O2 -Wall -Wextra -pthread main.c -o echo-server

ENV PORT=8080

EXPOSE 8080

CMD ["./echo-server"]