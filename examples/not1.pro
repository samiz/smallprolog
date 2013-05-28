clauses

member(X,[X|T]).
member(X,[A|T]):-member(X,T).

main():-not(member(12,[11,12,13])),X=12.
