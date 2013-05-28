clauses

member(X,[X|T]).
member(X,[A|T]):-member(X,T).

main():-not(member(12,[11,13,14])),X=12.
