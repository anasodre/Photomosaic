# Configura o compilador
CC = gcc

# Gera warnings detalhadas ao compilar
CFLAGS = -Wall

# objetos a serem compilados
objs = mosaico.o fotolib.o fileReader.o

# Regra principal
all: mosaico

# Regras de ligacao
mosaico: $(objs) -lm

# Regras de compilacao
mosaico.o: mosaico.c fotolib.h fileReader.h
fotolib.o: fotolib.c fotolib.h fileReader.h
fileReader.o: fileReader.c fileReader.h

# Remove arquivos temporarios
clean:
	-rm -f $(objs) *~

# Remove todos os arquivos do programa sem ser o codigo fonte
purge: clean
	-rm -f mosaico