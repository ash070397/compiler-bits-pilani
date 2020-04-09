add:
	gcc -o stage1exe driver.c lexer.c parser.c ast.c symbolTable.c codegen.c codegen_driver.c
	