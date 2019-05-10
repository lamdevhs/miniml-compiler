all: comp simu

# Compilation of Ocaml files
# Attention: order of object files important
comp: miniml.cmo parser.cmo lexer.cmo tools.cmo encoder.cmo flattener.cmo codeGenerator.cmo interf.cmo comp.cmo
	ocamlc -o comp $^

# Compilation of .ml files
miniml.cmo: miniml.ml
	ocamlc -c $<

instrs.cmo:  instrs.ml miniml.cmo
	ocamlc -c $<

interf.cmo: interf.ml parser.cmo lexer.cmo
	ocamlc -c $<

comp.cmo: comp.ml codeGenerator.cmo interf.cmo
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

simu: miniml.cmo parser.cmo lexer.cmo tools.cmo encoder.cmo interf.cmo simulator.cmo
	ocamlc -o simu $^

simulator.cmo: simulator.ml miniml.cmo encoder.cmo
	ocamlc -c $<

tools.cmo:  tools.ml
	ocamlc -c $<

encoder.cmo:  encoder.ml miniml.cmo tools.cmo
	ocamlc -c $<

flattener.cmo:  flattener.ml encoder.cmo
	ocamlc -c $<

codeGenerator.cmo:  codeGenerator.ml flattener.cmo
	ocamlc -c $<

#### Generic rules

%.cmi: %.mli
	ocamlc -c $<

.PHONY: clean

clean:
	rm -f lexer.ml parser.ml parser.output *.mli *.cmi *.cmo
