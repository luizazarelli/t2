CC     = gcc
CFLAGS = -std=c99 -fstack-protector-all -Wall -Wextra -g -DUNITY_INCLUDE_DOUBLE
LFLAGS = -lm

TST   = ../tst
UNITY = $(TST)/unity

OBJS = main.o libs.o quadra.o svg.o graph.o paths.o system.o qry.o

PROJECT = ted

ted: $(OBJS)
	$(CC) $(CFLAGS) -o ted $(OBJS) $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

UNITY_OBJ = $(UNITY)/unity.o

$(UNITY)/unity.o: $(UNITY)/unity.c
	$(CC) $(CFLAGS) -c $< -o $@

t_libs: $(UNITY_OBJ) $(TST)/t_libs.o libs.o
	$(CC) $(CFLAGS) -o $(TST)/t_libs $(UNITY_OBJ) $(TST)/t_libs.o libs.o $(LFLAGS)
	$(TST)/t_libs

t_quadra: $(UNITY_OBJ) $(TST)/t_quadra.o quadra.o libs.o
	$(CC) $(CFLAGS) -o $(TST)/t_quadra $(UNITY_OBJ) $(TST)/t_quadra.o quadra.o libs.o $(LFLAGS)
	$(TST)/t_quadra

t_graph: $(UNITY_OBJ) $(TST)/t_graph.o graph.o libs.o
	$(CC) $(CFLAGS) -o $(TST)/t_graph $(UNITY_OBJ) $(TST)/t_graph.o graph.o libs.o $(LFLAGS)
	$(TST)/t_graph

t_paths: $(UNITY_OBJ) $(TST)/t_paths.o paths.o graph.o libs.o
	$(CC) $(CFLAGS) -o $(TST)/t_paths $(UNITY_OBJ) $(TST)/t_paths.o paths.o graph.o libs.o $(LFLAGS)
	$(TST)/t_paths

t_svg: $(UNITY_OBJ) $(TST)/t_svg.o svg.o libs.o
	$(CC) $(CFLAGS) -o $(TST)/t_svg $(UNITY_OBJ) $(TST)/t_svg.o svg.o libs.o $(LFLAGS)
	$(TST)/t_svg

t_system: $(UNITY_OBJ) $(TST)/t_system.o system.o quadra.o svg.o graph.o paths.o libs.o
	$(CC) $(CFLAGS) -o $(TST)/t_system $(UNITY_OBJ) $(TST)/t_system.o system.o quadra.o svg.o graph.o paths.o libs.o $(LFLAGS)
	$(TST)/t_system

t_qry: $(UNITY_OBJ) $(TST)/t_qry.o qry.o system.o quadra.o svg.o graph.o paths.o libs.o
	$(CC) $(CFLAGS) -o $(TST)/t_qry $(UNITY_OBJ) $(TST)/t_qry.o qry.o system.o quadra.o svg.o graph.o paths.o libs.o $(LFLAGS)
	$(TST)/t_qry

tstall: t_libs t_quadra t_graph t_paths t_svg t_system t_qry

BED =
BSD =

run:
	$(PROJECT) -e $(BED) -f $(GEO) -q $(QRY) -v $(VIA) -o $(BSD)

clean:
	rm -f *.o $(TST)/*.o $(UNITY)/*.o $(PROJECT) $(TST)/t_libs $(TST)/t_quadra $(TST)/t_graph $(TST)/t_paths $(TST)/t_svg $(TST)/t_system $(TST)/t_qry

.PHONY: clean tstall run t_libs t_quadra t_graph t_paths t_svg t_system t_qry
