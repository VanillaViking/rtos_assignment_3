{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
buildInputs = [ gnumake zip gcc gdb typst ]; # your dependencies here
}
