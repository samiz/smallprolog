clauses

join([], "").
join([H|T], Result):- join(T,T1), ++(H, T1, Result).

main():- join(["Hello ", "world", " things ", "good?"], X). 
