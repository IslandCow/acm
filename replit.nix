{ pkgs }: {
	deps = [
   pkgs.vim
		pkgs.clang
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}