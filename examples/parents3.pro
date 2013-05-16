clauses

parent(pat, sam).
parent(pat, jen).
parent(matt, bill).
parent(jen, kim).
parent(sam, mika).

ancestor(A,B):-parent(A,B).
ancestor(A,B):-parent(A,C),ancestor(C, B).

main():- ancestor(pat, X).
