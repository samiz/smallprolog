facts
parent(symbol, symbol)
clauses

main():-
    assert(parent(samy, mohamed)),
    assert(parent(mohamed,mira)),
    delete(parent(mohamed,X)),
    parent(A,B).
