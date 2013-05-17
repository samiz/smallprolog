clauses

insertSorted(X,[],[X]).
insertSorted(X,[H|T],[X,H|T]):-<(X,H).
insertSorted(X,[H|T],[H|T1]):->=(X,H), insertSorted(X,T,T1).

sort([], []).
sort([H|T],Result):- sort(T,T1),insertSorted(H,T1,Result).

main():- sort([1,2,3,4],L1),
         sort([4,3,1,5],L2),
         sort([9,8,7,6],L3).
