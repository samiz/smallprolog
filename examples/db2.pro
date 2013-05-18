facts
parent(symbol, symbol)
clauses

main():-
	A=parent(samy, mohamed),
    assert(A),
    B=parent(mohamed,mira),
    assert(B),
    parent(X,Y).
