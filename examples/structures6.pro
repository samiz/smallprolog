clauses

append([],L,L).
append([H|T],L,[H|T1]) :- append(T,L,T1).  

reverse([], []).
reverse([H|T], Ret):- reverse(T, T1), append(T1,[H], Ret).

main():-reverse([1,2,3,4,5],X).
