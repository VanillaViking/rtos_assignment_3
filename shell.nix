{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
buildInputs = [ gnumake gcc gdb typst ]; # your dependencies here
}
