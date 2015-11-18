CC = gcc
#CPP = g++
CMP = diff -s
RM  = rm -rf

GOLDFILE = golden.pgm
#INFILE = input_small.pgm    
INFILE = input_big.pgm    
OUTFILE = output_edge.pgm

#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# ---- for compiling
#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# ---- the original make command
susan: *.c
	$(CC) -static -O4 -o susan *.c -lm  -g 

# ---- compiling the intact original file
susan_original: susan_original.c Makefile
	$(CC) -static -O4 -o susan_original  /checkpoint/susan_original.c -lm  -g 

# ---- allows for debugging 
susan_debug: susan.c Makefile
	$(CC) -static -O4 -o susan *.c -lm  -g  -v -da -Q

debug_helpers:
	$(CC) debug_helpers.c debug_helpers_test.c -o debug_helpers -g

#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# ---- for running
#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# ---- for debugging in gdb
run_debug:
	gdb --args ./susan $(INFILE) $(OUTFILE) -e 

# ---- running the code with edth dietction simply
run_test:
	./susan $(INFILE) $(OUTFILE) -e
	$(CMP) $(OUTFILE) $(GOLDFILE)

# ---- copy of the run:
test:
	./susan $(INFILE) $(OUTFILE) -e 
	$(CMP) $(OUTFILE) $(GOLDFILE)

# ---- running the orignal one
run_original:
	./susan_original  $(INFILE) $(OUTFILE) -e 
	$(CMP) $(OUTFILE) $(GOLDFILE)


#debug_helpers:


#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# ---- house keeping
#------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
clean:
	$(RM) susan $(OUTFILE)
