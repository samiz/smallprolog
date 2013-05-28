clauses

member(X,pair(X,T)).
member(X,pair(A,T)):-member(X,T).

main():-member(12,pair(11,pair(12,pair(13,nil())))).
