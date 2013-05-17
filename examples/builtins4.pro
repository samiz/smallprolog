clauses

fib(0,1).
fib(1,1).
fib(A,B):- >(A,1),-(A,1,A2),-(A,2,A3),fib(A2,B2),fib(A3,B3),+(B2,B3,B).

main():-fib(15,X).
