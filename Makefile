all: comp simu

# Compilation of Ocaml files
# Attention: order of object files important
comp: miniml.cmo parser.cmo lexer.cmo instrs.cmo interf.cmo comp.cmo
	ocamlc -o comp $^

# Compilation of .ml files
miniml.cmo: miniml.ml
	ocamlc -c $<

instrs.cmo:  instrs.ml miniml.cmo
	ocamlc -c $<

interf.cmo: interf.ml parser.cmo lexer.cmo instrs.cmo
	ocamlc -c $<

comp.cmo: comp.ml instrs.cmo parser.cmo interf.cmo
	ocamlc -c $<


# ocaml lexer and parser
lexer.ml: lexer.mll miniml.cmo
	ocamllex $<

parser.ml parser.mli: parser.mly miniml.cmo
	ocamlyacc -v $<

lexer.cmo: lexer.ml parser.cmo
	ocamlc -c $<
parser.cmo: parser.ml parser.cmi miniml.cmo
	ocamlc -c $<

## >>> Added to the original Makefile:
simu: miniml.cmo parser.cmo lexer.cmo instrs.cmo interf.cmo simulator.cmo
	ocamlc -o simu $^

simulator.cmo: simulator.ml miniml.cmo instrs.cmo
	ocamlc -c $<
## <<< end

#### Generic rules

%.cmi: %.mli
	ocamlc -c $<


.PHONY: clean

clean:
	rm -f lexer.ml parser.ml parser.output *.mli *.cmi *.cmo
