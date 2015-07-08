#cumulative n.


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Actions
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

packageat(P,L,I+1) :- deliverpackage(P,L,I), room(L), I=0..n-2.
:- deliverpackage(P,L,I), not at(L,I), I=0..n-1.
:- deliverpackage(P,L,I), packageat(P,L,I), I=0..n-1.
:- deliverpackage(P,L,I), not packageat(P,robot,I), I=0..n-1.


packageat(P,robot,I+1) :- receivepackage(P,I),packageat(P,L,I),room(L), I=0..n-2.
:- receivepackage(P,I), packageat(P,L,I),  not at(L,I), room(L), I=0..n-1.
:- receivepackage(P,I), packageat(P,robot,I),  I=0..n-1.


%:- receivepackage(C,P,I), approach(D,J), J+1==I, I=0..n-1.
%:- receivepackage(C,P,I), opendoor(D,J), J+1==I, I=0..n-1.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Inertia
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% received is inertial
packageat(P,L,I+1) :- packageat(P,L,I), not -packageat(P,L,I+1), I=0..n-2.
-packageat(P,L,I+1) :- -packageat(P,L,I), not packageat(P,L,I+1), I=0..n-2.

