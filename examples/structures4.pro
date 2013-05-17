domains
list=nil();pair(any,any).
clauses

member(X,[X|_]).
member(X,[_|T]):-member(X,T).

main():-member(X,[11,12,13]).
