domains
list=nil();pair(any,any).
clauses

member(X,[X|T]).
member(X,[A|T]):-member(X,T).

main():-member(12,[11,12,13]).
